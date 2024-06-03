
/* AB1805: a library to provide high level APIs to interface with the
* Abracon Corporation Real-time Clock. It is possible to use this library
* in Energia (the Arduino port for MSP microcontrollers) or in other
* toolchains.

* This file is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License
* version 3, both as published by the Free Software Foundation.
*/

#include "main.h"
#include "AB1805.h"
#include "printf.h"
#include "stm32u0xx_ll_spi.h"
extern SPI_HandleTypeDef hspi1;

inline static uint8_t read_rtc_register(uint8_t reg_addr);
inline static uint8_t write_rtc_register(uint8_t rtc_register, uint8_t data);
inline static uint32_t utils_enter_critical_section(void);
inline static void utils_exit_critical_section(uint32_t primask_bit);

#define AB1815_SPI_READ(offset) (127 & offset)
#define AB1815_SPI_WRITE(offset) (128 | offset)
#define DEBUG_SIO
// data
volatile uint8_t _year;
volatile uint8_t _month;
volatile uint8_t _date;
volatile uint8_t _weekday;
volatile uint8_t _hour;
volatile uint8_t _minute;
volatile uint8_t _second;
volatile uint8_t _hundredth;

volatile uint8_t _alarm_year;
volatile uint8_t _alarm_month;
volatile uint8_t _alarm_date;
volatile uint8_t _alarm_weekday;
volatile uint8_t _alarm_hour;
volatile uint8_t _alarm_minute;
volatile uint8_t _alarm_second;

char data_time_string[CONST_DATE_TIME_STRING_LEN]; // CONST_DATE_TIME_STRING_LEN=30

uint8_t _status;
uint8_t _control1;
uint8_t _control2;
uint8_t _interrupt;
uint8_t _sleep_mode;
uint8_t _timer_control_mode;
uint8_t _outcontrol;
uint8_t _osc_control;
uint8_t _wdt_register;

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

uint8_t get_rtc_data(const uint8_t rtc_register, const uint8_t register_mask)
{
  return (read_rtc_register(rtc_register) & register_mask);
}

uint8_t get_hundredths(void)
{
  return get_rtc_data(HUNDRETH_REGISTER, 0xFF);
}

uint8_t get_second(void)
{
  return get_rtc_data(SECOND_REGISTER, SECOND_MASK);
}

uint8_t get_minute(void)
{
  return get_rtc_data(MINUTE_REGISTER, MINUTE_MASK);
}

uint8_t get_hour(void)
{
  return get_rtc_data(HOUR_REGISTER, HOUR_24_MASK);
}

uint8_t get_weekday(void)
{
  return get_rtc_data(WEEKDAY_REGISTER, DAY_MASK);
}

uint8_t get_date(void)
{
  return get_rtc_data(DATE_REGISTER, DATE_MASK);
}

uint8_t get_month(void)
{
  return get_rtc_data(MONTH_REGISTER, MONTH_MASK);
}

uint8_t get_year(void)
{
  return get_rtc_data(YEAR_REGISTER, YEAR_MASK);
}

void get_datetime(void)
{
  // rtc_check_valid();
  //_hundredths = get_hundredths();
  _second = get_second();
  _minute = get_minute();
  _hour = get_hour();
  _weekday = get_weekday();
  _date = get_date();
  _month = get_month();
  _year = get_year();
}

uint8_t get_second_alarm(void)
{
  _alarm_second = get_rtc_data(SECOND_ALARM_REGISTER, SECOND_ALARM_MASK);
  return _alarm_second;
}

uint8_t get_minute_alarm(void)
{
  _alarm_minute = get_rtc_data(MINUTE_ALARM_REGISTER, MINUTE_ALARM_MASK);
  return _alarm_minute;
}

uint8_t get_hour_alarm(void)
{
  _alarm_hour = get_rtc_data(HOUR_ALARM_REGISTER, HOUR_24_ALARM_MASK);
  return _alarm_hour;
}

uint8_t get_weekday_alarm(void)
{
  _alarm_weekday = get_rtc_data(WEEKDAY_ALARM_REGISTER, WEEKDAY_ALARM_MASK);
  return _alarm_weekday;
}

uint8_t get_date_alarm(void)
{
  _alarm_date = get_rtc_data(DATE_ALARM_REGISTER, DATE_ALARM_MASK);
  return _alarm_date;
}

uint8_t get_month_alarm(void)
{
  _alarm_month = get_rtc_data(MONTH_ALARM_REGISTER, MONTH_ALARM_MASK);
  return _alarm_month;
}

void set_second(const uint8_t value)
{
  _second = value % MAX_SECOND;
  write_rtc_register(SECOND_REGISTER, _second);
}

void set_minute(const uint8_t value)
{
  _minute = value % MAX_MINUTE;
  write_rtc_register(MINUTE_REGISTER, _minute);
}

void set_hour(const uint8_t value)
{
  _hour = value % MAX_HOURS;
  write_rtc_register(HOUR_REGISTER, _hour);
}

void set_weekday(const uint8_t value)
{
  _weekday = value % MAX_DAY;
  write_rtc_register(WEEKDAY_REGISTER, _weekday);
}

void set_date(const uint8_t value)
{
  _date = value % MAX_DATE;
  write_rtc_register(DATE_REGISTER, _date);
}

void set_month(const uint8_t value)
{
  _month = value % MAX_MONTH;
  write_rtc_register(MONTH_REGISTER, _month);
}

void set_year(const uint8_t value)
{
  _year = value % MAX_YEAR;
  write_rtc_register(YEAR_REGISTER, value);
}

void set_datetime(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  set_year(year);
  set_month(month);
  set_date(date);
  set_weekday(weekday);
  set_hour(hour);
  set_minute(minutes);
  set_second(seconds);
}

void set_datetime_alarm_month(uint8_t month, uint8_t date, uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  set_month_alarm(month);
  set_date_alarm(date);
  set_hour_alarm(hour);
  set_minute_alarm(minutes);
  set_second_alarm(seconds);
  set_RPT_time_control(TIMER_CONTROL_RPT_ONCE_PER_YEAR);
}

void set_datetime_alarm_date(uint8_t date, uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  set_date_alarm(date);
  set_hour_alarm(hour);
  set_minute_alarm(minutes);
  set_second_alarm(seconds);
  set_RPT_time_control(TIMER_CONTROL_RPT_ONCE_PER_MONTH);
}

void set_week_alarm(uint8_t weekday, uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  set_weekday_alarm(weekday);
  set_hour_alarm(hour);
  set_minute_alarm(minutes);
  set_second_alarm(seconds);
  set_RPT_time_control(TIMER_CONTROL_RPT_ONCE_PER_WEEK);
}

void set_datetime_alarm_hour(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  set_hour_alarm(hour);
  set_minute_alarm(minutes);
  set_second_alarm(seconds);
  set_RPT_time_control(TIMER_CONTROL_RPT_ONCE_PER_DAY);
}

void set_datetime_alarm_minutes(uint8_t minutes, uint8_t seconds)
{
  set_minute_alarm(minutes);
  set_second_alarm(seconds);
  set_RPT_time_control(TIMER_CONTROL_RPT_ONCE_PER_HOUR);
}

void set_datetime_alarm_seconds(uint8_t seconds)
{
  set_second_alarm(seconds);
  set_RPT_time_control(TIMER_CONTROL_RPT_ONCE_PER_MINUTE);
}

void set_once_second_alarm(uint8_t hundredths)
{
  // set_second_alarm(seconds);
  set_RPT_time_control(TIMER_CONTROL_RPT_ONCE_PER_SECOND);
}

bool set_second_alarm(uint8_t value)
{
  _alarm_second = value % MAX_SECOND;
  write_rtc_register(SECOND_ALARM_REGISTER, _alarm_second);
  _alarm_second = get_second_alarm();
  return (value == _alarm_second);
}

bool set_minute_alarm(uint8_t value)
{
  _alarm_minute = value % MAX_MINUTE;
  write_rtc_register(MINUTE_ALARM_REGISTER, _alarm_minute);
  _alarm_minute = get_minute_alarm();
  return (value == _alarm_minute);
}

bool set_hour_alarm(uint8_t value)
{
  _alarm_hour = value % MAX_HOURS;
  write_rtc_register(HOUR_ALARM_REGISTER, _alarm_hour);
  _alarm_hour = get_hour_alarm();
  return (value == _alarm_hour);
}

bool set_weekday_alarm(uint8_t value)
{
  _alarm_weekday = value % MAX_DAY;
  write_rtc_register(WEEKDAY_ALARM_REGISTER, _alarm_weekday);
  _alarm_weekday = get_weekday_alarm();
  return (value == _alarm_weekday);
}

bool set_date_alarm(uint8_t value)
{
  _alarm_date = value % MAX_DATE;
  write_rtc_register(DATE_ALARM_REGISTER, _alarm_date);
  _alarm_date = get_date_alarm();
  return (value == _alarm_date);
}

bool set_month_alarm(uint8_t value)
{
  _alarm_month = value % MAX_MONTH;
  write_rtc_register(MONTH_ALARM_REGISTER, _alarm_month);
  _alarm_month = get_month_alarm();
  return (value == _alarm_month);
}

// String get_date_string(void)
// {

//   String date_string;

//   _date = get_date();
//   date_string += ((_date & 0xF0) >> 4);
//   date_string += (_date & 0x0F);

//   date_string += '/';

//   _month = get_month();
//   date_string += ((_month & 0xF0) >> 4);
//   date_string += (_month & 0x0F);

//   date_string += '/';

//   _year = get_year();
//   date_string += ((_year & 0xF0) >> 4);
//   date_string += (_year & 0x0F);

//   date_string += ' ';

//   _hour = get_hour();
//   date_string += ((_hour & 0xF0) >> 4);
//   date_string += (_hour & 0x0F);

//   date_string += ':';

//   _minute = get_minute();
//   date_string += ((_minute & 0xF0) >> 4);
//   date_string += (_minute & 0x0F);

//   date_string += ':';

//   _second = get_second();
//   date_string += ((_second & 0xF0) >> 4);
//   date_string += (_second & 0x0F);

//   return date_string;
// }

char *get_date_time(void)
{
  char *currPos;
  currPos = data_time_string;
  *currPos = 0;
  get_datetime();

  *currPos = ((_date & 0xF0) >> 4) + 0x30;
  currPos++;
  *currPos = (_date & 0x0F) + 0x30;
  currPos++;

  *currPos = '/';
  currPos++;

  *currPos = ((_month & 0xF0) >> 4) + 0x30;
  currPos++;
  *currPos = (_month & 0x0F) + 0x30;
  currPos++;

  *currPos = '/';
  currPos++;

  *currPos = ((_year & 0xF0) >> 4) + 0x30;
  currPos++;
  *currPos = (_year & 0x0F) + 0x30;
  currPos++;

  *currPos = ' ';
  currPos++;

  *currPos = ((_hour & 0xF0) >> 4) + 0x30;
  currPos++;
  *currPos = (_hour & 0x0F) + 0x30;
  currPos++;

  *currPos = ':';
  currPos++;

  *currPos = ((_minute & 0xF0) >> 4) + 0x30;
  currPos++;
  *currPos = (_minute & 0x0F) + 0x30;
  currPos++;

  *currPos = ':';
  currPos++;

  *currPos = ((_second & 0xF0) >> 4) + 0x30;
  currPos++;
  *currPos = (_second & 0x0F) + 0x30;
  currPos++;

  *currPos = ' ';
  currPos++;

  *currPos = ((_hundredth & 0xF0) >> 4) + 0x30;
  currPos++;
  *currPos = (_hundredth & 0x0F) + 0x30;
  currPos++;

  return data_time_string;
}

uint8_t dec_hex(uint8_t tens, uint8_t digits)
{
  uint8_t ret;
  ret = (tens << 4) + digits;
  return ret;
}

uint8_t hex_dec(uint8_t hex)
{
  uint8_t ret;
  ret = ((hex >> 4) & 0xF * 10) + (hex & 0xF);
  return ret;
}

// status
uint8_t get_status(void)
{
  _status = get_rtc_data(STATU_REGISTER, STATUS_READ_ALL_MASK);
  return _status;
}

uint8_t get_CB_status(void)
{
  get_status();
  return (_status & (STATUS_CB_MASK > 7));
};

uint8_t get_BAT_status(void)
{
  get_status();
  return (_status & (STATUS_BAT_MASK > 6));
};

uint8_t get_WDT_status(void)
{
  get_status();
  return (_status & (STATUS_WDT_MASK > 5));
};

uint8_t get_BL_status(void)
{
  get_status();
  return (_status & (STATUS_BL_MASK > 4));
};

uint8_t get_TIM_status(void)
{
  get_status();
  return (_status & (STATUS_TIM_MASK > 3));
};

uint8_t get_ALM_status(void)
{
  get_status();
  return (_status & (STATUS_ALM_MASK > 2));
};

uint8_t get_EX2_status(void)
{
  get_status();
  return (_status & (STATUS_EX2_MASK > 1));
};

uint8_t get_EX1_status(void)
{
  get_status();
  return (_status & (STATUS_EX1_MASK));
};

uint8_t clean_CB_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_CB_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

uint8_t clean_BAT_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_BAT_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

uint8_t clean_WDT_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_WDT_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

uint8_t clean_BL_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_BL_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

uint8_t clean_TIM_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_TIM_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

uint8_t clean_ALM_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_ALM_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

uint8_t clean_EX2_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_EX2_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

uint8_t clean_EX1_status(void)
{
  uint8_t c1;
  c1 = read_rtc_register(STATU_REGISTER);
  c1 &= ~STATUS_EX1_MASK;
  write_rtc_register(STATU_REGISTER, c1);
  return 1;
}

// set func
uint8_t set_control1(uint8_t value)
{
  write_rtc_register(CONTROL1_REGISTER, value);
  _control1 = read_rtc_register(CONTROL1_REGISTER);
  return _control1;
}

uint8_t set_1224(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL1_REGISTER);
  c1 &= ~CONTROL1_1224_MASK;
  if (value)
  {
    c1 |= CONTROL1_1224_MASK;
  }
  write_rtc_register(CONTROL1_REGISTER, c1);
  return 1;
}

uint8_t set_RSP(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL1_REGISTER);
  c1 &= ~CONTROL1_RSP_MASK;
  if (value)
  {
    c1 |= CONTROL1_RSP_MASK;
  }
  write_rtc_register(CONTROL1_REGISTER, c1);
  return 1;
}

uint8_t set_ARST(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL1_REGISTER);
  c1 &= ~CONTROL1_ARST_MASK;
  if (value)
  {
    c1 |= CONTROL1_ARST_MASK;
  }
  write_rtc_register(CONTROL1_REGISTER, c1);
  return 1;
}

uint8_t set_PWR2(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL1_REGISTER);
  c1 &= ~CONTROL1_PWR2_MASK;
  if (value)
  {
    c1 |= CONTROL1_PWR2_MASK;
  }
  write_rtc_register(CONTROL1_REGISTER, c1);
  return 1;
}

uint8_t set_WRTC(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL1_REGISTER);
  c1 &= ~CONTROL1_WRTC_MASK;
  if (value)
  {
    c1 |= CONTROL1_WRTC_MASK;
  }
  write_rtc_register(CONTROL1_REGISTER, c1);
  return 1;
}

// interrupt
uint8_t get_interrupt(void)
{
  _interrupt = read_rtc_register(INT_MASK_REGISTER);
  return _interrupt;
}

uint8_t get_BLIE_interrupt(void)
{
  get_status();
  return (_interrupt & (STATUS_BL_MASK > 4));
};

uint8_t get_TIE_interrupt(void)
{
  get_status();
  return (_interrupt & (STATUS_TIM_MASK > 3));
};

uint8_t get_AIE_interrupt(void)
{
  get_status();
  return (_interrupt & (STATUS_ALM_MASK > 2));
};

uint8_t get_EX2E_interrupt(void)
{
  get_status();
  return (_interrupt & (STATUS_EX2_MASK > 1));
};

uint8_t get_EX1E_interrupt(void)
{
  get_status();
  return (_interrupt & (STATUS_EX1_MASK));
};

uint8_t set_interrupt(uint8_t value)
{
  write_rtc_register(INT_MASK_REGISTER, value);
  _interrupt = read_rtc_register(INT_MASK_REGISTER);
  return _interrupt;
}

uint8_t set_BLIE_interrupt(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(INT_MASK_REGISTER);
  c1 &= ~INTERRUPT_BLIE_MASK;
  if (value)
  {
    c1 |= INTERRUPT_BLIE_MASK;
  }
  write_rtc_register(INT_MASK_REGISTER, c1);
  return 1;
}

uint8_t set_TIE_interrupt(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(INT_MASK_REGISTER);
  c1 &= ~INTERRUPT_TIE_MASK;
  if (value)
  {
    c1 |= INTERRUPT_TIE_MASK;
  }
  write_rtc_register(INT_MASK_REGISTER, c1);
  return 1;
}

uint8_t set_AIE_interrupt(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(INT_MASK_REGISTER);
  c1 &= ~INTERRUPT_AIE_MASK;
  if (value)
  {
    c1 |= INTERRUPT_AIE_MASK;
  }
  write_rtc_register(INT_MASK_REGISTER, c1);
  return 1;
}

uint8_t set_EX1E_interrupt(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(INT_MASK_REGISTER);
  c1 &= ~INTERRUPT_EX1E_MASK;
  if (value)
  {
    c1 |= INTERRUPT_EX1E_MASK;
  }
  write_rtc_register(INT_MASK_REGISTER, c1);
  return 1;
}

uint8_t set_EX2E_interrupt(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(INT_MASK_REGISTER);
  c1 &= ~INTERRUPT_EX2E_MASK;
  if (value)
  {
    c1 |= INTERRUPT_EX2E_MASK;
  }
  write_rtc_register(INT_MASK_REGISTER, c1);
  return 1;
}

// control2
uint8_t get_control2(void)
{
  _control2 = read_rtc_register(CONTROL2_REGISTER);
  return _control2;
}

uint8_t get_RS1E_control2(void)
{
  get_control2();
  return (_control2 & (CONTROL2_RS1E_MASK > 5));
};

uint8_t get_OUT2S_control2(void)
{
  get_control2();
  return (_control2 & (CONTROL2_OUT2S_MASK > 2));
};

uint8_t get_OUT1S_control2(void)
{
  get_control2();
  return (_control2 & (CONTROL2_OUT1S_MASK));
};

uint8_t set_control2(uint8_t value)
{
  write_rtc_register(CONTROL2_REGISTER, value);
  _control2 = read_rtc_register(CONTROL2_REGISTER);
  return _control2;
}

uint8_t set_RS1E_control2(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL2_REGISTER);
  c1 &= ~CONTROL2_RS1E_MASK;
  if (value)
  {
    c1 |= CONTROL2_RS1E_MASK;
  }
  write_rtc_register(CONTROL2_REGISTER, c1);
  return c1;
}

uint8_t set_OUT2S_control2(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL2_REGISTER);
  c1 &= ~CONTROL2_OUT2S_MASK;
  c1 |= (value << 2) & CONTROL2_OUT2S_MASK;
  write_rtc_register(CONTROL2_REGISTER, c1);
  return c1;
}

uint8_t set_OUT1S_control2(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(CONTROL2_REGISTER);
  c1 &= ~CONTROL2_OUT1S_MASK;
  c1 |= (value)&CONTROL2_OUT1S_MASK;
  write_rtc_register(CONTROL2_REGISTER, c1);
  return c1;
}

// sleep mode
uint8_t get_sleep(void)
{
  _sleep_mode = read_rtc_register(SLEEP_CONTROL_REGISTER);
  return _sleep_mode;
}

uint8_t get_SLP_sleep(void)
{
  get_sleep();
  return (_sleep_mode & (SLEEP_CONTROL_SLP_MASK > 7));
};

uint8_t get_SLRES_sleep(void)
{
  get_sleep();
  return (_sleep_mode & (SLEEP_CONTROL_SLRES_MASK > 6));
};

uint8_t get_EX2P_sleep(void)
{
  get_sleep();
  return (_sleep_mode & (SLEEP_CONTROL_EX2P_MASK > 5));
};

uint8_t get_EX1P_sleep(void)
{
  get_sleep();
  return (_sleep_mode & (SLEEP_CONTROL_EX1P_MASK > 4));
};

uint8_t get_SLST_sleep(void)
{
  get_sleep();
  return (_sleep_mode & (SLEEP_CONTROL_SLST_MASK > 3));
};

uint8_t get_SLTO_sleep(void)
{
  get_sleep();
  return (_sleep_mode & (SLEEP_CONTROL_SLTO_MASK));
};

uint8_t set_sleep(uint8_t value)
{
  write_rtc_register(SLEEP_CONTROL_REGISTER, value);
  _sleep_mode = read_rtc_register(SLEEP_CONTROL_REGISTER);
  return _sleep_mode;
}

uint8_t set_SLP_sleep(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(SLEEP_CONTROL_REGISTER);
  c1 &= ~SLEEP_CONTROL_SLP_MASK;
  if (value)
  {
    c1 |= SLEEP_CONTROL_SLP_MASK;
  }
  write_rtc_register(SLEEP_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_SLRES_sleep(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(SLEEP_CONTROL_REGISTER);
  c1 &= ~SLEEP_CONTROL_SLRES_MASK;
  if (value)
  {
    c1 |= SLEEP_CONTROL_SLRES_MASK;
  }
  write_rtc_register(SLEEP_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_EX2P_sleep(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(SLEEP_CONTROL_REGISTER);
  c1 &= ~SLEEP_CONTROL_EX2P_MASK;
  if (value)
  {
    c1 |= SLEEP_CONTROL_EX2P_MASK;
  }
  write_rtc_register(SLEEP_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_EX1P_sleep(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(SLEEP_CONTROL_REGISTER);
  c1 &= ~SLEEP_CONTROL_EX1P_MASK;
  if (value)
  {
    c1 |= SLEEP_CONTROL_EX1P_MASK;
  }
  write_rtc_register(SLEEP_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t clean_SLST_sleep(void)
{
  uint8_t c1;
  c1 = read_rtc_register(SLEEP_CONTROL_REGISTER);
  c1 &= ~SLEEP_CONTROL_SLST_MASK;
  write_rtc_register(SLEEP_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_SLTO_sleep(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(SLEEP_CONTROL_REGISTER);
  c1 &= ~SLEEP_CONTROL_SLTO_MASK;
  c1 |= value;
  write_rtc_register(SLEEP_CONTROL_REGISTER, c1);
  return c1;
}

// countdown control
uint8_t get_time_control(void)
{
  _timer_control_mode = read_rtc_register(TIMER_CONTROL_REGISTER);
  return _timer_control_mode;
}

uint8_t get_TE_time_control(void)
{
  get_time_control();
  return (_timer_control_mode & (TIMER_CONTROL_TE_MASK > 7));
};

uint8_t get_TM_time_control(void)
{
  get_time_control();
  return (_timer_control_mode & (TIMER_CONTROL_TM_MASK > 6));
};

uint8_t get_TRPT_time_control(void)
{
  get_time_control();
  return (_timer_control_mode & (TIMER_CONTROL_TRPT_MASK > 5));
};

uint8_t get_RPT_time_control(void)
{
  get_time_control();
  return (_timer_control_mode & (TIMER_CONTROL_RPT_MASK > 2));
};

uint8_t get_TFS_time_control(void)
{
  get_time_control();
  return (_timer_control_mode & (TIMER_CONTROL_TFS_MASK));
};

uint8_t set_time_control(uint8_t value)
{
  write_rtc_register(TIMER_CONTROL_REGISTER, value);
  _timer_control_mode = read_rtc_register(TIMER_CONTROL_REGISTER);
  return _timer_control_mode;
}

uint8_t set_TE_time_control(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(TIMER_CONTROL_REGISTER);
  c1 &= ~TIMER_CONTROL_TE_MASK;
  if (value)
  {
    c1 |= TIMER_CONTROL_TE_MASK;
  }
  write_rtc_register(TIMER_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_TM_time_control(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(TIMER_CONTROL_REGISTER);
  c1 &= ~TIMER_CONTROL_TM_MASK;
  if (value)
  {
    c1 |= TIMER_CONTROL_TM_MASK;
  }
  write_rtc_register(TIMER_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_TRPT_time_control(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(TIMER_CONTROL_REGISTER);
  c1 &= ~TIMER_CONTROL_TRPT_MASK;
  if (value)
  {
    c1 |= TIMER_CONTROL_TRPT_MASK;
  }
  write_rtc_register(TIMER_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_RPT_time_control(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(TIMER_CONTROL_REGISTER);
  c1 &= ~TIMER_CONTROL_RPT_MASK;
  c1 |= (value << 2) & TIMER_CONTROL_RPT_MASK;
  write_rtc_register(TIMER_CONTROL_REGISTER, c1);
  return c1;
}

uint8_t set_TFS_time_control(uint8_t value)
{
  uint8_t c1;
  c1 = read_rtc_register(TIMER_CONTROL_REGISTER);
  c1 &= ~TIMER_CONTROL_TFS_MASK;
  c1 |= value;
  write_rtc_register(TIMER_CONTROL_REGISTER, c1);
  return c1;
}

// outcontrol registers
uint8_t get_outcontrol(void)
{
  _outcontrol = read_rtc_register(OCTRL_REGISTER);
  return _outcontrol;
}

uint8_t get_outcontrol_mask(uint8_t mask)
{
  uint8_t ret;
  ret = get_outcontrol() & mask;
  switch (mask)
  {
  case OCTRL_WDBM_MASK:
    ret = ret >> 7;
    break;
  case OCTRL_EXBM_MASK:
    ret = ret >> 6;
    break;
  case OCTRL_WDDS_MASK:
    ret = ret >> 5;
    break;
  case OCTRL_EXDS_MASK:
    ret = ret >> 4;
    break;
  case OCTRL_RSEN_MASK:
    ret = ret >> 3;
    break;
  case OCTRL_O4EN_MASK:
    ret = ret >> 2;
    break;
  case OCTRL_O3EN_MASK:
    ret = ret >> 1;
    break;
  case OCTRL_O1EN_MASK:
    ret = ret;
    break;
  default:
    ret = _outcontrol;
  }
  return ret;
}

uint8_t set_outcontrol_value(uint8_t value)
{
  write_rtc_register(OCTRL_REGISTER, value);
  _outcontrol = read_rtc_register(OCTRL_REGISTER);
  return _outcontrol;
}

uint8_t set_outcontrol_value_mask(uint8_t value, uint8_t mask)
{
  uint8_t c1;
  uint32_t primask_bit = utils_enter_critical_section();

  c1 = get_outcontrol();
  switch (mask)
  {
  case OCTRL_WDBM_MASK:
    c1 &= ~OCTRL_WDBM_MASK;
    c1 |= (value << 7) & OCTRL_WDBM_MASK;
    break;
  case OCTRL_EXBM_MASK:
    c1 &= ~OCTRL_EXBM_MASK;
    c1 |= (value << 6) & OCTRL_EXBM_MASK;
    break;
  case OCTRL_WDDS_MASK:
    c1 &= ~OCTRL_WDDS_MASK;
    c1 |= (value << 5) & OCTRL_WDDS_MASK;
    break;
  case OCTRL_EXDS_MASK:
    c1 &= ~OCTRL_EXDS_MASK;
    c1 |= (value << 4) & OCTRL_EXDS_MASK;
    break;
  case OCTRL_RSEN_MASK:
    c1 &= ~OCTRL_RSEN_MASK;
    c1 |= (value << 3) & OCTRL_RSEN_MASK;
    break;
  case OCTRL_O4EN_MASK:
    c1 &= ~OCTRL_O4EN_MASK;
    c1 |= (value << 2) & OCTRL_O4EN_MASK;
    break;
  case OCTRL_O3EN_MASK:
    c1 &= ~OCTRL_O3EN_MASK;
    c1 |= (value << 1) & OCTRL_O3EN_MASK;
    break;
  case OCTRL_O1EN_MASK:
    c1 &= ~OCTRL_O1EN_MASK;
    c1 |= (value)&OCTRL_O1EN_MASK;
    break;
  case CCTRL_SLEEP_MODE_MASK:
    // special mode for sleep mode, to Avoiding Unexpected Leakage Paths
    c1 &= ~CCTRL_SLEEP_MODE_MASK;
    c1 |= (value)&CCTRL_SLEEP_MODE_MASK;
  default:
    c1 = value;
  }
  return set_outcontrol_value(c1);
  utils_exit_critical_section(primask_bit);
}

// OSC control registers
uint8_t get_osc_control(void)
{
  _osc_control = read_rtc_register(OSC_CONTROL_REGISTER);
  return _osc_control;
}

uint8_t get_osc_control_mask(uint8_t mask)
{
  uint8_t ret;
  ret = get_osc_control() & mask;
  switch (mask)
  {
  case OSC_CONTROL_OSEL_MASK:
    ret = ret >> 7;
    break;
  case OSC_CONTROL_ACAL_MASK:
    ret = ret >> 5;
    break;
  case OSC_CONTROL_AOS_MASK:
    ret = ret >> 4;
    break;
  case OSC_CONTROL_FOS_MASK:
    ret = ret >> 3;
    break;
  case OSC_CONTROL_PWGT_MASK:
    ret = ret >> 2;
    break;
  case OSC_CONTROL_OFIE_MASK:
    ret = ret >> 1;
    break;
  case OSC_CONTROL_ACIE_MASK:
    ret = ret;
    break;
  default:
    ret = _osc_control;
  }
  return ret;
}

uint8_t set_osc_control_value(uint8_t value)
{
  write_rtc_register(OSC_CONTROL_REGISTER, value);
  _osc_control = read_rtc_register(OSC_CONTROL_REGISTER);
  return _osc_control;
}

uint8_t set_osc_control_value_mask(uint8_t value, uint8_t mask)
{
  uint8_t c1;
  c1 = get_osc_control();
  switch (mask)
  {
  case OSC_CONTROL_OSEL_MASK:
    c1 &= ~OSC_CONTROL_OSEL_MASK;
    c1 |= (value << 7) & OSC_CONTROL_OSEL_MASK;
    break;
  case OSC_CONTROL_ACAL_MASK:
    c1 &= ~OSC_CONTROL_ACAL_MASK;
    c1 |= (value << 5) & OSC_CONTROL_ACAL_MASK;
    break;
  case OSC_CONTROL_AOS_MASK:
    c1 &= ~OSC_CONTROL_AOS_MASK;
    c1 |= (value << 4) & OSC_CONTROL_AOS_MASK;
    break;
  case OSC_CONTROL_FOS_MASK:
    c1 &= ~OSC_CONTROL_FOS_MASK;
    c1 |= (value << 3) & OSC_CONTROL_FOS_MASK;
    break;
  case OSC_CONTROL_PWGT_MASK:
    c1 &= ~OSC_CONTROL_PWGT_MASK;
    c1 |= (value << 2) & OSC_CONTROL_PWGT_MASK;
    break;
  case OSC_CONTROL_OFIE_MASK:
    c1 &= ~OSC_CONTROL_OFIE_MASK;
    c1 |= (value << 1) & OSC_CONTROL_OFIE_MASK;
    break;
  case OSC_CONTROL_ACIE_MASK:
    c1 &= ~OSC_CONTROL_ACIE_MASK;
    c1 |= (value)&OSC_CONTROL_ACIE_MASK;
    break;
  default:
    c1 = value;
  }
  return set_osc_control_value(c1);
}

// WDT register
uint8_t get_WDT_register(void)
{
  _wdt_register = read_rtc_register(WDT_REGISTER);
  return _wdt_register;
}

uint8_t get_WDT_register_mask(uint8_t mask)
{
  uint8_t ret;
  ret = get_WDT_register() & mask;
  switch (mask)
  {
  case WDT_REGISTER_WDS_MASK:
    ret = ret >> 7;
    break;
  case WDT_REGISTER_BMB_MASK:
    ret = ret >> 2;
    break;
  case WDT_REGISTER_WRB_MASK:
    ret = ret;
    break;
  default:
    ret = _wdt_register;
  }
  return ret;
}

uint8_t set_value(uint8_t value)
{
  write_rtc_register(WDT_REGISTER, value);
  _wdt_register = read_rtc_register(WDT_REGISTER);
  return _wdt_register;
}

uint8_t set_WDT_register_mask(uint8_t value, uint8_t mask)
{
  uint8_t c1;
  c1 = get_WDT_register();
  switch (mask)
  {
  case WDT_REGISTER_WDS_MASK:
    c1 &= ~WDT_REGISTER_WDS_MASK;
    c1 |= (value << 7) & WDT_REGISTER_WDS_MASK;
    break;
  case WDT_REGISTER_BMB_MASK:
    c1 &= ~WDT_REGISTER_BMB_MASK;
    c1 |= (value << 2) & WDT_REGISTER_BMB_MASK;
    break;
  case WDT_REGISTER_WRB_MASK:
    c1 &= ~WDT_REGISTER_WRB_MASK;
    c1 |= (value)&WDT_REGISTER_WRB_MASK;
    break;
  default:
    c1 = value;
  }
  return set_WDT_register_value(c1);
}

// void enter_sleep_mode_PWR(void)
// {
//   enter_sleep_mode_PWR_value(SLEEP_CONTROL_SLTO_MAX_VALUE);
// }

void enter_sleep_mode_PWR_value(uint8_t value)
{
  if (get_SLST_sleep())
  { // clean previous sleep
    clean_SLST_sleep();
  }
  // set OUT2S sleep mode
  // the OUT2S field must be set to a value of 6 to select the SLEEP output.
  set_OUT2S_control2(CONTROL2_SLEEP_MODE);

  // Avoiding Unexpected Leakage Paths
  // clearing the RSEN,O1EN,O4EN,O3EN,EXDS,WDDS bits in output control register
  set_outcontrol_value_mask(0, CCTRL_SLEEP_MODE_MASK);

  // The I2C or SPI interface pins are disabled in Sleep Mode by setting the PWGT bit in the Oscillator Control Register.
  set_osc_control_value_mask(0, OSC_CONTROL_PWGT_MASK);

  // The number of 7.8 ms periods after SLP is set until the Power Control SM goes into the SLEEP state.
  set_SLTO_sleep(value);

  // set sleep mode
  set_SLP_sleep(1);
}

/** Parameter:
 *  timeout - minimum timeout period in 7.8 ms periods (0 to 7)
 *  mode - sleep mode (nRST modes not available in AM08xx)
 *      0 => nRST is pulled low in sleep mode
 *      1 => PSW/nIRQ2 is pulled high on a sleep      <-- This one!!!
 *      2 => nRST pulled low and PSW/nIRQ2 pulled high on sleep
 *  error ?returned value of the attempted sleep command
 *      0 => sleep request accepted, sleep mode will be initiated in timeout seconds
 *      1 => illegal input values
 *      2 => sleep request declined, interrupt is currently pending
 *      3 => sleep request declined, no sleep trigger interrupt enabled
 **/
uint8_t enter_sleep_mode(uint8_t timeout, uint8_t mode)
{
  uint8_t ret = 0;
  uint8_t slres = 0;
  char temp = 0;

#ifdef DEBUG_SIO
  temp = get_SLST_sleep(); // Get SLST bit (temp & 0x08)

  if ((temp) == 0)
  {
    printf("\r\nPrevious Sleep Failed");
  }
  else
  {
    printf("\r\nPrevious Sleep Successful");
  }
  clean_SLST_sleep(); // Clear SLST

  temp = get_SLST_sleep(); // Get SLST bit (temp & 0x08)

  if ((temp) == 0)
  {
    printf("\r\nClear Succ");
  }
  else
  {
    printf("\r\nClear Fail");
  }
  clean_SLST_sleep(); // Clear SLST
#endif

  if (mode > 0)
  {
    /* Sleep to PSW/nIRQ2 */
    temp = get_OUT2S_control2(); // Read OUT2S
    temp = (temp & 0xE3) | 0x18; // MUST NOT WRITE OUT2S WITH 000
    set_OUT2S_control2(temp);    // Write value to OUT2S
    slres = 0;
  }

  temp = timeout | (slres << 6) | 0x80; // Assemble SLEEP register value
  set_sleep(temp);                      // Write to the register

#ifdef DEBUG_SIO
  /* Determine if SLEEP was accepted */
  temp = get_SLP_sleep(); // Get SLP bit (temp & 0x80)

  if ((temp) == 0)
  {
    char reg_wdi_value = 0;
    /* SLEEP did not happen - determine why and return reason. */
    temp = get_interrupt(); // Get status register interrupt enables
    reg_wdi_value = get_WDT_register();
    if (((temp & 0x0F) == 0) & (((reg_wdi_value & 0x7C) == 0) || ((reg_wdi_value & 0x80) == 0x80)))
    {
      ret = 3;
      printf("\r\nNo trigger interrupts enabled");
    }
    else
    {
      ret = 2;
      printf("\r\nInterrupt pending");
    }
  }
  else
  {
    ret = 0;
    printf("\r\nSLEEP request successful");
  }
#endif
  return ret;
}

// *********************************__________ MY_____________**********************
inline static uint8_t SPI1_SendByte(uint8_t data)
{
  while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET)
    ;
  LL_SPI_TransmitData8(SPI1, data);

  while (LL_SPI_IsActiveFlag_RXNE(SPI1) == RESET)
    ;
  return LL_SPI_ReceiveData8(SPI1);
}

inline static uint8_t read_rtc_register(uint8_t reg_addr)
{
  uint8_t val;
  uint32_t primask_bit = utils_enter_critical_section();

  // #define AB1815_SPI_READ(offset) (127 & offset)		127 - 0x7F
  // #define AB1815_SPI_WRITE(offset) (128 | offset)  	128 - 0x80
  uint8_t addr = AB1815_SPI_READ(reg_addr);
  RTC_L();
  SPI1_SendByte(addr);
  val = SPI1_SendByte(0x00); // Send DUMMY to read data
  RTC_H();
  utils_exit_critical_section(primask_bit);

  return val;
}

// bool read(uint8_t offset, uint8_t *buf, uint8_t length)
// {
//   uint8_t address = AB1815_SPI_READ(offset);

//   spi_select_slave(0);

//   unsigned int i = 0;
//   if (!((SPI1)->CR1 & SPI_CR1_SPE))
//   {
//     SPI1->CR1 |= SPI_CR1_SPE;
//   }

//   SPI1_SendByte(address);
//   while (i < length)
//   {
//     buf[i++] = SPI1_SendByte(0x00); // Send DUMMY to read data
//   }

//   spi_select_slave(1);
//   return ab1815_status_e_OK;
// };

uint8_t write_rtc_register(uint8_t offset, uint8_t buf)
{
  uint8_t address = AB1815_SPI_WRITE(offset);
  uint32_t primask_bit = utils_enter_critical_section();

  spi_select_slave(0);

  if (!((SPI1)->CR1 & SPI_CR1_SPE))
  {
    SPI1->CR1 |= SPI_CR1_SPE;
  }
  RTC_L();
  SPI1_SendByte(address);
  SPI1_SendByte(buf); // Send Data to write

  RTC_H();
  utils_exit_critical_section(primask_bit);
  return 1;
};

void hex_dump(void)
{
  uint8_t buffer[9];
  for (uint8_t pos = 0; pos < 0x7F; pos += 8)
  {

    uint8_t ii = 0;
    for (ii = 0; ii < 7; ii++)
    {
      buffer[ii] = read_rtc_register(pos + ii);
    }
    printf("# 0x%02x: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\r\n", pos, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
  }
}

bool resetConfig(void)
{
  printf("resetConfig\n");

  // wire.lock();

  // // Reset configuration registers to default values
  // writeRegister(REG_STATUS, REG_STATUS_DEFAULT, false);
  // writeRegister(REG_CTRL_1, REG_CTRL_1_DEFAULT, false);
  // writeRegister(REG_CTRL_2, REG_CTRL_2_DEFAULT, false);
  // writeRegister(REG_INT_MASK, REG_INT_MASK_DEFAULT, false);
  // writeRegister(REG_SQW, REG_SQW_DEFAULT, false);
  // writeRegister(REG_SLEEP_CTRL, REG_SLEEP_CTRL_DEFAULT, false);

  // writeRegister(REG_TIMER_CTRL, REG_TIMER_CTRL_DEFAULT, false);

  // writeRegister(REG_TIMER, REG_TIMER_DEFAULT, false);
  // writeRegister(REG_TIMER_INITIAL, REG_TIMER_INITIAL_DEFAULT, false);
  // writeRegister(REG_WDT, REG_WDT_DEFAULT, false);

  // uint8_t oscCtrl = REG_OSC_CTRL_DEFAULT;

  // writeRegister(REG_OSC_CTRL, oscCtrl, false);
  // writeRegister(REG_TRICKLE, REG_TRICKLE_DEFAULT, false);
  // writeRegister(REG_BREF_CTRL, REG_BREF_CTRL_DEFAULT, false);
  // writeRegister(REG_AFCTRL, REG_AFCTRL_DEFAULT, false);
  // writeRegister(REG_BATMODE_IO, REG_BATMODE_IO_DEFAULT, false);
  // writeRegister(REG_OCTRL, REG_OCTRL_DEFAULT, false);

  write_rtc_register(STATU_REGISTER, 0x0);          //!< Status register, default
  write_rtc_register(CONTROL1_REGISTER, 0x13);      //!< Control register 1, 0b00010011 (OUT | RSO | PWR2 | WRTC)
  write_rtc_register(CONTROL2_REGISTER, 0x3c);      //!< Control register 2, 0b00111100 (OUT2S = OUTB)
  write_rtc_register(INT_MASK_REGISTER, 0xe0);      //!< Interrupt mask, default 0b11100000 (CEB | IM=1/4 seconds)
  write_rtc_register(SQW_REGISTER, 0x26);           //!< Square wave output control, default 0b00100110
  write_rtc_register(SLEEP_CONTROL_REGISTER, 0x00); //!< Sleep control default (0b00000000)
  write_rtc_register(TIMER_CONTROL_REGISTER, 0x23); //!< Timer control default (0b00010011)//!< Countdown timer control, 0b00100011 (TFPT + TFS = 1/60 Hz0)
  write_rtc_register(TIMER_REGISTER, 0x00);         //!< Countdown timer current value register default value (0x00)
  write_rtc_register(TIMER_INITIAL_REGISTER, 0x0);  //!< Countdown timer inital value register default value
  write_rtc_register(WDT_REGISTER, 0x0);            //!< Watchdog timer control, default value
  write_rtc_register(OSC_CONTROL_REGISTER, 0x0);    //!< Oscillator control register, default value
  write_rtc_register(TRICKLE_REGISTER, 0x0);        //!< Trickle charger control register, default value
  write_rtc_register(BREF_CONTROL_REGISTER, 0xf0);  //!< Wakeup control system default 0b11110000
  write_rtc_register(AFCTRL_REGISTER, 0x0);         //!< Auto-calibration filter, default
  write_rtc_register(BATMODE_REGISTER, 0x80);       //!< Brownout control for IO interface, default value
  write_rtc_register(OCTRL_REGISTER, 0x0);          //!< Output control register, default

  // wire.unlock();

  return true;
}

static inline uint32_t utils_enter_critical_section(void)
{
  uint32_t primask_bit = __get_PRIMASK();
  __disable_irq();
  return primask_bit;
}

static inline void utils_exit_critical_section(uint32_t primask_bit)
{
  __set_PRIMASK(primask_bit);
}

// ****************************__________ deepPowerDown __________****************************

bool deepPowerDown(int seconds)
{
  static const char *errorMsg = "failure in deepPowerDown %d";
  bool bResult;

  printf("deepPowerDown %d", seconds);

  // Disable watchdog
  // bResult = setWDT(0);
  bResult = set_WDT_register_mask(0, 0x7c); // BMB
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  bResult = setCountdownTimer(seconds, false);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // Make sure STOP (stop clocking system is 0, otherwise sleep mode cannot be entered)
  // PWR2 = 1 (low resistance power switch)
  // (also would probably work with PWR2 = 0, as nIRQ2 should be high-true for sleep mode)
  bResult = maskRegister(REG_CTRL_1, (uint8_t) ~(0x80 | 0x08), 0x02);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // Disable the I/O interface in sleep
  bResult = setRegisterBit(REG_OSC_CTRL, REG_OSC_CTRL_PWGT);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // OUT2S = 6 to enable sleep mode
  bResult = maskRegister(REG_CTRL_2, (uint8_t)~REG_CTRL_2_OUT2S_MASK, REG_CTRL_2_OUT2S_SLEEP);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // Enter sleep mode and set nRST low
  bResult = writeRegister(REG_SLEEP_CTRL, REG_SLEEP_CTRL_SLP | REG_SLEEP_CTRL_SLRES);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // _log.trace("delay in case we didn't power down");
  unsigned long start = millis();
  while (millis() - start < (unsigned long)(seconds * 1000))
  {
    printf("REG_SLEEP_CTRL=0x%2x", readRegister(REG_SLEEP_CTRL));
    delay(1000);
  }

  printf("didn't power down");
  return true;
}

// ****************************__________ setcountdownTimer __________****************************

bool setCountdownTimer(int value, bool minutes)
{
  static const char *errorMsg = "failure in setCountdownTimer %d";
  bool bResult;

  // Clear any pending interrupts
  bResult = write_rtc_register(STATU_REGISTER, 0x00);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // Stop countdown timer if already running since it can't be set while running
  bResult = write_rtc_register(TIMER_CONTROL_REGISTER, 0x23);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // Set countdown timer duration
  if (value < 1)
  {
    value = 1;
  }
  if (value > 255)
  {
    value = 255;
  }
  bResult = write_rtc_register(TIMER_REGISTER, (uint8_t)value);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // Enable countdown timer interrupt (TIE = 1) in IntMask
  // bResult = setRegisterBit(REG_INT_MASK, REG_INT_MASK_TIE);
  bResult = set_TIE_interrupt(1);
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  // Set the TFS frequency to 1/60 Hz for minutes or 1 Hz for seconds
  // static const uint8_t   REG_TIMER_CTRL_TFS_1     = 0x02;      //!< Countdown timer control, clock frequency 1 Hz
  // static const uint8_t   REG_TIMER_CTRL_TFS_1_60  = 0x03;      //!< Countdown timer control, clock frequency
  uint8_t tfs = (minutes ? 0x03 : 0x02);

  // Enable countdown timer (TE = 1) in countdown timer control register
  // static const uint8_t   REG_TIMER_CTRL_TE        = 0x80;      //!< Countdown timer control, timer enable
  bResult = write_rtc_register(TIMER_CONTROL_REGISTER, TIMER_CONTROL_TE_MASK | tfs); // TIMER_CONTROL_TE_MASK 0x80
  if (!bResult)
  {
    printf(errorMsg, __LINE__);
    return false;
  }

  return true;
}
