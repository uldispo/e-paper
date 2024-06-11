/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : bme280_utils.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
	*	License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BME280_UTILS_H
#define __BME280_UTILS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>


int8_t user_spi_read(uint8_t reg_addr, uint8_t *data, uint32_t len,void *intf_ptr);
int8_t user_spi_write( uint8_t reg_addr,const uint8_t *data, uint32_t len,void *intf_ptr);
void user_delay_us(uint32_t period, void *intf_ptr);



#ifdef __cplusplus
}
#endif

#endif /* __BME280_UTILS_H */

