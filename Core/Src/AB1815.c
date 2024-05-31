/**
 *     An Abracon AB18X5 Real-Time Clock library for Arduino
 *     Copyright (C) 2015 NigelB
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with this program; if not, write to the Free Software Foundation, Inc.,
 *     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **/

// #include "AB1815_Arduino.h"
#include "SPI.h"
#include "stdarg.h"
#include "AB1815_A.h"
// #include "AB1815.h"
#include "TimeLib.h"
#include "printf.h"

bool ab1815_status_e_OK = 1;
bool ab1815_status_e_ERROR = 0;

void spi_select_slave(bool select) // 1 = high, 0 = low
{
	if (select)
	{
		RTC_H();
	}
	else
	{
		RTC_L();
	}
}

bool read(uint8_t offset, uint8_t *buf, uint8_t length)
{
	uint8_t address = AB1815_SPI_READ(offset);

	spi_select_slave(0);
	// SPI.transfer(address);
	// HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
	if (HAL_SPI_Transmit(&hspi1, &address, 1, 3000) != HAL_OK)
	{
		print_error(__func__, __LINE__);
		return ab1815_status_e_ERROR;
	}

	if (HAL_SPI_Receive(&hspi1, buf, length, 3000) != HAL_OK)
	{
		print_error(__func__, __LINE__);
		return ab1815_status_e_ERROR;
	}

	spi_select_slave(1);
	return ab1815_status_e_OK;
};

bool write(uint8_t offset, uint8_t *buf, uint8_t length)
{
	uint8_t address = AB1815_SPI_WRITE(offset);
	spi_select_slave(0);

	if (HAL_SPI_Transmit(&hspi1, &address, 1, 3000) != HAL_OK)
	{
		print_error(__func__, __LINE__);
	}

	if (HAL_SPI_Transmit(&hspi1, buf, length, 3000) != HAL_OK)
	{
		print_error(__func__, __LINE__);
	}

	spi_select_slave(1); // set 1
	return ab1815_status_e_OK;
};

// // 0x00
// time_t get()
// {
// 	tmElements_t tm;
// 	get_time(&tm);
// 	return makeTime(tm); // ???
// }

// // 0x00
// void set(time_t time)
// {
// 	ab1815_tmElements_t tm;
// 	breakTime(time, tm); // void breakTime(time_t timeInput, ab1815_tmElements_t tm)
// 	set_time(&tm);		 // bool set_time(ab1815_tmElements_t *time)
// }

// // 0x00
// bool get_time(ab1815_tmElements_t *time)
// {
// 	bool to_ret = ab1815_status_e_ERROR;
// 	size_t length = (AB1815_REG_ALARM_HUNDREDTHS - AB1815_REG_TIME_HUNDREDTHS);
// 	uint8_t buffer[length];
// 	memset(buffer, 0, length);
// 	if (read(AB1815_REG_TIME_HUNDREDTHS, buffer, length) == ab1815_status_e_OK)
// 	{
// 		to_ret = ab1815_status_e_OK;
// 		time->Hundredth = bcd2bin(buffer[0]);
// 		time->Second = bcd2bin(0x7F & buffer[1]);
// 		time->Minute = bcd2bin(0x7F & buffer[2]);
// 		time->Hour = bcd2bin(0x3F & buffer[3]);
// 		time->Day = bcd2bin(0x3F & buffer[4]);
// 		time->Month = bcd2bin(0x1F & buffer[5]);
// 		time->Year = y2kYearToTm(bcd2bin(buffer[6]));
// 		time->Wday = bcd2bin(0x07 & buffer[7]);
// 	}
// 	return to_ret;
// }

// // 0x00
// bool set_time(ab1815_tmElements_t *time)
// {
// 	size_t length = (AB1815_REG_ALARM_HUNDREDTHS - AB1815_REG_TIME_HUNDREDTHS);
// 	uint8_t buffer[length];
// 	bool result = ab1815_status_e_ERROR;

// 	memset(buffer, 0, length);
// 	buffer[0] = bin2bcd(time->Hundredth);
// 	buffer[1] = bin2bcd(0x7F & time->Second);
// 	buffer[2] = bin2bcd(0x7F & time->Minute);
// 	buffer[3] = bin2bcd(0x3F & time->Hour);
// 	buffer[4] = bin2bcd(0x3F & time->Day);
// 	buffer[5] = bin2bcd(0x1F & time->Month);
// 	buffer[6] = bin2bcd(tmYearToY2k(time->Year));
// 	buffer[7] = bin2bcd(0x07 & time->Wday);

// 	if (write(AB1815_REG_TIME_HUNDREDTHS, buffer, length) == ab1815_status_e_OK)
// 	{
// 		result = ab1815_status_e_OK;
// 	}

// 	return result;
// };

bool clear_hundrdeds()
{
	uint8_t buf[1];
	buf[0] = 0;
	return write(AB1815_REG_TIME_HUNDREDTHS, buf, 1);
};

// 0x08
// bool get_alarm(ab1815_tmElements_t *time, ab1815_alarm_repeat_mode *alarm_mode)
// {
// 	bool to_ret = ab1815_status_e_ERROR;
// 	size_t length = AB1815_REG_STATUS - AB1815_REG_ALARM_HUNDREDTHS;
// 	uint8_t buffer[length];
// 	memset(buffer, 0, length);
// 	countdown_control_t cd_reg;
// 	uint32_t *val = (uint32_t *)alarm_mode;

// 	if (get_countdown_control(&cd_reg) == ab1815_status_e_OK)
// 	{
// 		if (read(AB1815_REG_ALARM_HUNDREDTHS, buffer, length) == ab1815_status_e_OK)
// 		{
// 			to_ret = ab1815_status_e_OK;
// 			time->Hundredth = bcd2bin(buffer[0]);
// 			time->Second = bcd2bin(0x7F & buffer[1]);
// 			time->Minute = bcd2bin(0x7F & buffer[2]);
// 			time->Hour = bcd2bin(0x3F & buffer[3]);
// 			time->Day = bcd2bin(0x3F & buffer[4]);
// 			time->Month = bcd2bin(0x1F & buffer[5]);
// 			time->Wday = bcd2bin(0x07 & buffer[6]);
// 		}
// 		*alarm_mode = (ab1815_alarm_repeat_mode)cd_reg.fields.RPT;
// 		if (cd_reg.fields.RPT == 7)
// 		{
// 			if ((time->Hundredth & 0xF0) == 0xF0)
// 			{

// 				(*val)++;
// 				if ((time->Hundredth & 0xFF) == 0xFF)
// 				{
// 					(*val)++;
// 				}
// 			}
// 		}
// 	}
// 	return to_ret;
// };

// // 0x08
// bool set_alarm(ab1815_tmElements_t *time, ab1815_alarm_repeat_mode alarm_mode)
// {
// 	size_t length = AB1815_REG_STATUS - AB1815_REG_ALARM_HUNDREDTHS;
// 	uint8_t buffer[length];
// 	bool result = ab1815_status_e_ERROR;
// 	uint8_t repeat = alarm_mode;

// 	memset(buffer, 0, length);
// 	buffer[0] = bin2bcd(time->Hundredth);
// 	buffer[1] = bin2bcd(0x7F & time->Second);
// 	buffer[2] = bin2bcd(0x7F & time->Minute);
// 	buffer[3] = bin2bcd(0x3F & time->Hour);
// 	buffer[4] = bin2bcd(0x3F & time->Day);
// 	buffer[5] = bin2bcd(0x1F & time->Month);
// 	buffer[6] = bin2bcd(0x07 & time->Wday);

// 	switch (alarm_mode)
// 	{
// 	case ab1815_alarm_repeat_once_per_tenth:
// 		repeat = 7;
// 		buffer[0] |= 0xF0;
// 		break;
// 	case ab1815_alarm_repeat_once_per_hundredth:
// 		repeat = 7;
// 		buffer[0] = 0xFF;
// 		break;
// 	default:
// 		repeat = alarm_mode;
// 	}

// 	if (write(AB1815_REG_ALARM_HUNDREDTHS, buffer, length) == ab1815_status_e_OK)
// 	{
// 		countdown_control_t cd_reg;

// 		if (get_countdown_control(&cd_reg) == ab1815_status_e_OK)
// 		{
// 			cd_reg.fields.RPT = repeat;
// 			return set_countdown_control(&cd_reg);
// 		}
// 	}
// 	return result;
// };

// 0x0F - See also: ARST in Control1.
//	If ARST is a 1, a read of the Status register will produce the current state of all
//	the interrupt flags and then clear them
bool set_status(status_t *status)
{
	return write(AB1815_REG_STATUS, &status->value, 1);
};

bool get_status(status_t *status)
{
	return read(AB1815_REG_STATUS, &status->value, 1);
};

// 0x10
bool set_control1(control1_t *control1)
{
	return write(AB1815_REG_CONTROL1, &control1->value, 1);
};

bool get_control1(control1_t *control1)
{
	return read(AB1815_REG_CONTROL1, &control1->value, 1);
};

// 0x11
bool set_control2(control2_t *control2)
{
	return write(AB1815_REG_CONTROL2, &control2->value, 1);
};

bool get_control2(control2_t *control2)
{
	return read(AB1815_REG_CONTROL2, &control2->value, 1);
};

// 0x12
bool set_interrupt_mask(inturrupt_mask_t *inturrupt_mask)
{
	return write(AB1815_REG_INTERRUPT_MASK, &inturrupt_mask->value, 1);
};

bool get_interrupt_mask(inturrupt_mask_t *inturrupt_mask)
{
	return read(AB1815_REG_INTERRUPT_MASK, &inturrupt_mask->value, 1);
};

// 0x13
bool set_square_wave(square_wave_t *square_wave)
{
	return write(AB1815_REG_SQW, &square_wave->value, 1);
}

bool get_square_wave(square_wave_t *square_wave)
{
	return read(AB1815_REG_SQW, &square_wave->value, 1);
}

// 0x14
bool set_cal_xt(cal_xt_t *cal_xt)
{
	return write(AB1815_REG_CAL_XT, &cal_xt->value, 1);
}

bool get_cal_xt(cal_xt_t *cal_xt)
{
	return read(AB1815_REG_CAL_XT, &cal_xt->value, 1);
}

// 0x15
bool set_cal_rc_hi(cal_rc_hi_t *cal_rc_hi)
{
	return write(AB1815_REG_CAL_RC_HI, &cal_rc_hi->value, 1);
}

bool get_cal_rc_hi(cal_rc_hi_t *cal_rc_hi)
{
	return read(AB1815_REG_CAL_RC_HI, &cal_rc_hi->value, 1);
}

// 0x16
bool set_cal_rc_low(cal_rc_low_t *cal_rc_low)
{
	return write(AB1815_REG_CAL_RC_LOW, &cal_rc_low->OFFSETR, 1);
}

bool get_cal_rc_low(cal_rc_low_t *cal_rc_low)
{
	return read(AB1815_REG_CAL_RC_LOW, &cal_rc_low->OFFSETR, 1);
}

// 0x17 sleep_control_t
bool set_sleep_control(sleep_control_t *sleep_control)
{
	return write(AB1815_REG_SLEEP_CONTROL, &sleep_control->value, 1);
};

bool get_sleep_control(sleep_control_t *sleep_control)
{
	return read(AB1815_REG_SLEEP_CONTROL, &sleep_control->value, 1);
};

// 0x18
bool set_countdown_control(countdown_control_t *countdown_control)
{
	return write(AB1815_REG_COUNTDOWN_TIMER_CONTROL, &countdown_control->value, 1);
};

bool get_countdown_control(countdown_control_t *countdown_control)
{
	return read(AB1815_REG_COUNTDOWN_TIMER_CONTROL, &countdown_control->value, 1);
};

// 0x19
bool set_countdown_timer(uint8_t timer_value)
{
	return write(AB1815_REG_COUNTDOWN_TIMER, &timer_value, 1);
}

bool get_countdown_timer(uint8_t *timer_value)
{
	return read(AB1815_REG_COUNTDOWN_TIMER, timer_value, 1);
}

// 0x1A
bool set_countdown_timer_initial_value(uint8_t timer_value)
{
	return write(AB1815_REG_COUNTDOWN_TIMER_INITIAL, &timer_value, 1);
}

bool get_countdown_timer_initial_value(uint8_t *timer_value)
{
	return read(AB1815_REG_COUNTDOWN_TIMER_INITIAL, timer_value, 1);
}

// 0x1B
bool set_watchdog_timer(watchdog_timer_t *watchdog_timer)
{
	return write(AB1815_REG_WATCHDOG_TIMER, &watchdog_timer->value, 1);
}

bool get_watchdog_timer(watchdog_timer_t *watchdog_timer)
{
	return read(AB1815_REG_WATCHDOG_TIMER, &watchdog_timer->value, 1);
}

// 0x1C Get the oscillator control register
bool get_oscillator_control(oscillator_control_t *oscillator_control)
{
	return read(AB1815_REG_OSCILLATOR_CONTROL, &oscillator_control->value, 1);
};

bool set_oscillator_control(oscillator_control_t *oscillator_control)
{
	if (set_configuration_key(ab1815_oscillator_control) != ab1815_status_e_OK)
	{
		return ab1815_status_e_ERROR;
	}
	return write(AB1815_REG_OSCILLATOR_CONTROL, &oscillator_control->value, 1);
};

// 0x1D
bool set_oscillator_status(oscillator_status_t *oscillator_status)
{
	return write(AB1815_REG_OSCILLATOR_STATUS, &oscillator_status->value, 1);
}

bool get_oscillator_status(oscillator_status_t *oscillator_status)
{
	return read(AB1815_REG_OSCILLATOR_STATUS, &oscillator_status->value, 1);
}

// 0x1E - Nothing on the AB1815
// 0x1F
bool set_configuration_key(configuration_key_e configuration_key)
{
	return write(AB1815_REG_CONFIGURATION_KEY, (uint8_t *)&configuration_key, 1);
};

// 0x20
bool set_trickle(trickle_t *trickle)
{
	return write(AB1815_REG_TRICKLE_CONTROL, &trickle->value, 1);
}

bool get_trickle(trickle_t *trickle)
{
	return read(AB1815_REG_TRICKLE_CONTROL, &trickle->value, 1);
}

// 0x21
bool set_bref_control(bref_control_t *bref_control)
{
	return write(AB1815_REG_BREF_CONTROL, &bref_control->value, 1);
}

bool get_bref_control(bref_control_t *bref_control)
{
	return read(AB1815_REG_BREF_CONTROL, &bref_control->value, 1);
}

// 0x26
bool set_afctrl(afctrl_e afctrl)
{
	return write(AB1815_REG_AFCTRL, (uint8_t *)&afctrl, 1);
}

bool get_(afctrl_e *afctrl)
{
	return read(AB1815_REG_AFCTRL, (uint8_t *)afctrl, 1);
}

// 0x27
bool set_batmodeio(ab1815_batmodeio_e mode)
{
	if (set_configuration_key(ab1815_reg_control) != ab1815_status_e_OK)
	{
		return ab1815_status_e_ERROR;
	}
	uint8_t buf[1];
	buf[0] = mode;
	return write(AB1815_REG_BATMODE_IO, buf, 1);
};

bool get_batmodeio(ab1815_batmodeio_e *mode)
{
	return read(AB1815_REG_BATMODE_IO, (uint8_t *)mode, 1);
}

// 0x28
bool get_id(ab1815_id_t *id)
{
	size_t length = AB1815_REG_ID6 - AB1815_REG_ID0;
	uint8_t buffer[length];
	memset(buffer, 0, length);
	bool result = ab1815_status_e_ERROR;

	result = read(AB1815_REG_ID0, buffer, length);
	if (result == ab1815_status_e_OK)
	{
		id->ID0 = bcd2bin(buffer[0]);
		id->ID1 = bcd2bin(buffer[1]);
		id->ID2.value = buffer[2];
		id->ID3 = bcd2bin(buffer[3]);
		id->ID4 = bcd2bin(buffer[4]);
		id->ID5 = bcd2bin(buffer[5]);
		id->ID6 = bcd2bin(buffer[6]);
	}

	return result;
};

// 0x2F
bool set_analog_status_register(ab1815_analog_status_t *analog_status)
{
	return write(AB1815_REG_ANALOG_STATUS, &analog_status->value, 1);
}

bool get_analog_status_register(ab1815_analog_status_t *analog_status)
{
	return read(AB1815_REG_ANALOG_STATUS, &analog_status->value, 1);
}

// 0x30
bool set_output_control(ab1815_output_control_t *output_control)
{
	return write(AB1815_REG_OUTPUT_CONTROL, &output_control->value, 1);
}

bool get_output_control(ab1815_output_control_t *output_control)
{
	return read(AB1815_REG_OUTPUT_CONTROL, &output_control->value, 1);
}

// 0x3F
bool set_extension_ram(extension_ram_t *extension_ram)
{
	return write(AB1815_EXTENTION_RAM, &extension_ram->value, 1);
}

bool get_extension_ram(extension_ram_t *extension_ram)
{
	return read(AB1815_EXTENTION_RAM, &extension_ram->value, 1);
}

void hex_dump(void)
{
	uint8_t buffer[8];
	for (uint8_t pos = 0; pos < 0x7F; pos += 8)
	{
		read(pos, buffer, 8);
		printf("# 0x%02x: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\r\n", pos, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
	}
}

bool detectChip()
{
	bool bResult, finalResult = false;
	uint8_t value = 0;

	// FOUT/nIRQ  will go HIGH when the chip is ready to respond

	uint32_t start = HAL_GetTick();

		while (HAL_GPIO_ReadPin(NIRQ_GPIO_Port, NIRQ_Pin) != GPIO_PIN_SET) // B12
		{
			if((HAL_GetTick() - start) > 5000)
			{
				printf("FOUT did not go HIGH\n");
				break;
			}
		}


	bResult = read(AB1815_REG_ID0, &value, 1); // REG_ID0 = 0x28, the upper RW bit indicating read (if 0) or write (if 1).
	if (bResult && value == REG_ID0_AB18XX)
	{
		printf("REG_ID0 = %X", value);
		bResult = read(AB1815_REG_ID1, &value, 1);
		if (bResult && value == REG_ID1_ABXX15)
		{
			printf("%X\n", value);
			finalResult = true;
		}
	}
	if (!finalResult)
	{
		printf("not detected\n");
	}

	return finalResult;
}

void initialize_clock(void)
{
	oscillator_control_t oscillator_control;
	get_oscillator_control(&oscillator_control);
	printf("# retrieved oscillator_control: %X\n", oscillator_control.value);

	//  OSEL = 1 when using RC oscillator instead of XTAL
	oscillator_control.fields.OSEL = 1;

	//  Disable I/O Interface during sleep to ensure the clock it not corrupted
	//  by floating pins and what not.
	oscillator_control.fields.PWGT = 1;
	printf("# set oscillator_control: %X\n", oscillator_control.value);

	set_oscillator_control(&oscillator_control);

	//  Hundredths don't seem to tick over when using the RC clock source
	//  So I clear them
	clear_hundrdeds();

	control1_t control1;
	get_control1(&control1);

	//	0 is 24 Hour Mode
	control1.fields._12_24 = 0;

	//  1 is Power Switch Mode
	control1.fields.PWR2 = 1;
	set_control1(&control1);

	inturrupt_mask_t int_mask;
	get_interrupt_mask(&int_mask);

	//  Alarm Interrupt Enable = true
	int_mask.fields.AIE = 1;

	//  Set Interrupt Mode to be a Logic Level (opposed to a pulse)
	int_mask.fields.IM = ab1815_interrupt_im_level;
	set_interrupt_mask(&int_mask);

	control2_t control2;

	//  Set NIRQ Pin to output NIRQ (since AIE is enabled)
	//    control2.fields.OUT1S = ab1815_fout_nIRQ_or_OUT;

	//  Set NIRQ2 pin to be power switched sleep
	control2.fields.OUT2S = ab1815_psw_SLEEP;
	set_control2(&control2);
}

void set_timer(void)
{
	countdown_control_t control;
	inturrupt_mask_t int_mask;
	int_mask.fields.TIE = 1;
	set_interrupt_mask(&int_mask);
	// Clear any pending interrupts
	write(AB1815_REG_STATUS, 0, 1); // REG_STATUS_DEFAULT= 0x00; //!< Status register, default
	// Stop countdown timer if already running since it can't be set while running
	// REG_TIMER_CTRL_DEFAULT   = 0x23; //!< Countdown timer control, 0b00100011 (TFPT + TFS = 1/60 Hz0)
	write(AB1815_REG_COUNTDOWN_TIMER_CONTROL, (uint8_t *)0x23, 1);

	control.fields.TM = 1;
	control.fields.TRPT = 1;
	control.fields.TFS = 0b11;
	set_countdown_control(&control);
	set_countdown_timer(60);
}

void enable_countdown(void)
{
	countdown_control_t control;
	control.fields.TE = 1;
	set_countdown_control(&control);
}
