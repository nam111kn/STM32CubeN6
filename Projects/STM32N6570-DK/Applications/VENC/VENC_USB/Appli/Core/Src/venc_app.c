/**
******************************************************************************
* @file    main.c
* @author  MCD Application Team
* @brief   This project is a HAL template project for STM32N6xx devices.
******************************************************************************
* @attention
*
* Copyright (c) 2023 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "ewl.h"
#include "h264encapi.h"
#include "venc_app.h"
#include "imx335.h"
#include "stm32n6xx_ll_venc.h"
#include "stm32n6570_discovery.h"
#include "stm32n6570_discovery_camera.h"
#include "tx_api.h"
#include "utils.h"
#include "venc_h264_config.h"

/** @addtogroup Templates
* @{
*/

/** @addtogroup HAL
* @{
*/

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  uint32_t size;
  uint32_t * block_addr;
  uint32_t * aligned_block_addr;
} venc_output_frame_t;
/* Private define ------------------------------------------------------------*/

#define VENC_APP_QUEUE_SIZE 15
#define VENC_OUTPUT_BLOCK_SIZE 0x30000
#define VENC_OUTPUT_BLOCK_NBR 10


/* Private macro -------------------------------------------------------------*/

#define ALIGNED(ptr, bytes) (((ptr + bytes - 1)/bytes) * bytes)
/* Private variables ---------------------------------------------------------*/


extern DCMIPP_HandleTypeDef hcamera_dcmipp;


static H264EncIn encIn= {0};
	static H264EncOut encOut= {H264ENC_INTRA_FRAME,0, 0, 0, 0, 0, 0, 0,  H264ENC_NO_REFERENCE_NO_REFRESH,  H264ENC_NO_REFERENCE_NO_REFRESH};
static H264EncInst encoder= {0};
static uint32_t frame_nb = 0;

/* Input Frame : in internal ram */
venc_output_frame_t queue_buf[VENC_OUTPUT_BLOCK_NBR];
uint8_t output_block_buffer[VENC_OUTPUT_BLOCK_NBR * VENC_OUTPUT_BLOCK_SIZE] ALIGN_32 __NON_CACHEABLE ;

TX_EVENT_FLAGS_GROUP venc_app_flags;
TX_QUEUE enc_frame_queue;
TX_BLOCK_POOL venc_block_pool;

/* Private function prototypes -----------------------------------------------*/
static int encoder_prepare(void);
static int encode_frame(void);
static int encoder_end(void);
static int encoder_start(void);


void venc_thread_func(ULONG arg){
  ULONG flags;
  /* initialize LEDs to signal processing is ongoing */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  if(tx_event_flags_create(&venc_app_flags, "venc_app_events") != TX_SUCCESS)
  {
    return ;
  }
  if(tx_queue_create(&enc_frame_queue, "ENC frame queue", sizeof(venc_output_frame_t)/4, &queue_buf, sizeof(queue_buf)) != TX_SUCCESS)
  {
    Error_Handler();
  }

  if(tx_block_pool_create(&venc_block_pool, "venc output bock pool", VENC_OUTPUT_BLOCK_SIZE,
                          output_block_buffer, VENC_OUTPUT_BLOCK_NBR * VENC_OUTPUT_BLOCK_SIZE) != TX_SUCCESS)
  {
    Error_Handler();
  }

   /* Initialize camera */
  if(BSP_CAMERA_Init(0,CAMERA_R2592x1944, CAMERA_PF_RAW_RGGB10) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  IMX335_SetFramerate(Camera_CompObj, hVencH264Instance.cfgH264Main.frameRateNum /hVencH264Instance.cfgH264Main.frameRateDenom);
  /* initialize VENC */
  LL_VENC_Init();

  /* initialization done. Turn on the LEDs */
  BSP_LED_On(LED1);
  BSP_LED_On(LED2);

  encoder_prepare();
  VENC_APP_EncodingStart();

  while(1)
  {
    tx_event_flags_get(&venc_app_flags, VIDEO_START_FLAG, TX_AND, &flags, TX_WAIT_FOREVER);
    if(BSP_CAMERA_BackgroundProcess() != BSP_ERROR_NONE)
    {
      printf("Error in BSP image processing\n");
    }
    tx_event_flags_get(&venc_app_flags, FRAME_RECEIVED_FLAG, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
    if(!encode_frame())
    {
      BSP_LED_Toggle(LED_GREEN);
    }
  }
}

int encoder_prepare(void)
{
  H264EncRet ret;
  frame_nb = 0;

  /* Set encode configuration */
  ret = H264EncInit(&hVencH264Instance.cfgH264Main, &encoder);

  if (ret != H264ENC_OK)
  {
    return -1;
  }


  /* Set preprocessing*/
  ret = H264EncSetPreProcessing(encoder, &hVencH264Instance.cfgH264Preproc);
  if(ret != H264ENC_OK)
  {
    return -1;
  }
  ret = H264EncSetCodingCtrl(encoder, &hVencH264Instance.cfgH264Coding);
  if(ret != H264ENC_OK)
  {
    return -1;
  }

  /* set bit rate configuration */
  ret = H264EncSetRateCtrl(encoder, &hVencH264Instance.cfgH264Rate);
  if(ret != H264ENC_OK)
  {
    return -1;
  }
  return 0;

}

static void  encoder_reset(void)
{
  if (HAL_DCMIPP_PIPE_Suspend(&hcamera_dcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    printf("HAL_DCMIPP_PIPE_Suspend failed\n");
  }

  /* Wait for end of trame */
  tx_thread_sleep(15 * TX_TIMER_TICKS_PER_SECOND / 1000);


  /* VENV HW REset */
  __HAL_RCC_VENC_FORCE_RESET();
  tx_thread_sleep(1 * TX_TIMER_TICKS_PER_SECOND / 1000);
  __HAL_RCC_VENC_RELEASE_RESET();
  tx_thread_sleep(1 * TX_TIMER_TICKS_PER_SECOND / 1000);

  /*Resume DCMIPP after VENC reset*/
  if (HAL_DCMIPP_PIPE_Resume(&hcamera_dcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    printf("HAL_DCMIPP_PIPE_Resume failed");
  }

}

static int encoder_start(void)
{
  H264EncRet ret;
  venc_output_frame_t frame_buffer = {0};


  /* start camera acquisition */
  if(BSP_CAMERA_Start(0,  GetInputFrame(NULL), CAMERA_MODE_CONTINUOUS)!= BSP_ERROR_NONE){
    Error_Handler();
  }

  if(tx_block_allocate(&venc_block_pool, (void **) &frame_buffer.block_addr, TX_NO_WAIT) != TX_SUCCESS)
  {
    return -1;
  }
  frame_buffer.aligned_block_addr = (uint32_t *) ALIGNED((uint32_t) frame_buffer.block_addr, 8);
  encIn.pOutBuf = frame_buffer.aligned_block_addr;
  encIn.busOutBuf = (uint32_t) encIn.pOutBuf;
  encIn.outBufSize = VENC_OUTPUT_BLOCK_SIZE;

  /* create stream */
  ret = H264EncStrmStart(encoder, &encIn, &encOut);
  if (ret != H264ENC_OK)
  {
    return -1;
  }
  frame_buffer.size = encOut.streamSize;
  if(tx_queue_send(&enc_frame_queue, (void *) &frame_buffer, TX_NO_WAIT) != TX_SUCCESS)
  {
    tx_block_release(frame_buffer.block_addr);
  }
  encIn.codingType = H264ENC_INTRA_FRAME;
  return 0;
}

__weak void timeMonitor(void) {};
uint32_t nb_encoded_frame = 0;

static int encode_frame(void)
{
  venc_output_frame_t frame_buffer = {0};
  int ret = H264ENC_FRAME_READY;
  if (!(frame_nb % 30))
  {
    /* if frame is the first : set as intra coded */
    encIn.codingType = H264ENC_INTRA_FRAME;
  }
  else
  {
    /* if there was a frame previously, set as predicted */
    encIn.timeIncrement = 1;
    encIn.codingType = H264ENC_PREDICTED_FRAME;
  }
  encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
  encIn.ltrf = H264ENC_REFERENCE;
  /* set input buffers to structures */
   encIn.busLuma = (uint32_t)GetInputFrame(NULL);

  /* allocate and set output buffer */

  if(tx_block_allocate(&venc_block_pool, (void **) &frame_buffer.block_addr, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    printf("VENC : failed to allocate output buffer\n");
    return -1;
  }
  frame_buffer.aligned_block_addr = (uint32_t *) ALIGNED((uint32_t) frame_buffer.block_addr, 8);
  encIn.pOutBuf = frame_buffer.aligned_block_addr;
  encIn.busOutBuf = (uint32_t) encIn.pOutBuf;
  encIn.outBufSize = VENC_OUTPUT_BLOCK_SIZE - 8;
  ret = H264EncStrmEncode(encoder, &encIn, &encOut, NULL, NULL, NULL);

  timeMonitor();

  switch (ret)
  {
  case H264ENC_FRAME_READY:
    /*save stream */
    if(encOut.streamSize == 0)
    {
      encIn.codingType = H264ENC_INTRA_FRAME;
      tx_block_release(frame_buffer.block_addr);
      return -1;
    }
    frame_buffer.size = encOut.streamSize;
    if(tx_queue_send(&enc_frame_queue, (void *) &frame_buffer, TX_NO_WAIT) != TX_SUCCESS)
    {
      tx_block_release(frame_buffer.block_addr);
    }
    encIn.codingType = H264ENC_PREDICTED_FRAME;
     nb_encoded_frame++;
    break;
  case H264ENC_FUSE_ERROR:
    printf("DCMIPP and VENC desync (frame#%ld), restart the video\n", frame_nb);
    tx_block_release(frame_buffer.block_addr);
    encoder_reset();
    break;
  default:
    printf("error encoding frame %d\n", ret);
    tx_block_release(frame_buffer.block_addr);
    encIn.codingType = H264ENC_INTRA_FRAME;
    return -1;
    break;
  }
  frame_nb++;
  return 0;
}


static int encoder_end(void){
  BSP_CAMERA_Stop(0);
  int ret = H264EncStrmEnd(encoder, &encIn, &encOut);
  if (ret != H264ENC_OK)
  {
    return -1;
  }
  tx_queue_flush(&enc_frame_queue);
  tx_block_pool_delete(&venc_block_pool);
  return 0;
}


HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp)
{
 RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Configure DCMIPP ck_ker_dcmipp to ic17 with PLL2 (1000MHz) / 3 = 333MHz */
  PeriphClkInit.PeriphClockSelection |= RCC_PERIPHCLK_DCMIPP;
  PeriphClkInit.DcmippClockSelection = RCC_DCMIPPCLKSOURCE_IC17;
  PeriphClkInit.ICSelection[RCC_IC17].ClockSelection = RCC_ICCLKSOURCE_PLL1;
  PeriphClkInit.ICSelection[RCC_IC17].ClockDivider = 4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    return HAL_ERROR;
  }
  LL_RCC_IC18_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC18_SetDivider(60);   /* 800/40=20Mhz */
  LL_RCC_IC18_Enable();

  return HAL_OK;
}

static HAL_StatusTypeDef  dcmipp_downsize(DCMIPP_HandleTypeDef *hdcmipp,int32_t pipe, int32_t camWidth,int32_t camHeight,int32_t captureWidth,int32_t captureHeight)
{
  /* Calcultation explained in RM0486v2 table 354*/

  DCMIPP_DownsizeTypeDef DonwsizeConf ={0};
  /* Configure the downsize */
  DonwsizeConf.HRatio      = (uint32_t)((((float)(camWidth)) / ((float)(captureWidth))) * 8192.F);
  DonwsizeConf.VRatio      = (uint32_t)((((float)(camHeight )) / ((float)(captureHeight ))) * 8192.F);
  DonwsizeConf.HSize       = captureWidth;
  DonwsizeConf.VSize       = captureHeight;

  DonwsizeConf.HDivFactor  = (uint32_t)floor((1024 * 8192 -1) / DonwsizeConf.HRatio);
  DonwsizeConf.VDivFactor  = (uint32_t)floor((1024 * 8192 -1) / DonwsizeConf.VRatio);

  if(HAL_DCMIPP_PIPE_SetDownsizeConfig(hdcmipp, pipe, &DonwsizeConf) != HAL_OK) return HAL_ERROR;
  if(HAL_DCMIPP_PIPE_EnableDownsize(hdcmipp, pipe)!= HAL_OK) return HAL_ERROR;
  return HAL_OK;
}

static HAL_StatusTypeDef  dcmipp_enable_HW_Handshake(DCMIPP_HandleTypeDef *hdcmipp)
{
  if (HAL_DCMIPP_PIPE_SetLineWrappingConfig(hdcmipp, DCMIPP_PIPE1, DCMIPP_WRAP_ADDRESS_64_LINES) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_DCMIPP_PIPE_EnableLineWrapping(hdcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    return HAL_ERROR;
  }
 /* Enable line event with no interrupt to use the hardware trigger to the video encoder  */
  MODIFY_REG(DCMIPP->P1PPCR, DCMIPP_P1PPCR_LINEMULT_Msk,DCMIPP_MULTILINE_32_LINES);
  return HAL_OK;
}

HAL_StatusTypeDef MX_DCMIPP_Init(DCMIPP_HandleTypeDef *hdcmipp)
{
  DCMIPP_PipeConfTypeDef pPipeConf = {0};
  DCMIPP_CSI_PIPE_ConfTypeDef pCSIPipeConf = {0};
  DCMIPP_CSI_ConfTypeDef csiconf = {0};
   HAL_StatusTypeDef ret = HAL_OK;

  if (HAL_DCMIPP_Init(hdcmipp) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Configure the CSI */
  csiconf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csiconf.NumberOfLanes   = DCMIPP_CSI_TWO_DATA_LANES;
  csiconf.PHYBitrate      = DCMIPP_CSI_PHY_BT_1600;
  HAL_DCMIPP_CSI_SetConfig(hdcmipp, &csiconf);

  /* Configure the Virtual Channel 0 */
  /* Set Virtual Channel config */
  HAL_DCMIPP_CSI_SetVCConfig(hdcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP10);


  /* Configure the serial Pipe */
  pCSIPipeConf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  pCSIPipeConf.DataTypeIDA  = DCMIPP_DT_RAW10;
  pCSIPipeConf.DataTypeIDB  = DCMIPP_DT_RAW10; /* Don't Care */


  if (HAL_DCMIPP_CSI_PIPE_SetConfig(hdcmipp, DCMIPP_PIPE1, &pCSIPipeConf) != HAL_OK)
  {
    return HAL_ERROR;
  }

   pPipeConf.FrameRate  = DCMIPP_FRAME_RATE_ALL;  /* Sensor framerate is set with IMX335_SetFramerate(..FRAMERATE);*/
   pPipeConf.PixelPackerFormat = hDcmippH264Instance.format;

  /* Set Pitch for Main and Ancillary Pipes */
   pPipeConf.PixelPipePitch  =hDcmippH264Instance.pitch; /* Number of bytes */

  /* Configure Pipe */
  if (HAL_DCMIPP_PIPE_SetConfig(hdcmipp, DCMIPP_PIPE1, &pPipeConf) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (HAL_DCMIPP_PIPE_EnableRedBlueSwap(hdcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    return HAL_ERROR;
  }
  #define N10(val) (((val) ^ 0x7FF) + 1)
  DCMIPP_ColorConversionConfTypeDef color_conf = {
    .ClampOutputSamples = ENABLE,
    .OutputSamplesType = DCMIPP_CLAMP_YUV,
    .RR = N10(26), .RG = N10(87), .RB = 112, .RA = 128,
    .GR = 47, .GG = 157, .GB = 16, .GA = 16,
    .BR = 112, .BG = N10(102), .BB = N10(10), .BA = 128,
  };
  if (HAL_DCMIPP_PIPE_SetYUVConversionConfig(hdcmipp, DCMIPP_PIPE1, &color_conf) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (HAL_DCMIPP_PIPE_EnableYUVConversion(hdcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Configure DCMIPP output size*/
  ret= dcmipp_downsize(hdcmipp,DCMIPP_PIPE1,
                        hCamH264Instance.width,
                        hCamH264Instance.height,
                        hVencH264Instance.cfgH264Main.width,
                        hVencH264Instance.cfgH264Main.height);

  /* Specific Hardware Handshake if needed*/
  if (ret == HAL_OK && hVencH264Instance.cfgH264Coding.inputLineBufHwModeEn)
  {
    ret = dcmipp_enable_HW_Handshake(hdcmipp);
  }

  return ret;

}

void BSP_CAMERA_FrameEventCallback(uint32_t instance)
{
  /* signal new frame*/
  tx_event_flags_set(&venc_app_flags, FRAME_RECEIVED_FLAG, TX_OR);
}



/* Functions. */
void VENC_APP_EncodingStart(void)
{
  /* initialize encoder software for camera feed encoding */
  encoder_start();
  tx_event_flags_set(&venc_app_flags, VIDEO_START_FLAG, TX_OR);
}

UINT VENC_APP_GetData(UCHAR **data, ULONG *size)
{
  static uint32_t * curr_block = NULL;
  if(curr_block)
  {
    tx_block_release(curr_block);
    curr_block = NULL;
  }
  venc_output_frame_t frame_block;
  if(tx_queue_receive(&enc_frame_queue, (void *) &frame_block, TX_NO_WAIT) != TX_SUCCESS)
  {
    *data = NULL;
    *size = 0;
    return(-1);
  }
  *data = (UCHAR *) frame_block.aligned_block_addr;
  *size = frame_block.size + 15;
  curr_block = frame_block.block_addr;
  return(0);
}

UINT VENC_APP_EncodingStop(void)
{
  ULONG flags;
  tx_event_flags_get(&venc_app_flags, VIDEO_START_FLAG, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
  (void) encoder_end();
  return(0);
}
