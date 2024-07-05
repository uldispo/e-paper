/**
 ******************************************************************************
 * @file    lpm.h
 * @author  upo
 * @version V1.0.0
 * @date    16/11/2021
 * @brief   This file contains the headers of low power management file
 ******************************************************************************
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPM_H
#define __LPM_H

#include "main.h"

void enter_stop2(uint32_t sleep_time, uint32_t wakeup_clock);

#endif /* __LPM_H */
