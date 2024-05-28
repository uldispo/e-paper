/**
  ******************************************************************************
  * @file    adc_if.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    14-December-2021
  * @brief   Header for adc_if.c file
  ******************************************************************************
	*/
	
#ifndef __ADC_IF_H
#define __ADC_IF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


uint32_t battery_voltage(void);
void Activate_ADC(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_IF_H */

