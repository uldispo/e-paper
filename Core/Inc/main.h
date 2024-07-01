/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stm32u073xx.h>
#include "stm32u0xx_ll_pwr.h"
#include "stm32u0xx_hal_adc.h"
#include "stm32u0xx_ll_spi.h"
#include "stm32u0xx_ll_rtc.h"
#include "stm32u0xx.h"
#include "stm32u0xx_ll_system.h"
#include "stm32u0xx_ll_pwr.h"
#include "stm32u0xx_ll_bus.h"
#include "stm32u0xx_ll_cortex.h"
#include "stm32u0xx_ll_utils.h"
#include "stm32u0xx_ll_dma.h"
#include "stm32u0xx_ll_exti.h"
#include "stm32u0xx_ll_gpio.h"
#include "stm32u0xx_ll_rcc.h"
#include "stm32u0xx_ll_usart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "printf.h"

    /* USER CODE END Includes */

    /* Exported types ------------------------------------------------------------*/
    /* USER CODE BEGIN ET */

    /* USER CODE END ET */

    /* Exported constants --------------------------------------------------------*/
    /* USER CODE BEGIN EC */

    /* USER CODE END EC */

    /* Exported macro ------------------------------------------------------------*/
    /* USER CODE BEGIN EM */

    /* USER CODE END EM */

    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOB
#define NIRQ_Pin GPIO_PIN_12
#define NIRQ_GPIO_Port GPIOB
#define WDI_Pin GPIO_PIN_13
#define WDI_GPIO_Port GPIOB
#define CE_RTC_Pin GPIO_PIN_14
#define CE_RTC_GPIO_Port GPIOB
#define PAPER_ON_Pin GPIO_PIN_15
#define PAPER_ON_GPIO_Port GPIOB
#define D_C_Pin GPIO_PIN_8
#define D_C_GPIO_Port GPIOA
#define BUSY_Pin GPIO_PIN_9
#define BUSY_GPIO_Port GPIOA
#define CSB_Pin GPIO_PIN_10
#define CSB_GPIO_Port GPIOA
#define RST_Pin GPIO_PIN_11
#define RST_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_12
#define CS_GPIO_Port GPIOA

    /* USER CODE BEGIN Private defines */

#define CS_H() GPIOA->BSRR = GPIO_BSRR_BS12 // CS display
#define CS_L() GPIOA->BSRR = GPIO_BSRR_BR12

#define CSB_H() GPIOA->BSRR = GPIO_BSRR_BS10 // CS BME280
#define CSB_L() GPIOA->BSRR = GPIO_BSRR_BR10

#define RST_H() GPIOA->BSRR = GPIO_BSRR_BS11 // Reset Display
#define RST_L() GPIOA->BSRR = GPIO_BSRR_BR11

#define LED1_ON() GPIOB->BSRR = GPIO_BSRR_BS1
#define LED1_OFF() GPIOB->BSRR = GPIO_BSRR_BR1

#define DC_H() GPIOA->BSRR = GPIO_BSRR_BS8 // Data/command display
#define DC_L() GPIOA->BSRR = GPIO_BSRR_BR8

#define RTC_H() GPIOB->BSRR = GPIO_BSRR_BS14 // CS RTC
#define RTC_L() GPIOB->BSRR = GPIO_BSRR_BR14

#define PAPER_ON_H() GPIOB->BSRR = GPIO_BSRR_BS15 // e-Paper power on/pff
#define PAPER_ON_L() GPIOB->BSRR = GPIO_BSRR_BR15

#define PB6_H() GPIOB->BSRR = GPIO_BSRR_BS6 // e-Paper power on/pff
#define PB6_L() GPIOB->BSRR = GPIO_BSRR_BR6

#define PB7_H() GPIOB->BSRR = GPIO_BSRR_BS7 // e-Paper power on/pff
#define PB7_L() GPIOB->BSRR = GPIO_BSRR_BR7

#define timeout_value 3000
#define RTC_WUT_TIME_SEC ((uint32_t)60) /* 60 s */

#define RTC_Clear_WUT()            \
    do                             \
    {                              \
        LL_RTC_ClearFlag_WUT(RTC); \
        __ISB();                   \
    } while (LL_RTC_IsActiveFlag_WUT(RTC));

    void print_error(const char *func, uint32_t line);
    void timeout_reset(const char *func, uint32_t line);

    /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
