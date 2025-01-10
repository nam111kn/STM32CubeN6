/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32n6xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief SD MSP Initialization
* This function configures the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
  GPIO_InitTypeDef GPIO_Init_Structure = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hsd->Instance==SDMMC2)
  {
   /* USER CODE BEGIN SDMMC2_MspInit 0 */

  /* USER CODE END SDMMC2_MspInit 0 */


 /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC2;
    PeriphClkInitStruct.Sdmmc2ClockSelection = RCC_SDMMC2CLKSOURCE_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    HAL_PWREx_EnableVddIO5();
    /* Enable SDMMC clock */
    __HAL_RCC_SDMMC2_CLK_ENABLE();

    /* SDMMC2 access */
    __HAL_RCC_RIFSC_CLK_ENABLE();
    RIMC_MasterConfig_t RIMC_master = {0};
    RIMC_master.MasterCID = RIF_CID_1;

    RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_SDMMC2, &RIMC_master);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_SDMMC2, RIF_ATTRIBUTE_SEC|RIF_ATTRIBUTE_PRIV);

  /* USER CODE END DMA2D_MspInit 1 */
  /* USER CODE BEGIN SDMMC2_MspInit 0 */

  /* USER CODE END SDMMC2_MspInit 0 */

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

     /**SDMMC2 GPIO Configuration
    PC0    ------> SDMMC2_CMD
    PC2     ------> SDMMC2_D2
    PC3     ------> SDMMC2_D3
    PC4     ------> SDMMC2_CK
    PC5     ------> SDMMC2_D0
    PE4     ------> SDMMC2_D1

    */

    /* Common GPIO configuration */
    GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull      = GPIO_PULLUP;
    GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init_Structure.Alternate = GPIO_AF11_SDMMC2;

    /* D2-CLK-CMD-D0-D1*/
    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4  | GPIO_PIN_5 ;
    HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);

    /* D3*/
    GPIO_Init_Structure.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);



    /* NVIC configuration for SDMMC interrupts */
    HAL_NVIC_SetPriority(SDMMC2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDMMC2_IRQn);
  /* USER CODE BEGIN SDMMC2_MspInit 1 */

  /* USER CODE END SDMMC2_MspInit 1 */
  }

}

/**
* @brief SD MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
  if(hsd->Instance==SDMMC2)
  {
  /* USER CODE BEGIN SDMMC2_MspDeInit 0 */

  /* USER CODE END SDMMC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC2_CLK_DISABLE();

    /**SDMMC2 GPIO Configuration
    PD2     ------> SDMMC2_CK
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDMMC2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(SDMMC2_IRQn);
  /* USER CODE BEGIN SDMMC2_MspDeInit 1 */

  /* USER CODE END SDMMC2_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
