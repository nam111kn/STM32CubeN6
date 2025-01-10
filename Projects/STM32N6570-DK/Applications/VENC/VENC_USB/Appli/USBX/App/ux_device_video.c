/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_video.c
  * @author  MCD Application Team
  * @brief   USBX Device Video applicative source file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ux_device_video.h"
#include "venc_app.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define UVC_PLAY_STATUS_STOP       0x00U
#define UVC_PLAY_STATUS_READY      0x01U
#define UVC_PLAY_STATUS_STREAMING  0x02U
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint32_t payload_count;
ULONG uvc_state;
UX_DEVICE_CLASS_VIDEO *video;
UX_DEVICE_CLASS_VIDEO_STREAM *stream_write;
UCHAR video_frame_buffer[1024];
uint8_t new_frame_requested = 1;
/* Video Probe data structure */
static USBD_VideoControlTypeDef video_Probe_Control =
{
  .bmHint = 0x0000U,
  .bFormatIndex = 0x01U,
  .bFrameIndex = 0x01U,
  .dwFrameInterval = UVC_INTERVAL(UVC_CAM_FPS_FS),
  .wKeyFrameRate = 0x0000U,
  .wPFrameRate = 0x0000U,
  .wCompQuality = 0x0000U,
  .wCompWindowSize = 0x0000U,
  .wDelay = 0x0000U,
  .dwMaxVideoFrameSize = 0x0000U,
  .dwMaxPayloadTransferSize = 0x00000000U,
  .dwClockFrequency = 0x00000000U,
  .bmFramingInfo = 0x00U,
  .bPreferedVersion = 0x00U,
  .bMinVersion = 0x00U,
  .bMaxVersion = 0x00U,
  .bUsage = 0x1U,
  .bBitDepthLuma = 0x0U,
  .bmSettings = 0x0U,
  .bMaxNumberOfRefFramesPlus1 = 0x1U,
  .bmRateControlModes = 0x0U,
  .bmLayoutPerStream = 0x0U,
};

/* Video Commit data structure */
static USBD_VideoControlTypeDef video_Commit_Control =
{
  .bmHint = 0x0000U,
  .bFormatIndex = 0x01U,
  .bFrameIndex = 0x01U,
  .dwFrameInterval = UVC_INTERVAL(UVC_CAM_FPS_FS),
  .wKeyFrameRate = 0x0000U,
  .wPFrameRate = 0x0000U,
  .wCompQuality = 0x0000U,
  .wCompWindowSize = 0x0000U,
  .wDelay = 0x0000U,
  .dwMaxVideoFrameSize = 0x0000U,
  .dwMaxPayloadTransferSize = 0x00000000U,
  .dwClockFrequency = 0x00000000U,
  .bmFramingInfo = 0x00U,
  .bPreferedVersion = 0x00U,
  .bMinVersion = 0x00U,
  .bMaxVersion = 0x00U,
};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID video_write_payload(UX_DEVICE_CLASS_VIDEO_STREAM *stream);
UINT usb_sender_session_h264_send(UX_DEVICE_CLASS_VIDEO_STREAM *stream, UCHAR *frame_data, ULONG frame_data_size, ULONG timestamp, UINT marker);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


/**
  * @brief  usb_video_data_read
  * @param  data: Pointer to the video Data.
  * @param  size: Pointer to the video Data length.
  * @retval status
  */
UINT usb_video_data_read(UCHAR **data, ULONG *size)
{
  return VENC_APP_GetData(data, size);
}

/**
  * @brief  USBD_VIDEO_Activate
  *         This function is called when insertion of a Video device.
  * @param  video_instance: Pointer to the video class instance.
  * @retval none
  */
VOID USBD_VIDEO_Activate(VOID *video_instance)
{
  /* USER CODE BEGIN USBD_VIDEO_Activate */

  /* Save the video instance */
  video = (UX_DEVICE_CLASS_VIDEO*) video_instance;

  /* Get the streams instances */
  ux_device_class_video_stream_get(video, 0, &stream_write);
  //VENC_APP_EncodingStart();
  /* USER CODE END USBD_VIDEO_Activate */

  return;
}

/**
  * @brief  USBD_VIDEO_Deactivate
  *         This function is called when extraction of a Video device.
  * @param  video_instance: Pointer to the video class instance.
  * @retval none
  */
VOID USBD_VIDEO_Deactivate(VOID *video_instance)
{
  /* USER CODE BEGIN USBD_VIDEO_Deactivate */
  UX_PARAMETER_NOT_USED(video_instance);

  /* Reset the video instance */
  video = UX_NULL;
  //VENC_APP_EncodingStop();
  /* Reset the video streams */
  stream_write = UX_NULL;

  /* USER CODE END USBD_VIDEO_Deactivate */

  return;
}

/**
  * @brief  USBD_VIDEO_StreamChange
  *         This function is invoked to inform application that the
  *         alternate setting are changed.
  * @param  video_stream: Pointer to video class stream instance.
  * @param  alternate_setting: interface alternate setting.
  * @retval none
  */
VOID USBD_VIDEO_StreamChange(UX_DEVICE_CLASS_VIDEO_STREAM *video_stream,
                             ULONG alternate_setting)
{
  /* USER CODE BEGIN USBD_VIDEO_StreamChange */

  /* Stop video payload loop back if stream closed */
  if (alternate_setting == 0U)
  {
    /* Update State machine */
    uvc_state = UVC_PLAY_STATUS_STOP;

    return;
  }

  /* Update State machine */
  uvc_state = UVC_PLAY_STATUS_STREAMING;

  /* Write buffers until achieve threadshold */
  video_write_payload(video_stream);

  /* Start sending valid payloads in the Video class */
  ux_device_class_video_transmission_start(video_stream);

  /* USER CODE END USBD_VIDEO_StreamChange */

  return;
}

/**
  * @brief  USBD_VIDEO_StreamPayloadDone
  *         This function is invoked when stream data payload received.
  * @param  video_stream: Pointer to video class stream instance.
  * @param  length: transfer length.
  * @retval none
  */
VOID USBD_VIDEO_StreamPayloadDone(UX_DEVICE_CLASS_VIDEO_STREAM *video_stream,
                                  ULONG length)
{
  /* USER CODE BEGIN USBD_VIDEO_StreamPayloadDone */
  /* Check length is not NULL */
  if (length != 0U)
  {
    /* Update state machine */
    /* Prepare next payload */
    video_write_payload(video_stream);
  }
  /* USER CODE END USBD_VIDEO_StreamPayloadDone */

  return;
}

/**
  * @brief  USBD_VIDEO_StreamRequest
  *         This function is invoked to manage the UVC class requests.
  * @param  video_stream: Pointer to video class stream instance.
  * @param  transfer: Pointer to the transfer request.
  * @retval status
  */
UINT USBD_VIDEO_StreamRequest(UX_DEVICE_CLASS_VIDEO_STREAM *video_stream,
                              UX_SLAVE_TRANSFER *transfer)
{
   UINT status  = UX_SUCCESS;

  /* USER CODE BEGIN USBD_VIDEO_StreamRequest */
  UCHAR *data, bRequest;
  USHORT wValue_CS, wLength;

  /* Decode setup packet */
  bRequest = transfer -> ux_slave_transfer_request_setup[UX_SETUP_REQUEST];
  wValue_CS = transfer -> ux_slave_transfer_request_setup[UX_SETUP_VALUE + 1];
  wLength = ux_utility_short_get(transfer -> ux_slave_transfer_request_setup + UX_SETUP_LENGTH);
  data = transfer -> ux_slave_transfer_request_data_pointer;

  /* Check CS */
  switch(wValue_CS)
  {
    case UX_DEVICE_CLASS_VIDEO_VS_PROBE_CONTROL:

      switch(bRequest)
      {
        case UX_DEVICE_CLASS_VIDEO_SET_CUR:

          status = UX_SUCCESS;

          break;

        case UX_DEVICE_CLASS_VIDEO_GET_DEF:
        case UX_DEVICE_CLASS_VIDEO_GET_CUR:
        case UX_DEVICE_CLASS_VIDEO_GET_MIN:
        case UX_DEVICE_CLASS_VIDEO_GET_MAX:

          /* Update bPreferedVersion, bMinVersion and bMaxVersion which must be set only by Device */
          video_Probe_Control.bPreferedVersion = 0x00U;
          video_Probe_Control.bMinVersion = 0x00U;
          video_Probe_Control.bMaxVersion = 0x00U;
          video_Probe_Control.dwMaxVideoFrameSize = UVC_MAX_FRAME_SIZE;
          video_Probe_Control.dwClockFrequency = 0x02DC6C00U;

          /* Check device speed */
          if(_ux_system_slave->ux_system_slave_speed == UX_FULL_SPEED_DEVICE)
          {
            video_Probe_Control.dwFrameInterval = (UVC_INTERVAL(UVC_CAM_FPS_FS));
            video_Probe_Control.dwMaxPayloadTransferSize = USBD_VIDEO_EPIN_FS_MPS;
          }
          else
          {
            video_Probe_Control.dwFrameInterval = (UVC_INTERVAL(UVC_CAM_FPS_HS));
            video_Probe_Control.dwMaxPayloadTransferSize = USBD_VIDEO_EPIN_HS_MPS;
          }

          /* Copy data for transfer */
          ux_utility_memory_copy(data, (VOID *)&video_Probe_Control, sizeof(USBD_VideoControlTypeDef));

          /* Transfer request */
          status = ux_device_stack_transfer_request(transfer, UX_MIN(wLength, sizeof(USBD_VideoControlTypeDef)),
                                                    UX_MIN(wLength, sizeof(USBD_VideoControlTypeDef)));

          break;

        default:
          break;
      }
      break;

    case UX_DEVICE_CLASS_VIDEO_VS_COMMIT_CONTROL:

      /* Check request length */
      if (wLength < 26)
      {
        break;
      }

      switch(bRequest)
      {
        case UX_DEVICE_CLASS_VIDEO_SET_CUR:

          status = UX_SUCCESS;

          break;

        case UX_DEVICE_CLASS_VIDEO_GET_CUR:

          /* Check device speed */
          if(_ux_system_slave->ux_system_slave_speed == UX_FULL_SPEED_DEVICE)
          {
            video_Commit_Control.dwFrameInterval = (UVC_INTERVAL(UVC_CAM_FPS_FS));
            video_Commit_Control.dwMaxPayloadTransferSize = USBD_VIDEO_EPIN_FS_MPS;
          }
          else
          {
            video_Commit_Control.dwFrameInterval = (UVC_INTERVAL(UVC_CAM_FPS_HS));
            video_Commit_Control.dwMaxPayloadTransferSize = USBD_VIDEO_EPIN_HS_BINTERVAL;
          }

          /* Copy data for transfer */
          ux_utility_memory_copy(data, (VOID *)&video_Commit_Control, 26);

          /* Send transfer request */
          status = ux_device_stack_transfer_request(transfer,
                                                    UX_MIN(wLength, sizeof(USBD_VideoControlTypeDef)),
                                                    UX_MIN(wLength, sizeof(USBD_VideoControlTypeDef)));

          break;

        default:
          break;

      }

      break;

      default:
        status = UX_ERROR;
        break;
  }

  /* USER CODE END USBD_VIDEO_StreamRequest */

  return status;
}

/**
  * @brief  USBD_VIDEO_StreamGetMaxPayloadBufferSize
  *         Get video stream max payload buffer size.
  * @param  none
  * @retval max payload
  */
ULONG USBD_VIDEO_StreamGetMaxPayloadBufferSize(VOID)
{
  ULONG max_playload = 0U;

  /* USER CODE BEGIN USBD_VIDEO_StreamGetMaxPayloadBufferSize */

  max_playload = USBD_VIDEO_EPIN_HS_MPS;

  /* USER CODE END USBD_VIDEO_StreamGetMaxPayloadBufferSize */

  return max_playload;
}

/* USER CODE BEGIN 1 */

/**
  * @brief  video_write_payload
            Manage the UVC data packets.
  * @param  stream : Video class stream instance.
  * @retval none
  */
VOID video_write_payload(UX_DEVICE_CLASS_VIDEO_STREAM *stream)
{
  static ULONG length;
  ULONG timestamp = 100000;
  static uint8_t *Pcktdata = NULL;

  /* Check UVC state*/
  switch(uvc_state)
  {
    case UVC_PLAY_STATUS_STREAMING:
      /* Get the pointer to the next packet to be transmitted */
      if((new_frame_requested == 1)|| (Pcktdata == NULL))
      {
        new_frame_requested = 0;
        do{
          usb_video_data_read(&Pcktdata,&length);    /* Copy the packet in the buffer */
        }while(Pcktdata == NULL);
      }
      usb_sender_session_h264_send(stream, Pcktdata, length, timestamp, UX_TRUE);
      break;

    case UVC_PLAY_STATUS_STOP:
      break;
    default:
      return;
  }
}

/* USER CODE END 1 */

UINT usb_sender_session_h264_send(UX_DEVICE_CLASS_VIDEO_STREAM *stream, UCHAR *frame_data, ULONG frame_data_size, ULONG timestamp, UINT marker)
{


static ULONG     nal_unit_size = 0;
ULONG            nal_video_xfer;
static UCHAR     *nal_unit_start;
ULONG            usbd_video_ep_mps = stream->ux_device_class_video_stream_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
UCHAR            *buffer;
ULONG            buffer_length;

static UCHAR FID_Bit = FID_NEF_FALSE;

  if(nal_unit_start == NULL)
  {
    nal_unit_size = frame_data_size;
    nal_unit_start = frame_data;
  }
  /* Get payload buffer */
  ux_device_class_video_write_payload_get(stream, &buffer, &buffer_length);


  /*case > 1024*/
  if (nal_unit_size > (usbd_video_ep_mps - PAYLOAD_HEADER_SIZE))
  {
    nal_video_xfer = usbd_video_ep_mps - PAYLOAD_HEADER_SIZE;
    nal_unit_size -= nal_video_xfer;
    /* Set the length of the header. */
    video_frame_buffer[0] =  PAYLOAD_HEADER_SIZE;
    video_frame_buffer[1] = FID_Bit;
    *(uint32_t*)((video_frame_buffer) + 2) = timestamp;
    *(uint32_t*)((video_frame_buffer) + 6) = SOURCE_CLOCK_REF;

    /* Copy the packet in the buffer */
    ux_utility_memory_copy((video_frame_buffer + PAYLOAD_HEADER_SIZE), nal_unit_start, nal_video_xfer);
    nal_unit_start += nal_video_xfer;

    /* Copy video buffer in video frame buffer */
    /* Add the packet header */
    ux_utility_memory_copy(buffer, video_frame_buffer, nal_video_xfer + PAYLOAD_HEADER_SIZE);
    SCB_CleanDCache_by_Addr(buffer, nal_video_xfer + PAYLOAD_HEADER_SIZE);
    /* Commit payload buffer */
    ux_device_class_video_write_payload_commit(stream, nal_video_xfer + PAYLOAD_HEADER_SIZE);
  }
  else
  {
	  nal_video_xfer = nal_unit_size;
    /* Set the length of the header. */
    video_frame_buffer[0] =  PAYLOAD_HEADER_SIZE;
    video_frame_buffer[1] = FID_Bit;
    if (FID_Bit == FID_EF_FALSE)
    {
      FID_Bit = FID_EF_TRUE;
    }
    else
    {
      FID_Bit = FID_EF_FALSE;
    }

    *(uint32_t*)((video_frame_buffer) + 2) = timestamp;
    *(uint32_t*)((video_frame_buffer) + 6) = SOURCE_CLOCK_REF;

    /* Copy the packet in the buffer */
    ux_utility_memory_copy((video_frame_buffer + PAYLOAD_HEADER_SIZE), nal_unit_start, nal_video_xfer);
    /* Copy video buffer in video frame buffer */
    /* Add the packet header */
    ux_utility_memory_copy(buffer, video_frame_buffer, nal_video_xfer + PAYLOAD_HEADER_SIZE);
    SCB_CleanDCache_by_Addr(buffer, nal_video_xfer + PAYLOAD_HEADER_SIZE);
    /* Commit payload buffer */
    ux_device_class_video_write_payload_commit(stream, nal_video_xfer + PAYLOAD_HEADER_SIZE);
    new_frame_requested = 1;
    nal_unit_start = NULL;

    }
    /* Return success status. */
    return(UX_SUCCESS);
}
