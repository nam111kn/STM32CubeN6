/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tracer_emb_conf.h
  * @author  MCD Application Team
  * @brief   This file contains the Trace HW related defines.
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
/* USER CODE END Header */

#ifndef TRACER_EMB_CONF_H
#define TRACER_EMB_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_dma.h"
#include "stm32n6xx_ll_gpio.h"
#include "stm32n6xx_ll_rcc.h"
#include "stm32n6xx_ll_usart.h"
#include "stm32n6xx_hal_rcc.h"
#include "stdio.h"
/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
      Definitions for TRACE feature
-------------------------------------------------------------------------------*/

#define TRACER_EMB_BAUDRATE                          921600UL

#define TRACER_EMB_DMA_MODE                          1UL
#define TRACER_EMB_IT_MODE                           0UL

#define TRACER_EMB_BUFFER_SIZE                       1024UL

/* -----------------------------------------------------------------------------
      Definitions for TRACE Hw information
-------------------------------------------------------------------------------*/

#define TRACER_EMB_IS_INSTANCE_LPUART_TYPE           0UL /* set to 0UL if USART is used */

#define TRACER_EMB_USART_INSTANCE                    USART1

#define TRACER_EMB_TX_GPIO                           GPIOE
#define TRACER_EMB_TX_PIN                            LL_GPIO_PIN_5
#define TRACER_EMB_TX_AF                             LL_GPIO_AF_7
#define TRACER_EMB_TX_GPIO_ENABLE_CLOCK()            LL_AHB2_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE)
#define TRACER_EMB_RX_GPIO                           GPIOE
#define TRACER_EMB_RX_PIN                            LL_GPIO_PIN_6
#define TRACER_EMB_RX_AF                             LL_GPIO_AF_7
#define TRACER_EMB_RX_GPIO_ENABLE_CLOCK()            LL_AHB2_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE)

#define TRACER_EMB_ENABLE_CLK_USART()                LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define TRACER_EMB_DISABLE_CLK_USART()               LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1)
#define TRACER_EMB_SET_CLK_SOURCE_USART()            do                                                                                 \
                                                     {                                                                                  \
                                                       RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};                              \
                                                       PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;                 \
                                                       PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_IC9;              \
                                                       PeriphClkInitStruct.ICSelection[RCC_IC9].ClockSelection = RCC_ICCLKSOURCE_PLL4;  \
                                                       PeriphClkInitStruct.ICSelection[RCC_IC9].ClockDivider = 1;                       \
                                                       if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)                   \
                                                       {                                                                                \
                                                         while(1);                                                                      \
                                                       }                                                                                \
                                                     } while(0);
#define TRACER_EMB_USART_IRQ                         USART1_IRQn
#define TRACER_EMB_USART_IRQHANDLER                  USART1_IRQHandler

#define TRACER_EMB_TX_AF_FUNCTION                    LL_GPIO_SetAFPin_8_15
#define TRACER_EMB_RX_AF_FUNCTION                    LL_GPIO_SetAFPin_8_15

#define TRACER_EMB_DMA_INSTANCE                      GPDMA1
#define TRACER_EMB_ENABLE_CLK_DMA()                  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPDMA1)
#define TRACER_EMB_TX_DMA_REQUEST                    LL_GPDMA1_REQUEST_USART1_TX
#define TRACER_EMB_TX_DMA_CHANNEL                    LL_DMA_CHANNEL_0
#define TRACER_EMB_ENABLECHANNEL                     LL_DMA_ResumeChannel
#define TRACER_EMB_DISABLECHANNEL                    LL_DMA_SuspendChannel
#define TRACER_EMB_TX_DMA_IRQ                        GPDMA1_Channel0_IRQn
#define TRACER_EMB_TX_DMA_IRQHANDLER                 GPDMA1_Channel0_IRQHandler
#define TRACER_EMB_TX_DMA_ACTIVE_FLAG(_DMA_)         LL_DMA_IsActiveFlag_TC(_DMA_, TRACER_EMB_TX_DMA_CHANNEL)
#define TRACER_EMB_TX_DMA_CLEAR_FLAG(_DMA_)          LL_DMA_ClearFlag_TC(_DMA_, TRACER_EMB_TX_DMA_CHANNEL)

#ifdef __cplusplus
}
#endif

#endif /* TRACER_EMB_CONF_H */
