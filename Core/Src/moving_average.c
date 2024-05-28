
/**
 ******************************************************************************
 *
 * Copyright (C) 2019 Binary Maker - All Rights Reserved
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution.
 * Written by Binary Maker <https://github.com/binarymaker>
 ******************************************************************************
 */

#include "moving_average.h"
#include <stdlib.h>
#include "main.h"

movingAverage_t temp_filter __attribute__((section(".bss.RAM2")));
movingAverage_t hum_filter __attribute__((section(".bss.RAM2")));
movingAverage_t bat_filter __attribute__((section(".bss.RAM2")));

int32_t temp_buf[temp_length] __attribute__((section(".bss.RAM2")));
int32_t hum_buf[hum_length] __attribute__((section(".bss.RAM2")));
int32_t bat_buf[battery_length] __attribute__((section(".bss.RAM2")));

void moving_average_init(void){
	moving_average_create(&temp_filter, temp_length, temp_buf);
	moving_average_create(&hum_filter, hum_length, hum_buf);
	moving_average_create(&bat_filter, battery_length, bat_buf);	
}

void moving_average_create(movingAverage_t *context, uint16_t filter_size, int32_t *array)
{
	context->size = filter_size;
	context->buffer = array;
	context->index = 0;
	context->sum = 0;
	context->fill = 0;
	context->filtered = 0;
}

void moving_average_filter(movingAverage_t *context, uint32_t filter_input)
{
  if (context->fill >= context->size)
  {
    context->sum -= context->buffer[context->index];
  }

  context->buffer[context->index] = filter_input;
  context->sum = context->sum + context->buffer[context->index];

  context->index++;
  if (context->index >= context->size)
  {
    context->index = 0;
  }

  if (context->fill < context->size)
  {
    context->fill++;
  }

  context->filtered = (uint32_t)(context->sum / context->fill);
}



