
/**
  ******************************************************************************                              |___/                                  
  *                                                                       
  * Copyright (C) 2019 Binary Maker - All Rights Reserved
  *
  * This program and the accompanying materials are made available
  * under the terms described in the LICENSE file which accompanies
  * this distribution.
  * Written by Binary Maker <https://github.com/binarymaker>
  ******************************************************************************
  */


/**
 * @file moving-average.h
 * @author binary maker <https://github.com/binarymaker>
 * @brief Average filter based on FIFO buffer
 * @date 2019-05-25
 *
 */

#include "stdint.h"

#define temp_length 16
#define battery_length 8
#define hum_length 8

typedef struct movingAverage_s
{
  int32_t *buffer;      /**< Data buffer pointer */
  uint16_t size;        /**< Size of filter buffer */
  uint16_t index;       /**< Current location in buffer */
  uint16_t fill;        /**< Buffer filled level */
  int32_t sum;          /**< Cumulative value of buffer */
  int32_t filtered;     /**< Filtered output */
} movingAverage_t;

/**
 * @brief filter object initialization
 * 
 * @param context [in] instance of filter object
 * @param filter_size [in] size of filter buffer
 * @param sample_time [in] filter sampling time in ms 
 */
void moving_average_create(movingAverage_t *context, uint16_t filter_size, int32_t *array);

/**
 * @brief filter process function
 * 
 * @param context [in] instance of filter object
 * @param input [in] data sample to filter
 */
void moving_average_filter(movingAverage_t *context, uint32_t filter_input);

void moving_average_init(void);



