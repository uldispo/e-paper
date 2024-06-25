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

static void gpio_from_stop2(void);
static void gpio_before_stop2(void);

extern SPI_HandleTypeDef hspi1;
extern RTC_HandleTypeDef hrtc;
extern uint8_t ubTransmissionComplete;
extern uint8_t ubUSARTTransmissionComplete;
extern uint32_t work_time;
extern uint32_t time0;

// ##########################################################

__attribute__((noreturn)) void PWR_EnterStandbyMode(void)
{

	// uint32_t Timeout = 0; /* Variable used for Timeout management */
	HAL_PWREx_EnablePullUpPullDownConfig();

	/* ######## ENABLE WUT #################################################*/
	uint32_t tickstart;
	tickstart = HAL_GetTick();
	while ((LL_USART_IsActiveFlag_TXE(USART2) == 0) | (ubTransmissionComplete == 0))
	{											 // Wait for USART transmition to complete
		if ((HAL_GetTick() - tickstart) > 10000) // ubTransmissionComplete_timeout
		{
			timeout_reset(__func__, __LINE__);
		}
	}

	LL_RTC_DisableWriteProtection(RTC);
	/* Disable wake up timer to modify it */
	LL_RTC_WAKEUP_Disable(RTC);

	__asm volatile("nop");

	/* Wait until it is allow to modify wake up reload value */
#if (USE_TIMEOUT == 1)
	Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

	while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1)
	{
#if (USE_TIMEOUT == 1)
		if (LL_SYSTICK_IsActiveCounterFlag())
		{
			Timeout--;
		}
		if (Timeout == 0)
		{
			/* LSI activation error */
			timeout_reset(__func__, __LINE__);
		}
#endif /* USE_TIMEOUT */
	}

	LL_RTC_WAKEUP_SetAutoReload(RTC, RTC_WUT_TIME_SEC); // #define RTC_WUT_TIME_SEC ((uint32_t)60)
	LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);

	__asm volatile("nop");

	/* Enable RTC registers write protection */
	LL_RTC_EnableWriteProtection(RTC);

	/* Enable wake up counter and wake up interrupt */
	/* Note: Periodic wakeup interrupt should be enabled to exit the device
	   from low-power modes.*/
	LL_RTC_EnableIT_WUT(RTC);
	LL_RTC_WAKEUP_Enable(RTC);

	/* ######## ENTER IN STANDBY MODE ######################################*/
	/* Reset Internal Wake up flag */
	RTC_Clear_WUT();

	/* Check that PWR Internal Wake-up is enabled */
	if (LL_PWR_IsEnabledInternWU() == 0)
	{
		/* Need to enable the Internal Wake-up line */
		LL_PWR_EnableInternWU();
	}

	// LL_IWDG_ReloadCounter(IWDG);

	HAL_SuspendTick();
	// work_time = (uint32_t)((DWT->CYCCNT - time0) / 24000.0);
	//  Configure MCU low-power mode for CPU deep sleep mode
	PWR->CR1 |= LL_PWR_MODE_STANDBY; // PWR_CR1_LPMS_SHUTDOWN
	(void)PWR->CR1;					 // Ensure that the previous PWR register operations have been completed

	// Configure CPU core
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Enable CPU deep sleep mode
#ifdef NDEBUG
	DBGMCU->CR = 0; // Disable debug, trace and IWDG in low-power modes
#endif

	// Enter low-power mode
	__WFI();
	for (;;)
	{
		__DSB();
		__WFI();
	}

	//	########__  never be here  ___##########
}

// ##########################################################################
/**
 * @brief  Function to configure and enter in STOP Mode.
 * @param  None
 * @retval None
 */

void enter_stop2(uint32_t sleep_time, uint32_t wakeup_clock)
{
	//	uint32_t Timeout = 0; /* Variable used for Timeout management */

	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, sleep_time, wakeup_clock, 0) != HAL_OK)
	{
		Error_Handler();
	}

	//gpio_before_stop2();

	HAL_SuspendTick();

	/* Set Standby mode */
	// Configure MCU low-power mode for CPU deep sleep mode
	PWR->CR1 |= LL_PWR_MODE_STOP2; // PWR_CR1_LPMS_SHUTDOWN
	(void)PWR->CR1;				   // Ensure that the previous PWR register operations have been completed

	// Configure CPU core
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Enable CPU deep sleep mode
#ifdef NDEBUG
	DBGMCU->CR = 0; // Disable debug, trace and IWDG in low-power modes
#endif

	// Enter low-power mode
	__DSB();
	__WFI();

	HAL_ResumeTick();
	//gpio_from_stop2();
}

// #############################################################################

static void gpio_before_stop2(void)
{
	//  PA13	SWDIO
	//  PA14	SWCLK

	/* Set all GPIO in analog state to reduce power consumption  */

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_AHB1_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA |
							 LL_IOP_GRP1_PERIPH_GPIOB |
							 LL_IOP_GRP1_PERIPH_GPIOC |
							 LL_IOP_GRP1_PERIPH_GPIOD);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_0;

	//	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	// LL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/* Except PA13, PA14, PA10, PA12 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
						  GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_15;

	GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	LL_AHB1_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOB |
							  LL_IOP_GRP1_PERIPH_GPIOC |
							  LL_IOP_GRP1_PERIPH_GPIOD);
}

//  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void gpio_from_stop2(void)
{

	MX_GPIO_Init();
	HAL_SPI_MspInit(&hspi1);

	/**USART2 GPIO Configuration
	PA2   ------> USART2_TX
	PA3   ------> USART2_RX
	*/
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Peripheral clock enable */
	LL_AHB1_GRP1_EnableClock(LL_APB1_GRP2_PERIPH_ADC); // ???
}
