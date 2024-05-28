/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    AB1815.h
 * @brief   Drivers for AB1815
 ******************************************************************************
 * @attention
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AB1815_H
#define __AB1815_H
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//#define PIN_INVALID 0

// static inline uint32_t utils_enter_critical_section(void)
// {
//   uint32_t primask_bit = __get_PRIMASK();
//   __disable_irq();
//   return primask_bit;
// }

// static inline void utils_exit_critical_section(uint32_t primask_bit)
// {
//   __set_PRIMASK(primask_bit);
// }

/**
 * @brief Which GPIO is connected to FOUT/nIRQ
 *
 * Set to PIN_INVALID if not connected.
 *
 * This is used for interrupts, and also to detect if the AB1805 is alive, during
 * detectChip(). If not connected, then only I2C is used to detect the chip.
 *
    emap.cs_pin = PIN_INVALID; // default
    remap.reset_pin = PIN_INVALID; // default
    remap.int_pin = PIN_INVALID; // default
 */
//uint16_t foutPin = 0;

/**
 * @brief The AB18X5 includes a standard I2C interface. The device is accessed at addresses 0xD2/D3, and
supports Fast Mode (up to 400 kHz).
 */
//uint8_t i2cAddr = 0xD2;

/**
 * @brief Checks the I2C bus to make sure there is an AB1815 present
 *
 * This is called during AB1805::setup().
 */
bool detectChip();

/**
 * @brief Resets the configuration of the AB1805 to default settings
 *
 * @param flags flags to customize reset behavior (default: 0)
 *
 * @return true on success or false if an error occurs.
 *
 * The only exception currently defined is `AB1805::RESET_PRESERVE_REPEATING_TIMER` that
 * keeps repeating timers programmed when resetting configuration.
 */
bool resetConfig(uint32_t flags);

/**
 * @brief Reads a AB1805 register (single byte)
 *
 * @param regAddr Register address to read from (0x00 - 0xff)
 *
 * @param value Filled in with the value from the register
 *
 * @param lock Lock the I2C bus. Default = true. Pass false if surrounding a block of
 * related calls with a wire.lock() and wire.unlock() so the block cannot be interrupted
 * with other I2C operations.
 *
 * @return true on success or false on error
 *
 * There is also an overload that returns value instead of passing it by reference.
 */
// bool readRegister(uint8_t regAddr, uint8_t &value, bool lock = true);

/**
 * @brief Reads a AB1815 register (single byte) and returns it
 *
 * @param regAddr Register address to read from (0x00 - 0xff)
 *
 * @param lock Lock the I2C bus. Default = true. Pass false if surrounding a block of
 * related calls with a wire.lock() and wire.unlock() so the block cannot be interrupted
 * with other I2C operations.
 *
 * @return The value of the register or 0x00 if it could not be read.
 *
 * There is an overload that takes a uint8_t &value parameter which is generally
 * better because you can tell the difference between a value of 0 and a failure
 * (false is returned).
 */
uint8_t readRegister_value(uint8_t regAddr, bool lock);

bool readRegister(uint8_t regAddr, uint8_t value, bool lock);

/**
 * @brief Reads sequential registers
 *
 * @param regAddr Register address to start reading from (0x00 - 0xff)
 *
 * @param array Array of uint8_t values, filled in by this call
 *
 * @param num Number of registers to read
 *
 * @param lock Lock the I2C bus. Default = true. Pass false if surrounding a block of
 * related calls with a wire.lock() and wire.unlock() so the block cannot be interrupted
 * with other I2C operations.
 *
 * Reads a number of registers at once. This is done when reading the RTC value so
 * it's atomic (counters will not be incremented in the middle of a read). Also used
 * for reading the device RAM.
 *
 * Do not read past address 0xff.
 */
bool readRegisters(uint8_t regAddr, uint8_t *array, uint8_t num, bool lock);
/**
 * @brief Writes a AB1805 register (single byte)
 *
 * @param regAddr Register address to write to (0x00 - 0xff)
 *
 * @param value This value is written to the register
 *
 * @param lock Lock the I2C bus. Default = true. Pass false if surrounding a block of
 * related calls with a wire.lock() and wire.unlock() so the block cannot be interrupted
 * with other I2C operations.
 *
 * @return true on success or false on error
 */
bool writeRegister(uint8_t regAddr, uint8_t value, bool lock);

/**
 * @brief Writes sequential AB1805 registers
 *
 * @param regAddr Register address to start writing to (0x00 - 0xff)
 *
 * @param array Array of uint8_t values to write
 *
 * @param num Number of registers to write
 *
 * @param lock Lock the I2C bus. Default = true. Pass false if surrounding a block of
 * related calls with a wire.lock() and wire.unlock() so the block cannot be interrupted
 * with other I2C operations.
 *
 * @return true on success or false on error
 *
 * Do not write past address 0xff.
 */
bool writeRegisters(uint8_t regAddr, uint8_t *array, size_t num, bool lock);

static const int WATCHDOG_MAX_SECONDS = 124; //!< Maximum value that can be passed to setWDT().

static const uint8_t REG_HUNDREDTH = 0x00;             //!< Hundredths of a second, 2 BCD digits
static const uint8_t REG_SECOND = 0x01;                //!< Seconds, 2 BCD digits, MSB is GP0
static const uint8_t REG_MINUTE = 0x02;                //!< Minutes, 2 BCD digits, MSB is GP1
static const uint8_t REG_HOUR = 0x03;                  //!< Hours, GP2, GP3
static const uint8_t REG_DATE = 0x04;                  //!< Day of month (1-31), 2 BCD digits, GP4, GP5
static const uint8_t REG_MONTH = 0x05;                 //!< Month (1-12), 2 BCD digits, GP6 - GP8
static const uint8_t REG_YEAR = 0x06;                  //!< Year (0-99), 2 BCD digits
static const uint8_t REG_WEEKDAY = 0x07;               //!< Weekday (0-6), GP9 - GP13
static const uint8_t REG_HUNDREDTH_ALARM = 0x08;       //!< Alarm on hundredths of a second (0-99), 2 BCD digits
static const uint8_t REG_SECOND_ALARM = 0x09;          //!< Alarm on seconds (0-59), 2 BCD digits, GP14
static const uint8_t REG_MINUTE_ALARM = 0x0a;          //!< Alarm on minutes (0-59), 2 BCD digits, GP15
static const uint8_t REG_HOUR_ALARM = 0x0b;            //!< Alarm on hour, GP16, GP17
static const uint8_t REG_DATE_ALARM = 0x0c;            //!< Alarm on date (1-31), 2 BCD digits, GP18-GP19
static const uint8_t REG_MONTH_ALARM = 0x0d;           //!< Alarm on month (1-12). 2 BCD digits, GP20-GP22
static const uint8_t REG_WEEKDAY_ALARM = 0x0e;         //!< Alarm on day of week (0-6). GP23-GP27
static const uint8_t REG_STATUS = 0x0f;                //!< Status register
static const uint8_t REG_STATUS_CB = 0x80;             //!< Status register century bit mask
static const uint8_t REG_STATUS_BAT = 0x40;            //!< Status register switched to VBAT bit mask
static const uint8_t REG_STATUS_WDT = 0x20;            //!< Status register watchdog timer enabled and triggered bit mask
static const uint8_t REG_STATUS_BL = 0x10;             //!< Status register battery voltage crossing bit mask
static const uint8_t REG_STATUS_TIM = 0x08;            //!< Status register countdown timer reaches 0 bit mask
static const uint8_t REG_STATUS_ALM = 0x04;            //!< Status register alarm register match bit mask
static const uint8_t REG_STATUS_EX2 = 0x02;            //!< Status register WDI interrupt bit mask
static const uint8_t REG_STATUS_EX1 = 0x01;            //!< Status register EXTI interrupt bit mask
static const uint8_t REG_STATUS_DEFAULT = 0x00;        //!< Status register, default
static const uint8_t REG_CTRL_1 = 0x10;                //!< Control register 1
static const uint8_t REG_CTRL_1_STOP = 0x80;           //!< Control register 1, stop clocking system
static const uint8_t REG_CTRL_1_12_24 = 0x40;          //!< Control register 1, 12/24 hour mode select (0 = 24 hour)
static const uint8_t REG_CTRL_1_OUTB = 0x20;           //!< Control register 1, value for nIRQ2
static const uint8_t REG_CTRL_1_OUT = 0x10;            //!< Control register 1, value for FOUT/nIRQ
static const uint8_t REG_CTRL_1_RSP = 0x08;            //!< Control register 1, Reset polarity
static const uint8_t REG_CTRL_1_ARST = 0x04;           //!< Control register 1, Auto reset enable
static const uint8_t REG_CTRL_1_PWR2 = 0x02;           //!< Control register 1, PWW/nIRQ pull-down enable
static const uint8_t REG_CTRL_1_WRTC = 0x01;           //!< Control register 1, write RTC mode
static const uint8_t REG_CTRL_1_DEFAULT = 0x13;        //!< Control register 1, 0b00010011 (OUT | RSO | PWR2 | WRTC)
static const uint8_t REG_CTRL_2 = 0x11;                //!< Control register 2
static const uint8_t REG_CTRL_2_RS1E = 0x20;           //!< Control register 2, nIRQ2 output mode
static const uint8_t REG_CTRL_2_OUT2S_MASK = 0x1c;     //!< Control register 2, nIRQ2 output mode
static const uint8_t REG_CTRL_2_OUT2S_nIRQ = 0x00;     //!< Control register 2, nIRQ2 output mode, nIRQ or OUTB
static const uint8_t REG_CTRL_2_OUT2S_SQW = 0x04;      //!< Control register 2, nIRQ2 output mode, SQW or OUTB
static const uint8_t REG_CTRL_2_OUT2S_nAIRQ = 0x0c;    //!< Control register 2, nIRQ2 output mode, nAIRQ or OUTB
static const uint8_t REG_CTRL_2_OUT2S_TIRQ = 0x10;     //!< Control register 2, nIRQ2 output mode, TIRQ or OUTB
static const uint8_t REG_CTRL_2_OUT2S_nTIRQ = 0x14;    //!< Control register 2, nIRQ2 output mode, nTIRQ or OUTB
static const uint8_t REG_CTRL_2_OUT2S_SLEEP = 0x18;    //!< Control register 2, nIRQ2 output mode, sleep mode
static const uint8_t REG_CTRL_2_OUT2S_OUTB = 0x1c;     //!< Control register 2, nIRQ2 output mode, OUTB
static const uint8_t REG_CTRL_2_OUT1S_MASK = 0x03;     //!< Control register 2, FOUT/nIRQ output mode
static const uint8_t REG_CTRL_2_OUT1S_nIRQ = 0x00;     //!< Control register 2, FOUT/nIRQ output mode, nIRQ, or OUT
static const uint8_t REG_CTRL_2_OUT1S_SQW = 0x01;      //!< Control register 2, FOUT/nIRQ output mode, SQW or OUT
static const uint8_t REG_CTRL_2_OUT1S_SQW_nIRQ = 0x02; //!< Control register 2, FOUT/nIRQ output mode, SQW, nIRQ, or OUT
static const uint8_t REG_CTRL_2_OUT1S_nAIRQ = 0x03;    //!< Control register 2, FOUT/nIRQ output mode, nIRQ or OUT
static const uint8_t REG_CTRL_2_DEFAULT = 0x3c;        //!< Control register 2, 0b00111100 (OUT2S = OUTB)
static const uint8_t REG_INT_MASK = 0x12;              //!< Interrupt mask
static const uint8_t REG_INT_MASK_CEB = 0x80;          //!< Interrupt mask, century enable
static const uint8_t REG_INT_MASK_IM = 0x60;           //!< Interrupt mask, interrupt mode bits (2 bits)
static const uint8_t REG_INT_MASK_BLIE = 0x10;         //!< Interrupt mask, battery low interrupt enable
static const uint8_t REG_INT_MASK_TIE = 0x08;          //!< Interrupt mask, timer interrupt enable
static const uint8_t REG_INT_MASK_AIE = 0x04;          //!< Interrupt mask, alarm interrupt enable
static const uint8_t REG_INT_MASK_EX2E = 0x02;         //!< Interrupt mask, XT2 interrupt enable
static const uint8_t REG_INT_MASK_EX1E = 0x01;         //!< Interrupt mask, XT1 interrupt enable
static const uint8_t REG_INT_MASK_DEFAULT = 0xe0;      //!< Interrupt mask, default 0b11100000 (CEB | IM=1/4 seconds)
static const uint8_t REG_SQW = 0x13;                   //!< Square wave output control
static const uint8_t REG_SQW_SQWE = 0x80;              //!< Square wave output control, enable
static const uint8_t REG_SQW_DEFAULT = 0x26;           //!< Square wave output control, default 0b00100110
static const uint8_t REG_CAL_XT = 0x14;                //!< Calibration for the XT oscillator
static const uint8_t REG_CAL_RC_HIGH = 0x15;           //!< Calibration for the RC oscillator, upper 8 bits
static const uint8_t REG_CAL_RC_LOW = 0x16;            //!< Calibration for the RC oscillator, lower 8 bits
static const uint8_t REG_SLEEP_CTRL = 0x17;            //!< Power control system sleep function
static const uint8_t REG_SLEEP_CTRL_SLP = 0x80;        //!< Sleep control, enter sleep mode
static const uint8_t REG_SLEEP_CTRL_SLRES = 0x40;      //!< Sleep control, nRST low on sleep
static const uint8_t REG_SLEEP_CTRL_EX2P = 0x20;       //!< Sleep control, XT2 on rising WDI
static const uint8_t REG_SLEEP_CTRL_EX1P = 0x10;       //!< Sleep control, XT1 on rising EXTI
static const uint8_t REG_SLEEP_CTRL_SLST = 0x08;       //!< Sleep control, set when sleep has occurred
static const uint8_t REG_SLEEP_CTRL_SLTO_MASK = 0x07;  //!< Sleep control, number of 7.8ms periods before sleep
static const uint8_t REG_SLEEP_CTRL_DEFAULT = 0x00;    //!< Sleep control default (0b00000000)
static const uint8_t REG_TIMER_CTRL = 0x18;            //!< Countdown timer control
static const uint8_t REG_TIMER_CTRL_TE = 0x80;         //!< Countdown timer control, timer enable
static const uint8_t REG_TIMER_CTRL_TM = 0x40;         //!< Countdown timer control, timer interrupt mode
static const uint8_t REG_TIMER_CTRL_TRPT = 0x20;       //!< Countdown timer control, timer repeat function
static const uint8_t REG_TIMER_CTRL_RPT_MASK = 0x1c;   //!< Countdown timer control, repeat function
static const uint8_t REG_TIMER_CTRL_RPT_HUN = 0x1c;    //!< Countdown timer control, repeat hundredths match (7)
static const uint8_t REG_TIMER_CTRL_RPT_SEC = 0x18;    //!< Countdown timer control, repeat hundredths, seconds match (once per minute) (6)
static const uint8_t REG_TIMER_CTRL_RPT_MIN = 0x14;    //!< Countdown timer control, repeat hundredths, seconds, minutes match (once per hour) (5)
static const uint8_t REG_TIMER_CTRL_RPT_HOUR = 0x10;   //!< Countdown timer control, repeat hundredths, seconds, minutes, hours match (once per day) (4)
static const uint8_t REG_TIMER_CTRL_RPT_WKDY = 0x0c;   //!< Countdown timer control, repeat hundredths, seconds, minutes, hours, weekday match (once per week) (3)
static const uint8_t REG_TIMER_CTRL_RPT_DATE = 0x08;   //!< Countdown timer control, repeat hundredths, seconds, minutes, hours, date match (once per month) (2)
static const uint8_t REG_TIMER_CTRL_RPT_MON = 0x04;    //!< Countdown timer control, repeat hundredths, seconds, minutes, hours, date, month match (once per year) (1)
static const uint8_t REG_TIMER_CTRL_RPT_DIS = 0x00;    //!< Countdown timer control, alarm disabled (0)
static const uint8_t REG_TIMER_CTRL_TFS_MASK = 0x03;   //!< Countdown timer control, clock frequency
static const uint8_t REG_TIMER_CTRL_TFS_FAST = 0x00;   //!< Countdown timer control, clock frequency 4.096 kHz or 128 Hz
static const uint8_t REG_TIMER_CTRL_TFS_64 = 0x01;     //!< Countdown timer control, clock frequency 64 Hz
static const uint8_t REG_TIMER_CTRL_TFS_1 = 0x02;      //!< Countdown timer control, clock frequency 1 Hz
static const uint8_t REG_TIMER_CTRL_TFS_1_60 = 0x03;   //!< Countdown timer control, clock frequency 1/60 Hz (1 minute)
static const uint8_t REG_TIMER_CTRL_DEFAULT = 0x23;    //!< Countdown timer control, 0b00100011 (TFPT + TFS = 1/60 Hz0)
static const uint8_t REG_TIMER = 0x19;                 //!< Countdown timer current value register
static const uint8_t REG_TIMER_DEFAULT = 0x00;         //!< Countdown timer current value register default value (0x00)
static const uint8_t REG_TIMER_INITIAL = 0x1a;         //!< Countdown timer inital (reload) value register
static const uint8_t REG_TIMER_INITIAL_DEFAULT = 0x00; //!< Countdown timer inital value register default value
static const uint8_t REG_WDT = 0x1b;                   //!< Watchdog timer control register
static const uint8_t REG_WDT_RESET = 0x80;             //!< Watchdog timer control, enable reset (1) or WIRQ (0)
static const uint8_t REG_WDT_WRB_16_HZ = 0x00;         //!< Watchdog timer control, WRB watchdog clock = 16 Hz
static const uint8_t REG_WDT_WRB_4_HZ = 0x01;          //!< Watchdog timer control, WRB watchdog clock = 4 Hz
static const uint8_t REG_WDT_WRB_1_HZ = 0x02;          //!< Watchdog timer control, WRB watchdog clock = 1 Hz
static const uint8_t REG_WDT_WRB_1_4_HZ = 0x03;        //!< Watchdog timer control, WRB watchdog clock = 1/4 Hz
static const uint8_t REG_WDT_DEFAULT = 0x00;           //!< Watchdog timer control, default value
static const uint8_t REG_OSC_CTRL = 0x1c;              //!< Oscillator control register
static const uint8_t REG_OSC_CTRL_OSEL = 0x80;         //!< Oscillator control, clock select 32.768 kHz (0) or 128 Hz (1)
static const uint8_t REG_OSC_CTRL_ACAL = 0x60;         //!< Oscillator control, auto-calibration
static const uint8_t REG_OSC_CTRL_AOS = 0x10;          //!< Oscillator control, automatic switch to RC oscillator on battery
static const uint8_t REG_OSC_CTRL_FOS = 0x08;          //!< Oscillator control, automatic switch to RC oscillator on failure
static const uint8_t REG_OSC_CTRL_PWGT = 0x04;         //!< Oscillator control, IO interface disable
static const uint8_t REG_OSC_CTRL_OFIE = 0x02;         //!< Oscillator control, oscillator fail interrupt enable
static const uint8_t REG_OSC_CTRL_ACIE = 0x01;         //!< Oscillator control, auto-calibration fail interrupt enable
static const uint8_t REG_OSC_CTRL_DEFAULT = 0x00;      //!< Oscillator control, default value
static const uint8_t REG_OSC_STATUS = 0x1d;            //!< Oscillator status register
static const uint8_t REG_OSC_STATUS_XTCAL = 0x0c;      //!< Oscillator status register, extended crystal calibration
static const uint8_t REG_OSC_STATUS_LKO2 = 0x04;       //!< Oscillator status register, lock OUT2
static const uint8_t REG_OSC_STATUS_OMODE = 0x01;      //!< Oscillator status register, oscillator mode (read-only)
static const uint8_t REG_OSC_STATUS_OF = 0x02;         //!< Oscillator status register, oscillator failure
static const uint8_t REG_OSC_STATUS_ACF = 0x01;        //!< Oscillator status register, auto-calibration failure
static const uint8_t REG_CONFIG_KEY = 0x1f;            //!< Register to set to modify certain other keys
static const uint8_t REG_CONFIG_KEY_OSC_CTRL = 0xa1;   //!< Configuration key, enable setting REG_OSC_CTRL
static const uint8_t REG_CONFIG_KEY_SW_RESET = 0x3c;   //!< Configuration key, software reset
static const uint8_t REG_CONFIG_KEY_OTHER = 0x9d;      //!< Configuration key, REG_TRICKLE, REG_BREF_CTRL, REG_AFCTRL, REG_BATMODE_IO, REG_OCTRL
static const uint8_t REG_TRICKLE = 0x20;               //!< Trickle charger control register
static const uint8_t REG_TRICKLE_DEFAULT = 0x00;       //!< Trickle charger control register, default value
static const uint8_t REG_TRICKLE_TCS_MASK = 0xf0;      //!< Trickle charger control register, enable mask
static const uint8_t REG_TRICKLE_TCS_ENABLE = 0xa0;    //!< Trickle charger control register, enable value (0b10100000)
static const uint8_t REG_TRICKLE_DIODE_MASK = 0x0c;    //!< Trickle charger control register, diode mask
static const uint8_t REG_TRICKLE_DIODE_0_6 = 0x08;     //!< Trickle charger control register, diode 0.6V drop
static const uint8_t REG_TRICKLE_DIODE_0_3 = 0x04;     //!< Trickle charger control register, diode 0.3V drop
static const uint8_t REG_TRICKLE_ROUT_MASK = 0x03;     //!< Trickle charger control register, rout mask
static const uint8_t REG_TRICKLE_ROUT_11K = 0x03;      //!< Trickle charger control register, rout 11K
static const uint8_t REG_TRICKLE_ROUT_6K = 0x02;       //!< Trickle charger control register, rout 6K
static const uint8_t REG_TRICKLE_ROUT_3K = 0x01;       //!< Trickle charger control register, rout 3K
static const uint8_t REG_TRICKLE_ROUT_DISABLE = 0x00;  //!< Trickle charger control register, rout disable
static const uint8_t REG_BREF_CTRL = 0x21;             //!< Wakeup control system reference voltages
static const uint8_t REG_BREF_CTRL_DEFAULT = 0xf0;     //!< Wakeup control system default 0b11110000
static const uint8_t REG_BREF_CTRL_25_30 = 0x70;       //!< Wakeup control falling 2.5V rising 3.0V
static const uint8_t REG_BREF_CTRL_21_25 = 0xb0;       //!< Wakeup control falling 2.1V rising 2.5V
static const uint8_t REG_BREF_CTRL_18_22 = 0xd0;       //!< Wakeup control falling 1.8V rising 2.2V
static const uint8_t REG_BREF_CTRL_14_16 = 0xf0;       //!< Wakeup control falling 1.4V rising 1.6V, default value
static const uint8_t REG_AFCTRL = 0x26;                //!< Auto-calibration filter capacitor enable register
static const uint8_t REG_AFCTRL_ENABLE = 0xa0;         //!< Auto-calibration filter capacitor enable
static const uint8_t REG_AFCTRL_DISABLE = 0x00;        //!< Auto-calibration filter capacitor disable
static const uint8_t REG_AFCTRL_DEFAULT = 0x00;        //!< Auto-calibration filter, default
static const uint8_t REG_BATMODE_IO = 0x27;            //!< Brownout control for IO interface
static const uint8_t REG_BATMODE_IO_DEFAULT = 0x80;    //!< Brownout control for IO interface, default value
static const uint8_t REG_BATMODE_IO_IOBM = 0x80;       //!< Brownout control for IO interface, enable IO when on VBAT
static const uint8_t REG_ID0 = 0x28;                   //!< Part number, upper (read-only)
static const uint8_t REG_ID0_AB08XX = 0x18;            //!< Part number, upper, AB08xx
static const uint8_t REG_ID0_AB18XX = 0x18;            //!< Part number, upper, AB18xx
static const uint8_t REG_ID1 = 0x29;                   //!< Part number, lower (read-only)
static const uint8_t REG_ID1_ABXX05 = 0x05;            //!< Part number, lower, AB1805 or AB0805 (I2C)
static const uint8_t REG_ID1_ABXX15 = 0x05;            //!< Part number, lower, AB1815 or AB0815 (SPI)
static const uint8_t REG_ID2 = 0x2a;                   //!< Part revision (read-only)
static const uint8_t REG_ID3 = 0x2b;                   //!< Lot number, lower (read-only)
static const uint8_t REG_ID4 = 0x2c;                   //!< Manufacturing unique ID upper (read-only)
static const uint8_t REG_ID5 = 0x2d;                   //!< Manufacturing unique ID lower (read-only)
static const uint8_t REG_ID6 = 0x2e;                   //!< Lot and wafer information (read-only)
static const uint8_t REG_ASTAT = 0x2f;                 //!< Analog status register (read-only)
static const uint8_t REG_ASTAT_BBOD = 0x80;            //!< Analog status register. VBAT is above BREF (read-only)
static const uint8_t REG_ASTAT_BMIN = 0x40;            //!< Analog status register. VBAT is above minimum operating voltage 1.2V (read-only)
static const uint8_t REG_ASTAT_VINIT = 0x02;           //!< Analog status register. VCC is about minimum 1.6V (read-only)
static const uint8_t REG_OCTRL = 0x30;                 //!< Output control register at power-down
static const uint8_t REG_OCTRL_WDBM = 0x80;            //!< Output control register, WDI enabled when powered from VBAT
static const uint8_t REG_OCTRL_EXBM = 0x40;            //!< Output control register, EXTI enabled when powered from VBAT
static const uint8_t REG_OCTRL_WDDS = 0x20;            //!< Output control register, WDI disabled in sleep
static const uint8_t REG_OCTRL_EXDS = 0x10;            //!< Output control register, EXTI disabled in sleep
static const uint8_t REG_OCTRL_RSEN = 0x08;            //!< Output control register, nRST output enabled in sleep
static const uint8_t REG_OCTRL_O4EN = 0x04;            //!< Output control register, CLKOUT/nIRQ3 enabled in sleep
static const uint8_t REG_OCTRL_O3EN = 0x02;            //!< Output control register, nTIRQ enabled in sleep
static const uint8_t REG_OCTRL_O1EN = 0x01;            //!< Output control register, FOUT/nIRQ enabled in sleep
static const uint8_t REG_OCTRL_DEFAULT = 0x00;         //!< Output control register, default
static const uint8_t REG_EXT_ADDR = 0x3f;              //!< Extension RAM address
static const uint8_t REG_EXT_ADDR_O4MB = 0x80;         //!< Extension RAM address, CLKOUT/nIRQ3 enabled when powered from VBAT
static const uint8_t REG_EXT_ADDR_BPOL = 0x40;         //!< Extension RAM address, BL polarity
static const uint8_t REG_EXT_ADDR_WDIN = 0x20;         //!< Extension RAM address, level of WDI pin (read-only)
static const uint8_t REG_EXT_ADDR_EXIN = 0x10;         //!< Extension RAM address, level of EXTI pin (read-only)
static const uint8_t REG_EXT_ADDR_XADA = 0x04;         //!< Extension RAM address, Upper bit of alternate RAM address
static const uint8_t REG_EXT_ADDR_XADS = 0x03;         //!< Extension RAM address, Upper 2 bits of standard RAM address
static const uint8_t REG_RAM = 0x40;                   //!< Standard RAM
static const uint8_t REG_ALT_RAM = 0x80;               //!< Alternate RAM address
