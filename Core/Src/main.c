/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "AB1805_RK.h"
#include "stm32u0xx_ll_spi.h"
#include "adc_if.h"

#include "bme280.h"
#include "bme280_utils.h"

#include "paint_sensor.h"
#include "EPD_1in54_V2.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define H_old_RAM_address 0x40
#define T_old_RAM_address 0x42
#define vbat_old_RAM_address 0x44

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

struct bme280_dev dev;
extern struct bme280_data comp_data;
int8_t reslt = BME280_OK;
uint8_t settings_sel;
uint32_t req_delay;

uint16_t H_old = 0;
uint16_t T_old = 0;
uint16_t vbat_old = 0;

extern UBYTE *BlackImage;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev);
bool read_RTCRam(uint8_t address, uint16_t *read_data, bool lock);
bool write_ToRTCRam(uint8_t address, uint16_t write_data, bool lock);
bool clearRTCRam(bool lock);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  volatile int8_t rslt;
  uint8_t dev_addr = 0;
  dev.intf_ptr = &dev_addr;
  dev.intf = BME280_SPI_INTF;
  dev.read = user_spi_read;
  dev.write = user_spi_write;
  dev.delay_us = user_delay_us;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  uint16_t h_;
  uint16_t t_;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  LL_DBGMCU_DisableDBGStopMode(); // !!!__ Disable debug in stop mode __!!!
                                  //	LL_DBGMCU_EnableDBGStopMode();
  LED1_ON();
  LL_SPI_Enable(SPI1);

  uint8_t wdalarm = read(REG_WEEKDAY_ALARM); // REG_WEEKDAY_ALARM  0x0e;
  if ((wdalarm & 0xf8) != 0xa0)              // Startup from power up.
  {
    uint32_t clk = HAL_RCC_GetSysClockFreq();
    printf("\nMAIN. First power ON.   %d\n", clk);
    resetConfig(0);
    write(REG_WEEKDAY_ALARM, 0xa0); // Magic 0xa0
    clearRTCRam(1);
    PAPER_ON_H(); // The initial values of the RAM locations are undefined.
    ESP_Init();
  }
  else
  {
    PAPER_ON_H();
    printf("\nMAIN. Startup from RTC\n");
    EPD_1IN54_V2_Reset();
    ESP_Init_standby();
  }

  printf("BME280");
  dev.settings.osr_h = BME280_OVERSAMPLING_8X;
  dev.settings.osr_p = BME280_NO_OVERSAMPLING; // UINT8_C(0x00)
  dev.settings.osr_t = BME280_OVERSAMPLING_8X;
  dev.settings.filter = BME280_FILTER_COEFF_OFF;

  rslt = bme280_init(&dev);
  if (rslt != BME280_OK) // OK = 0
  {
    printf("Failed to initialize the device (code %+d).\n", rslt);
    Error_Handler();
  }

  // settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
  rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &dev);
  /*Calculate the minimum delay (ms) required between consecutive measurement based upon the sensor enabled
   *  and the oversampling configuration. */
  req_delay = bme280_cal_meas_delay(&dev.settings);
  printf("req_delay = %d\n", req_delay);

  Activate_ADC();
  int32_t vBat = get_vbat();
  printf("vBat = %d\n", vBat);

  // =====================================================================
  rslt = stream_sensor_data_forced_mode(&dev); // working time = 0.8 sec
  if (rslt != BME280_OK)
  {
    fprintf(stderr, "Failed to stream sensor data (code %+d).", rslt);
    Error_Handler();
  }
  // h_ = comp_data.humidity / 1000.0;
  h_ = (((uint16_t)comp_data.humidity * 1049 + 500) >> 20); // fast_divide_by_1000

  // t_ = comp_data.temperature / 10.0;
  t_ = (((uint16_t)comp_data.temperature * 6554 + 2) >> 16); // fast_divide_by_10

  read_RTCRam(H_old_RAM_address, &H_old, 1);
  read_RTCRam(T_old_RAM_address, &T_old, 1);
  read_RTCRam(vbat_old_RAM_address, &vbat_old, 1);

  printf("h_ = %d   h_old = %d   t_ = %d   t_old = %d  vBat = %d, vbat_old = %d\n", h_, H_old, t_, T_old, vBat, vbat_old);
  write_ToRTCRam(H_old_RAM_address, h_, 1);
  write_ToRTCRam(T_old_RAM_address, t_, 1);
  write_ToRTCRam(vbat_old_RAM_address, vBat, 1);

  battery_out(vbat_old);
  humidity_out(h_);
  temperature_out(t_);
  EPD_1IN54_V2_DisplayPart(BlackImage);
  EPD_1IN54_V2_Sleep(); // Deep sleep mode

  hex_dump();
  HAL_Delay(1);
  PAPER_ON_L(); // e-Paper OFF
  deepPowerDown(30);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // Never be here
    LED1_ON();
    HAL_Delay(100);
    LED1_OFF();
    HAL_Delay(100);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// Function to read a uint16_t value from RTC RAM
bool read_RTCRam(uint8_t address, uint16_t *read_data, bool lock)
{
  // Create a buffer to hold the data to be read
  uint8_t data[sizeof(uint16_t)];

  // Call the driver's readRam function
  if (!readRam(address, (uint8_t *)data, sizeof(data), lock))
  {
    // If the read operation fails, return false
    return false;
  }

  // Combine the two bytes into a uint16_t value
  *read_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

  return true;
}

// Function to write a uint16_t value to RTC RAM
bool write_ToRTCRam(uint8_t address, uint16_t write_data, bool lock)
{
  // Create a buffer to hold the data to be written
  uint8_t data[sizeof(uint16_t)];

  // Split the uint16_t value into two bytes
  data[0] = (uint8_t)(write_data & 0xFF);        // Lower byte
  data[1] = (uint8_t)((write_data >> 8) & 0xFF); // Upper byte

  // Call the driver's writeRam function
  return writeRam(address, (uint8_t *)data, sizeof(data), lock);
}

void print_error(const char *func, uint32_t line)
{
  printf(" *** Error:  %s ,   %d\n", func, line);
  HAL_Delay(100);
  timeout_reset(__func__, __LINE__);
}

__attribute__((noreturn)) void timeout_reset(const char *func, uint32_t line)
{
  LL_PWR_ClearFlag_CSB(); // Clear standby flag
  printf(" *** timeout_reset:  %s    %d\n", func, line);
  HAL_Delay(100);
  // NVIC_SystemReset();
  while (1)
    ;
}

// Read BME280 data
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev)
{

  //	dev->settings.osr_h = BME280_OVERSAMPLING_8X;
  //	dev->settings.osr_p = BME280_NO_OVERSAMPLING;
  //	dev->settings.osr_t = BME280_OVERSAMPLING_8X;
  //	dev->settings.filter = BME280_FILTER_COEFF_OFF;

  //	settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
  //	reslt = bme280_set_sensor_settings(settings_sel, dev);
  /*Calculate the minimum delay (ms) required between consecutive measurement based upon the sensor enabled
   *  and the oversampling configuration. */
  //	req_delay = bme280_cal_meas_delay(&dev->settings);
  //    printf("************  req_delay = %d\n",req_delay);
  reslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
  /* Wait for the measurement to complete and print data  */

  HAL_Delay(req_delay); // 9 ms !!!

  reslt = bme280_get_sensor_data(BME280_TEMP | BME280_HUM, &comp_data, dev);

  return reslt;
}

// Function to clear the entire RTC RAM
bool clearRTCRam(bool lock)
{
  uint8_t addr = 0;
  uint8_t clearValue = 0x00;   // Value to write to clear the RAM
  uint8_t RTC_RAM_SIZE = 0x80; // Size of the RTC RAM
  uint8_t size = addr + RTC_RAM_SIZE;
  // Loop through the entire RTC RAM space and write the clear value
  // const uint8_t REG_RAM = 0x40;
  for (addr = REG_RAM; addr < size; ++addr)
  {
    if (!writeRam(addr, &clearValue, sizeof(clearValue), lock))
    {
      // If the write operation fails, return false
      return false;
    }
  }

  return true;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
