/**
 ******************************************************************************
 * @file    lpm.c
 * @brief   This file provides code for the configuration
 *          of low power modes.
 ******************************************************************************
 *
 *  In the Stop 2 mode, all I/O pins keep the same state as in the Run mode.
 *
 ******************************************************************************
 **/

#include "lpm.h"
#include "main.h"
#include "gpio.h"
#include <stdio.h>
#include "usart.h"
#include "stm32u0xx_ll_usart.h"
#include "stm32u0xx_hal_rtc.h"

extern RTC_HandleTypeDef hrtc;

/**
 * @brief  Function to configure and enter in STOP Mode.
 * @param  None
 * @retval None
 */

void enter_stop2(uint32_t sleep_time, uint32_t wakeup_clock)
{
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, sleep_time, wakeup_clock, 0) != HAL_OK)
	{
		Error_Handler();
	}

	// LED1_OFF();
	HAL_SuspendTick();

	// Configure MCU low-power mode for CPU deep sleep mode
	PWR->CR1 |= LL_PWR_MODE_STOP2; // PWR_CR1_LPMS_SHUTDOWN
	(void)PWR->CR1;				   // Ensure that the previous PWR register operations have been completed

	// Configure CPU core
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Enable CPU deep sleep mode

	// Enter STOP2 mode
	__DSB();
	__WFI();

	HAL_ResumeTick();
	// LED1_ON();
	// printf("Exit from STOP2\n");
}
