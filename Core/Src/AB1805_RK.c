

#include "SPI.h"
#include "stdarg.h"
#include "AB1805_RK.h"
#include "printf.h"
#include "stm32u0xx_ll_spi.h"

// inline static uint8_t read_rtc_register(uint8_t reg_addr);
//  inline static uint8_t write_rtc_register(uint8_t rtc_register, uint8_t data);

static inline uint32_t utils_enter_critical_section(void);
static inline void utils_exit_critical_section(uint32_t primask_bit);
inline static uint8_t read_rtc_register(uint8_t regAddr);
inline static uint8_t write_rtc_register(uint8_t regAddr, uint8_t value);

#define AB1815_SPI_READ(offset) (127 & offset)
#define AB1815_SPI_WRITE(offset) (128 | offset)

enum WakeReason wakeReason;

/**
 * @brief Watchdog period in seconds (1 <= watchdogSecs <= 124) or 0 for disabled.
 * This is used so setWDT(-1) can restore the previous value.
 */
int watchdogSecs = 0;

/**
 * @brief The last millis() value where we called setWDT(-1)
 */
uint32_t lastWatchdogMillis = 0;

/**
 * @brief How often to call updateWDT(-1) in milliseconds
 */
uint32_t watchdogUpdatePeriod = 0;

/**
 * @brief True if we've set the RTC from the cloud time
 */
// bool timeSet = false;

static inline void spi_select_slave(bool select)
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

void setup(bool callBegin)
{

    if (detectChip())
    {
        updateWakeReason();
    }
    else
    {
        printf("failed to detect AB1805\n");
    }
}

void loop()
{
    (void)0; //  doing nothing
}

bool detectChip()
{
    bool finalResult = false;
    uint8_t value = 0;

    // FOUT/nIRQ  will go HIGH when the chip is ready to respond

    uint32_t start = HAL_GetTick();
    bool ready = false;
    while (HAL_GetTick() - start < 1000)
    {
        if (HAL_GPIO_ReadPin(NIRQ_GPIO_Port, NIRQ_Pin) == GPIO_PIN_SET) // B12
        {
            ready = true;
            break;
        }
        if (!ready)
        {
            printf("FOUT did not go HIGH\n");
            // May just want to return false here
            return false;
        }
    }

    value = read_rtc_register(REG_ID0); // REG_ID0 = 0x28, the upper RW bit indicating read (if 0) or write (if 1).
    if (value == REG_ID0_AB18XX)
    {
        value = read_rtc_register(REG_ID1);
        if (value == REG_ID1_ABXX15)
        {
            finalResult = true;
        }
    }
    if (!finalResult)
    {
        printf("not detected\n");
    }
    return finalResult;
}

bool usingRCOscillator()
{
    uint8_t value;

    value = read_rtc_register(REG_OSC_STATUS);

    return (value & REG_OSC_STATUS_OMODE) != 0;
}

bool resetConfig(uint32_t flags)
{
    printf("resetConfig(0x%08lx)\n", flags);

    // Reset configuration registers to default values
    write_rtc_register(REG_STATUS, REG_STATUS_DEFAULT);
    write_rtc_register(REG_CTRL_1, REG_CTRL_1_DEFAULT);
    write_rtc_register(REG_CTRL_2, REG_CTRL_2_DEFAULT);
    write_rtc_register(REG_INT_MASK, REG_INT_MASK_DEFAULT);
    write_rtc_register(REG_SQW, REG_SQW_DEFAULT);
    write_rtc_register(REG_SLEEP_CTRL, REG_SLEEP_CTRL_DEFAULT);

    if ((flags & RESET_PRESERVE_REPEATING_TIMER) != 0)
    {
        maskRegister(REG_TIMER_CTRL, ~REG_TIMER_CTRL_RPT_MASK, REG_TIMER_CTRL_DEFAULT & ~REG_TIMER_CTRL_RPT_MASK, false);
    }
    else
    {
        write_rtc_register(REG_TIMER_CTRL, REG_TIMER_CTRL_DEFAULT);
    }

    write_rtc_register(REG_TIMER, REG_TIMER_DEFAULT);
    write_rtc_register(REG_TIMER_INITIAL, REG_TIMER_INITIAL_DEFAULT);
    write_rtc_register(REG_WDT, REG_WDT_DEFAULT);

    uint8_t oscCtrl = REG_OSC_CTRL_DEFAULT;
    if ((flags & RESET_DISABLE_XT) != 0)
    {
        // If disabling XT oscillator, set OSEL to 1 (RC oscillator)
        // Also enable FOS so if the XT oscillator fails, it will switch to RC (just in case)
        // and ACAL to 0 (however REG_OSC_CTRL_DEFAULT already sets ACAL to 0)
        oscCtrl |= REG_OSC_CTRL_OSEL | REG_OSC_CTRL_FOS;
    }
    // write_rtc_register(REG_OSC_CTRL, oscCtrl);
    // write_rtc_register(REG_TRICKLE, REG_TRICKLE_DEFAULT);
    write_rtc_register(REG_BREF_CTRL, REG_BREF_CTRL_DEFAULT);
    write_rtc_register(REG_AFCTRL, REG_AFCTRL_DEFAULT);
    // write_rtc_register(REG_BATMODE_IO, REG_BATMODE_IO_DEFAULT);
    write_rtc_register(REG_OCTRL, REG_OCTRL_DEFAULT);

    return true;
}

bool updateWakeReason()
{
    // static const char *errorMsg = "failure in updateWakeReason %d";

    uint8_t status;
    status = read_rtc_register(REG_STATUS);

    const char *reason = 0;

    if ((status & REG_STATUS_WDT) != 0)
    {
        reason = "WATCHDOG";

        wakeReason = WATCHDOG;
        clearRegisterBit(REG_STATUS, REG_STATUS_WDT, 0);
    }
    else if (isBitSet(REG_SLEEP_CTRL, REG_SLEEP_CTRL_SLST, 0))
    {
        reason = "DEEP_POWER_DOWN";
        wakeReason = DEEP_POWER_DOWN;
    }
    else if ((status & REG_STATUS_TIM) != 0)
    {
        reason = "COUNTDOWN_TIMER";
        wakeReason = COUNTDOWN_TIMER;
        clearRegisterBit(REG_STATUS, REG_STATUS_TIM, 0);
    }
    else if ((status & REG_STATUS_ALM) != 0)
    {
        reason = "ALARM";
        wakeReason = ALARM;
        clearRegisterBit(REG_STATUS, REG_STATUS_ALM, 0);
    }

    if (reason)
    {
        printf("wake reason = %s\n", reason);
    }

    return true;
}

bool setWDT(int seconds)
{
    bool bResult = false;
    // printf("setWDT %d\n", seconds);

    if (seconds < 0)
    {
        seconds = watchdogSecs;
    }

    if (seconds == 0)
    {
        // Disable WDT
        bResult = write_rtc_register(REG_WDT, 0x00);

        watchdogSecs = 0;
        watchdogUpdatePeriod = 0;
    }
    else
    {
        // Use 1/4 Hz clock
        int fourSecs = seconds / 4;
        if (fourSecs < 1)
        {
            fourSecs = 1;
        }
        if (fourSecs > 31)
        {
            fourSecs = 31;
        }
        bResult = write_rtc_register(REG_WDT, REG_WDT_RESET | (fourSecs << 2) | REG_WDT_WRB_1_4_HZ);

        printf("watchdog set fourSecs=%d bResult=%d\n", fourSecs, bResult);

        watchdogSecs = seconds;

        // Update watchdog half way through period
        watchdogUpdatePeriod = (fourSecs * 2000);
    }

    return bResult;
}

bool clearRepeatingInterrupt()
{
    const char *errorMsg = "failure in clearRepeatingInterrupt %d\n";
    bool bResult;

    // Set FOUT/nIRQ control in Control2 to the default value
    bResult = maskRegister(REG_CTRL_2, ~REG_CTRL_2_OUT1S_MASK, REG_CTRL_2_OUT1S_nIRQ, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // Disable alarm interrupt (AIE) in interrupt mask register
    bResult = clearRegisterBit(REG_INT_MASK, REG_INT_MASK_AIE, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // Disable alarm
    bResult = maskRegister(REG_TIMER_CTRL, ~REG_TIMER_CTRL_RPT_MASK, REG_TIMER_CTRL_RPT_DIS, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    return true;
}

bool interruptCountdownTimer(int value, bool minutes) // @param minutes True if minutes, false if seconds
{
    const char *errorMsg = "failure in interruptCountdownTimer %d\n";
    bool bResult;

    // Disable watchdog
    bResult = setWDT(0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // Set FOUT/nIRQ control in OUT1S in Control2 for
    // "nIRQ if at least one interrupt is enabled, else OUT"
    bResult = maskRegister(REG_CTRL_2, ~REG_CTRL_2_OUT1S_MASK, REG_CTRL_2_OUT1S_nIRQ, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    bResult = setCountdownTimer(value, minutes);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    return true;
}

bool deepPowerDown(int seconds)
{
    const char *errorMsg = "failure in deepPowerDown %d\n";
    bool bResult;

    printf("deepPowerDown %d\n", seconds);

    // Disable watchdog
    bResult = setWDT(0);
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
    bResult = maskRegister(REG_CTRL_1, (uint8_t) ~(REG_CTRL_1_STOP | REG_CTRL_1_RSP), REG_CTRL_1_PWR2, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // Disable the I/O interface in sleep
    bResult = setRegisterBit(REG_OSC_CTRL, REG_OSC_CTRL_PWGT, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // OUT2S = 6 to enable sleep mode
    bResult = maskRegister(REG_CTRL_2, (uint8_t)~REG_CTRL_2_OUT2S_MASK, REG_CTRL_2_OUT2S_SLEEP, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }
    hex_dump();
    HAL_Delay(1);
    // Enter sleep mode
    bResult = write_rtc_register(REG_SLEEP_CTRL, REG_SLEEP_CTRL_SLP | REG_SLEEP_CTRL_SLRES); // REG_SLEEP_CTRL_SLP | 0x01
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }
    // bResult = write_rtc_register(REG_TIMER_CTRL, 0xc2); // enable
    // _log.trace("delay in case we didn't power down");
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < (uint32_t)(seconds * 1000))
    {
        printf("REG_SLEEP_CTRL=0x%2x\n", read_rtc_register(REG_SLEEP_CTRL));
        HAL_Delay(1000);
    }

    printf("didn't power down\n");

    return true;
}

bool setCountdownTimer(int value, bool minutes)
{
    const char *errorMsg = "failure in setCountdownTimer %d\n";
    bool bResult;

    // Clear any pending interrupts
    bResult = write_rtc_register(REG_STATUS, REG_STATUS_DEFAULT);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // Stop countdown timer if already running since it can't be set while running
    bResult = write_rtc_register(REG_TIMER_CTRL, REG_TIMER_CTRL_DEFAULT);
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
    bResult = write_rtc_register(REG_TIMER, (uint8_t)value);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // Enable countdown timer interrupt (TIE = 1) in IntMask
    bResult = setRegisterBit(REG_INT_MASK, REG_INT_MASK_TIE, 0);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // Set the TFS frequency to 1/60 Hz for minutes or 1 Hz for seconds
    uint8_t tfs = (minutes ? REG_TIMER_CTRL_TFS_1_60 : REG_TIMER_CTRL_TFS_1);

    // Enable countdown timer (TE = 1) in countdown timer control register
    bResult = write_rtc_register(REG_TIMER_CTRL, REG_TIMER_CTRL_TE | tfs);
    // bResult = write_rtc_register(REG_TIMER_CTRL, 0x42); // 0xc2

    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    return true;
}

bool maskRegister(uint8_t regAddr, uint8_t andValue, uint8_t orValue, bool lock)
{
    bool bResult = false;

    uint8_t value;

    value = read_rtc_register(regAddr);

    uint8_t newValue = (value & andValue) | orValue;

    bResult = write_rtc_register(regAddr, newValue);

    return bResult;
}

bool isBitClear(uint8_t regAddr, uint8_t bitMask, bool lock)
{
    bool bResult;
    uint8_t value = 0;

    bResult = read_rtc_register(regAddr);

    return bResult && ((value & bitMask) == 0);
}

bool isBitSet(uint8_t regAddr, uint8_t bitMask, bool lock)
{
    bool bResult;
    uint8_t value = 0;

    bResult = read_rtc_register(regAddr);

    return bResult && ((value & bitMask) != 0);
}

bool clearRegisterBit(uint8_t regAddr, uint8_t bitMask, bool lock)
{
    return maskRegister(regAddr, ~bitMask, 0x00, lock);
}

bool setRegisterBit(uint8_t regAddr, uint8_t bitMask, bool lock)
{
    return maskRegister(regAddr, 0xff, bitMask, lock);
}

/**
 * @brief Erases the RTC RAM to 0x00
 */
bool eraseRam(bool lock)
{
    bool bResult = true;
    uint8_t array[16];

    memset(array, 0, sizeof(array));
    for (size_t ii = 0; ii < 16; ii++)
    {
        bResult = writeRam(ii * sizeof(array), array, sizeof(array), false);
        if (!bResult)
        {
            printf("erase failed addr=%u\n", ii * sizeof(array));
            break;
        }
    }
    return bResult;
}

bool readRam(size_t ramAddr, uint8_t *data, size_t dataLen, bool lock)
{
    bool bResult = true;

    uint32_t primask_bit = utils_enter_critical_section();

    while (dataLen > 0)
    {
        size_t count = dataLen;
        if (count > 32)
        {
            // Too large for a single I2C operation
            count = 32;
        }
        if ((ramAddr < 128) && ((ramAddr + count) > 128))
        {
            // Crossing a page boundary
            count = 128 - ramAddr;
        }
        if (ramAddr < 128)
        {
            clearRegisterBit(REG_EXT_ADDR, REG_EXT_ADDR_XADA, 0);
        }
        else
        {
            setRegisterBit(REG_EXT_ADDR, REG_EXT_ADDR_XADA, 0);
        }

        bResult = readRegisters(REG_ALT_RAM + (ramAddr & 0x7f), data, count); // !!!!!!!!!
        if (!bResult)
        {
            break;
        }
        ramAddr += count;
        dataLen -= count;
        data += count;
    }

    utils_exit_critical_section(primask_bit);

    return bResult;
}

/**
 * @brief Low-level write call
 * @param ramAddr The address in the RTC RAM to write to
 * @param data The buffer containing the data to write
 * @param dataLen The number of bytes to write
 * The dataLen can be larger than the maximum I2C write. Multiple writes will be done if necessary.
 */
bool writeRam(size_t ramAddr, uint8_t *data, size_t dataLen, bool lock) // The initial values of the RAM locations are undefined.
{
    bool bResult = true;

    while (dataLen > 0)
    {
        size_t count = dataLen;
        if (count > 31)
        {
            // Too large for a single I2C operation
            count = 31;
        }
        if ((ramAddr < 128) && ((ramAddr + count) > 128))
        {
            // Crossing a page boundary
            count = 128 - ramAddr;
        }
        if (ramAddr < 128)
        {
            clearRegisterBit(REG_EXT_ADDR, REG_EXT_ADDR_XADA, 0);
        }
        else
        {
            setRegisterBit(REG_EXT_ADDR, REG_EXT_ADDR_XADA, 0);
        }

        bResult = writeRegisters(REG_ALT_RAM + (ramAddr & 0x7f), data, count);
        if (!bResult)
        {
            break;
        }
        ramAddr += count;
        dataLen -= count;
        data += count;
    }

    return bResult;
}

// [static]
int bcdToValue(uint8_t bcd)
{
    return (bcd >> 4) * 10 + (bcd & 0x0f);
}

// [static]
uint8_t valueToBcd(int value)
{
    int tens = (value / 10) % 10;
    int ones = value % 10;

    return (uint8_t)((tens << 4) | ones);
}

/**
 * @brief Gets the reason the device was reset or woken. For example, TIMER, ALARM, WATCHDOG, etc.
 *
 * This is set during setup() automatically (for HIBERNATE, ULP, and SLEEP_MODE_DEEP) as well as
 * normal reset, however if you are using STOP mode sleep, you must call `updateWakeReason()` after
 * System.sleep() returns to update the wake reason.
 */
enum WakeReason getWakeReason()
{
    return wakeReason;
};

inline static uint8_t SPI1_SendByte(uint8_t data)
{
    uint32_t start_time = HAL_GetTick();
    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET)
    {
        if ((HAL_GetTick() - start_time) > 1000)
        {
            print_error(__func__, __LINE__);
        }
    }

    LL_SPI_TransmitData8(SPI1, data);

    start_time = HAL_GetTick();
    while (LL_SPI_IsActiveFlag_RXNE(SPI1) == RESET)
    {
        if ((HAL_GetTick() - start_time) > 1000)
        {
            print_error(__func__, __LINE__);
        }
    }

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

inline static uint8_t write_rtc_register(uint8_t offset, uint8_t buf)
{
    // uint8_t address = AB1815_SPI_WRITE(offset);
    uint8_t address = offset | 0x80;
    uint32_t primask_bit = utils_enter_critical_section();

    if (!((SPI1)->CR1 & SPI_CR1_SPE))
    {
        SPI1->CR1 |= SPI_CR1_SPE;
    }
    spi_select_slave(0);
    SPI1_SendByte(address);
    SPI1_SendByte(buf); // Send Data to write

    spi_select_slave(1);
    utils_exit_critical_section(primask_bit);
    return 1;
};

void hex_dump(void)
{
    uint8_t buffer[9];
    for (uint8_t pos = 0; pos < 0x7F; pos += 8) // 0x7f
    {

        uint8_t ii = 0;
        for (ii = 0; ii < 7; ii++)
        {
            buffer[ii] = read_rtc_register(pos + ii);
        }
        printf("# 0x%02x: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\r\n", pos, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
    }
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

bool isRTCSet()
{
    return isBitClear(REG_CTRL_1, REG_CTRL_1_WRTC, 0);
};

// #########################################################################

bool readRegisters(uint8_t offset, uint8_t *buf, uint8_t length)
{
    uint8_t address = AB1815_SPI_READ(offset);
    uint32_t primask_bit = utils_enter_critical_section();
    spi_select_slave(0);

    unsigned int i = 0;
    if (!((SPI1)->CR1 & SPI_CR1_SPE))
    {
        SPI1->CR1 |= SPI_CR1_SPE;
    }

    SPI1_SendByte(address);
    while (i < length)
    {
        buf[i++] = SPI1_SendByte(0x00); // Send DUMMY to read data
    }

    spi_select_slave(1);
    utils_exit_critical_section(primask_bit);
    return true;
};

// ##########################################################################
bool writeRegisters(uint8_t offset, uint8_t *buf, uint8_t length)
{
    uint8_t address = AB1815_SPI_WRITE(offset);

    uint32_t primask_bit = utils_enter_critical_section();
    spi_select_slave(0);

    uint8_t i = 0;
    if (!((SPI1)->CR1 & SPI_CR1_SPE))
    {
        SPI1->CR1 |= SPI_CR1_SPE;
    }

    SPI1_SendByte(address);
    while (i < length)
    {
        SPI1_SendByte(buf[i++]); // Send Data to write
    }

    spi_select_slave(1); // set 1
    utils_exit_critical_section(primask_bit);
    return true;
};

/**
 * @brief Stops the watchdog timer. Useful before entering sleep mode.
 *
 * This is done automatically right before reset (using the reset system event)
 * so the watchdog won't trigger during a firmware update.
 */
bool stopWDT()
{
    return setWDT(0);
}

/**
 * @brief Resumes watchdog with same settings as before
 *
 * This is useful after returning from `System.sleep()` in STOP or ULTRA_LOW_POWER
 * sleep modes where executinon continues. You may also want to call `updateWakeReason()`.
 *
 * It's safe to call resumeWDT() even if the WDT has never been set, it does nothing
 * in this case, leaving the watchdog off.
 */
bool resumeWDT()
{
    return setWDT(-1);
}

uint8_t read(uint8_t reg)
{
    return read_rtc_register(reg);
}

uint8_t write(uint8_t reg, uint8_t value)
{
    return write_rtc_register(reg, value);
}

// ################################################################################################

const uint32_t RESET_PRESERVE_REPEATING_TIMER = 0x00000001; //!< When resetting registers, leave repeating timer settings intact
const uint32_t RESET_DISABLE_XT = 0x00000002;               //!< When resetting registers, disable XT oscillator

const int WATCHDOG_MAX_SECONDS = 124; //!< Maximum value that can be passed to setWDT().

const uint8_t REG_HUNDREDTH = 0x00;             //!< Hundredths of a second, 2 BCD digits
const uint8_t REG_SECOND = 0x01;                //!< Seconds, 2 BCD digits, MSB is GP0
const uint8_t REG_MINUTE = 0x02;                //!< Minutes, 2 BCD digits, MSB is GP1
const uint8_t REG_HOUR = 0x03;                  //!< Hours, GP2, GP3
const uint8_t REG_DATE = 0x04;                  //!< Day of month (1-31), 2 BCD digits, GP4, GP5
const uint8_t REG_MONTH = 0x05;                 //!< Month (1-12), 2 BCD digits, GP6 - GP8
const uint8_t REG_YEAR = 0x06;                  //!< Year (0-99), 2 BCD digits
const uint8_t REG_WEEKDAY = 0x07;               //!< Weekday (0-6), GP9 - GP13
const uint8_t REG_HUNDREDTH_ALARM = 0x08;       //!< Alarm on hundredths of a second (0-99), 2 BCD digits
const uint8_t REG_SECOND_ALARM = 0x09;          //!< Alarm on seconds (0-59), 2 BCD digits, GP14
const uint8_t REG_MINUTE_ALARM = 0x0a;          //!< Alarm on minutes (0-59), 2 BCD digits, GP15
const uint8_t REG_HOUR_ALARM = 0x0b;            //!< Alarm on hour, GP16, GP17
const uint8_t REG_DATE_ALARM = 0x0c;            //!< Alarm on date (1-31), 2 BCD digits, GP18-GP19
const uint8_t REG_MONTH_ALARM = 0x0d;           //!< Alarm on month (1-12). 2 BCD digits, GP20-GP22
const uint8_t REG_WEEKDAY_ALARM = 0x0e;         //!< Alarm on day of week (0-6). GP23-GP27
const uint8_t REG_STATUS = 0x0f;                //!< Status register
const uint8_t REG_STATUS_CB = 0x80;             //!< Status register century bit mask
const uint8_t REG_STATUS_BAT = 0x40;            //!< Status register switched to VBAT bit mask
const uint8_t REG_STATUS_WDT = 0x20;            //!< Status register watchdog timer enabled and triggered bit mask
const uint8_t REG_STATUS_BL = 0x10;             //!< Status register battery voltage crossing bit mask
const uint8_t REG_STATUS_TIM = 0x08;            //!< Status register countdown timer reaches 0 bit mask
const uint8_t REG_STATUS_ALM = 0x04;            //!< Status register alarm register match bit mask
const uint8_t REG_STATUS_EX2 = 0x02;            //!< Status register WDI interrupt bit mask
const uint8_t REG_STATUS_EX1 = 0x01;            //!< Status register EXTI interrupt bit mask
const uint8_t REG_STATUS_DEFAULT = 0x00;        //!< Status register, default
const uint8_t REG_CTRL_1 = 0x10;                //!< Control register 1
const uint8_t REG_CTRL_1_STOP = 0x80;           //!< Control register 1, stop clocking system
const uint8_t REG_CTRL_1_12_24 = 0x40;          //!< Control register 1, 12/24 hour mode select (0 = 24 hour)
const uint8_t REG_CTRL_1_OUTB = 0x20;           //!< Control register 1, value for nIRQ2
const uint8_t REG_CTRL_1_OUT = 0x10;            //!< Control register 1, value for FOUT/nIRQ
const uint8_t REG_CTRL_1_RSP = 0x08;            //!< Control register 1, Reset polarity
const uint8_t REG_CTRL_1_ARST = 0x04;           //!< Control register 1, Auto reset enable
const uint8_t REG_CTRL_1_PWR2 = 0x02;           //!< Control register 1, PWW/nIRQ pull-down enable
const uint8_t REG_CTRL_1_WRTC = 0x01;           //!< Control register 1, write RTC mode
const uint8_t REG_CTRL_1_DEFAULT = 0x12;        //!< Control register 1, 0b00010011 (OUT | RSO | PWR2 | WRTC) //!!0x12 not WRTC
const uint8_t REG_CTRL_2 = 0x11;                //!< Control register 2
const uint8_t REG_CTRL_2_RS1E = 0x20;           //!< Control register 2, nIRQ2 output mode
const uint8_t REG_CTRL_2_OUT2S_MASK = 0x1c;     //!< Control register 2, nIRQ2 output mode
const uint8_t REG_CTRL_2_OUT2S_nIRQ = 0x00;     //!< Control register 2, nIRQ2 output mode, nIRQ or OUTB
const uint8_t REG_CTRL_2_OUT2S_SQW = 0x04;      //!< Control register 2, nIRQ2 output mode, SQW or OUTB
const uint8_t REG_CTRL_2_OUT2S_nAIRQ = 0x0c;    //!< Control register 2, nIRQ2 output mode, nAIRQ or OUTB
const uint8_t REG_CTRL_2_OUT2S_TIRQ = 0x10;     //!< Control register 2, nIRQ2 output mode, TIRQ or OUTB
const uint8_t REG_CTRL_2_OUT2S_nTIRQ = 0x14;    //!< Control register 2, nIRQ2 output mode, nTIRQ or OUTB
const uint8_t REG_CTRL_2_OUT2S_SLEEP = 0x18;    //!< Control register 2, nIRQ2 output mode, sleep mode
const uint8_t REG_CTRL_2_OUT2S_OUTB = 0x1c;     //!< Control register 2, nIRQ2 output mode, OUTB
const uint8_t REG_CTRL_2_OUT1S_MASK = 0x03;     //!< Control register 2, FOUT/nIRQ output mode
const uint8_t REG_CTRL_2_OUT1S_nIRQ = 0x00;     //!< Control register 2, FOUT/nIRQ output mode, nIRQ, or OUT
const uint8_t REG_CTRL_2_OUT1S_SQW = 0x01;      //!< Control register 2, FOUT/nIRQ output mode, SQW or OUT
const uint8_t REG_CTRL_2_OUT1S_SQW_nIRQ = 0x02; //!< Control register 2, FOUT/nIRQ output mode, SQW, nIRQ, or OUT
const uint8_t REG_CTRL_2_OUT1S_nAIRQ = 0x03;    //!< Control register 2, FOUT/nIRQ output mode, nIRQ or OUT
const uint8_t REG_CTRL_2_DEFAULT = 0x3c;        //!< Control register 2, 0b00111100 (OUT2S = OUTB)
const uint8_t REG_INT_MASK = 0x12;              //!< Interrupt mask
const uint8_t REG_INT_MASK_CEB = 0x80;          //!< Interrupt mask, century enable
const uint8_t REG_INT_MASK_IM = 0x60;           //!< Interrupt mask, interrupt mode bits (2 bits)
const uint8_t REG_INT_MASK_BLIE = 0x10;         //!< Interrupt mask, battery low interrupt enable
const uint8_t REG_INT_MASK_TIE = 0x08;          //!< Interrupt mask, timer interrupt enable
const uint8_t REG_INT_MASK_AIE = 0x04;          //!< Interrupt mask, alarm interrupt enable
const uint8_t REG_INT_MASK_EX2E = 0x02;         //!< Interrupt mask, XT2 interrupt enable
const uint8_t REG_INT_MASK_EX1E = 0x01;         //!< Interrupt mask, XT1 interrupt enable
const uint8_t REG_INT_MASK_DEFAULT = 0xe0;      //!< Interrupt mask, default 0b11100000 (CEB | IM=1/4 seconds)
const uint8_t REG_SQW = 0x13;                   //!< Square wave output control
const uint8_t REG_SQW_SQWE = 0x80;              //!< Square wave output control, enable
const uint8_t REG_SQW_DEFAULT = 0x26;           //!< Square wave output control, default 0b00100110
const uint8_t REG_CAL_XT = 0x14;                //!< Calibration for the XT oscillator
const uint8_t REG_CAL_RC_HIGH = 0x15;           //!< Calibration for the RC oscillator, upper 8 bits
const uint8_t REG_CAL_RC_LOW = 0x16;            //!< Calibration for the RC oscillator, lower 8 bits
const uint8_t REG_SLEEP_CTRL = 0x17;            //!< Power control system sleep function
const uint8_t REG_SLEEP_CTRL_SLP = 0x80;        //!< Sleep control, enter sleep mode
const uint8_t REG_SLEEP_CTRL_SLRES = 0x40;      //!< Sleep control, nRST low on sleep
const uint8_t REG_SLEEP_CTRL_EX2P = 0x20;       //!< Sleep control, XT2 on rising WDI
const uint8_t REG_SLEEP_CTRL_EX1P = 0x10;       //!< Sleep control, XT1 on rising EXTI
const uint8_t REG_SLEEP_CTRL_SLST = 0x08;       //!< Sleep control, set when sleep has occurred
const uint8_t REG_SLEEP_CTRL_SLTO_MASK = 0x07;  //!< Sleep control, number of 7.8ms periods before sleep
const uint8_t REG_SLEEP_CTRL_DEFAULT = 0x00;    //!< Sleep control default (0b00000000)
const uint8_t REG_TIMER_CTRL = 0x18;            //!< Countdown timer control
const uint8_t REG_TIMER_CTRL_TE = 0x80;         //!< Countdown timer control, timer enable
const uint8_t REG_TIMER_CTRL_TM = 0x40;         //!< Countdown timer control, timer interrupt mode
const uint8_t REG_TIMER_CTRL_TRPT = 0x20;       //!< Countdown timer control, timer repeat function
const uint8_t REG_TIMER_CTRL_RPT_MASK = 0x1c;   //!< Countdown timer control, repeat function
const uint8_t REG_TIMER_CTRL_RPT_HUN = 0x1c;    //!< Countdown timer control, repeat hundredths match (7)
const uint8_t REG_TIMER_CTRL_RPT_SEC = 0x18;    //!< Countdown timer control, repeat hundredths, seconds match (once per minute) (6)
const uint8_t REG_TIMER_CTRL_RPT_MIN = 0x14;    //!< Countdown timer control, repeat hundredths, seconds, minutes match (once per hour) (5)
const uint8_t REG_TIMER_CTRL_RPT_HOUR = 0x10;   //!< Countdown timer control, repeat hundredths, seconds, minutes, hours match (once per day) (4)
const uint8_t REG_TIMER_CTRL_RPT_WKDY = 0x0c;   //!< Countdown timer control, repeat hundredths, seconds, minutes, hours, weekday match (once per week) (3)
const uint8_t REG_TIMER_CTRL_RPT_DATE = 0x08;   //!< Countdown timer control, repeat hundredths, seconds, minutes, hours, date match (once per month) (2)
const uint8_t REG_TIMER_CTRL_RPT_MON = 0x04;    //!< Countdown timer control, repeat hundredths, seconds, minutes, hours, date, month match (once per year) (1)
const uint8_t REG_TIMER_CTRL_RPT_DIS = 0x00;    //!< Countdown timer control, alarm disabled (0)
const uint8_t REG_TIMER_CTRL_TFS_MASK = 0x03;   //!< Countdown timer control, clock frequency
const uint8_t REG_TIMER_CTRL_TFS_FAST = 0x00;   //!< Countdown timer control, clock frequency 4.096 kHz or 128 Hz
const uint8_t REG_TIMER_CTRL_TFS_64 = 0x01;     //!< Countdown timer control, clock frequency 64 Hz
const uint8_t REG_TIMER_CTRL_TFS_1 = 0x02;      //!< Countdown timer control, clock frequency 1 Hz
const uint8_t REG_TIMER_CTRL_TFS_1_60 = 0x03;   //!< Countdown timer control, clock frequency 1/60 Hz (1 minute)
const uint8_t REG_TIMER_CTRL_DEFAULT = 0x23;    //!< Countdown timer control, 0b00100011 (TFPT + TFS = 1/60 Hz0)
const uint8_t REG_TIMER = 0x19;                 //!< Countdown timer current value register
const uint8_t REG_TIMER_DEFAULT = 0x00;         //!< Countdown timer current value register default value (0x00)
const uint8_t REG_TIMER_INITIAL = 0x1a;         //!< Countdown timer inital (reload) value register
const uint8_t REG_TIMER_INITIAL_DEFAULT = 0x00; //!< Countdown timer inital value register default value
const uint8_t REG_WDT = 0x1b;                   //!< Watchdog timer control register
const uint8_t REG_WDT_RESET = 0x80;             //!< Watchdog timer control, enable reset (1) or WIRQ (0)
const uint8_t REG_WDT_WRB_16_HZ = 0x00;         //!< Watchdog timer control, WRB watchdog clock = 16 Hz
const uint8_t REG_WDT_WRB_4_HZ = 0x01;          //!< Watchdog timer control, WRB watchdog clock = 4 Hz
const uint8_t REG_WDT_WRB_1_HZ = 0x02;          //!< Watchdog timer control, WRB watchdog clock = 1 Hz
const uint8_t REG_WDT_WRB_1_4_HZ = 0x03;        //!< Watchdog timer control, WRB watchdog clock = 1/4 Hz
const uint8_t REG_WDT_DEFAULT = 0x00;           //!< Watchdog timer control, default value
const uint8_t REG_OSC_CTRL = 0x1c;              //!< Oscillator control register
const uint8_t REG_OSC_CTRL_OSEL = 0x80;         //!< Oscillator control, clock select 32.768 kHz (0) or 128 Hz (1)
const uint8_t REG_OSC_CTRL_ACAL = 0x60;         //!< Oscillator control, auto-calibration
const uint8_t REG_OSC_CTRL_AOS = 0x10;          //!< Oscillator control, automatic switch to RC oscillator on battery
const uint8_t REG_OSC_CTRL_FOS = 0x08;          //!< Oscillator control, automatic switch to RC oscillator on failure
const uint8_t REG_OSC_CTRL_PWGT = 0x04;         //!< Oscillator control, IO interface disable
const uint8_t REG_OSC_CTRL_OFIE = 0x02;         //!< Oscillator control, oscillator fail interrupt enable
const uint8_t REG_OSC_CTRL_ACIE = 0x01;         //!< Oscillator control, auto-calibration fail interrupt enable
const uint8_t REG_OSC_CTRL_DEFAULT = 0x00;      //!< Oscillator control, default value
const uint8_t REG_OSC_STATUS = 0x1d;            //!< Oscillator status register
const uint8_t REG_OSC_STATUS_XTCAL = 0x0c;      //!< Oscillator status register, extended crystal calibration
const uint8_t REG_OSC_STATUS_LKO2 = 0x04;       //!< Oscillator status register, lock OUT2
const uint8_t REG_OSC_STATUS_OMODE = 0x01;      //!< Oscillator status register, oscillator mode (read-only)
const uint8_t REG_OSC_STATUS_OF = 0x02;         //!< Oscillator status register, oscillator failure
const uint8_t REG_OSC_STATUS_ACF = 0x01;        //!< Oscillator status register, auto-calibration failure
const uint8_t REG_CONFIG_KEY = 0x1f;            //!< Register to set to modify certain other keys
const uint8_t REG_CONFIG_KEY_OSC_CTRL = 0xa1;   //!< Configuration key, enable setting REG_OSC_CTRL
const uint8_t REG_CONFIG_KEY_SW_RESET = 0x3c;   //!< Configuration key, software reset
const uint8_t REG_CONFIG_KEY_OTHER = 0x9d;      //!< Configuration key, REG_TRICKLE, REG_BREF_CTRL, REG_AFCTRL, REG_BATMODE_IO, REG_OCTRL
const uint8_t REG_TRICKLE = 0x20;               //!< Trickle charger control register
const uint8_t REG_TRICKLE_DEFAULT = 0x00;       //!< Trickle charger control register, default value
const uint8_t REG_TRICKLE_TCS_MASK = 0xf0;      //!< Trickle charger control register, enable mask
const uint8_t REG_TRICKLE_TCS_ENABLE = 0xa0;    //!< Trickle charger control register, enable value (0b10100000)
const uint8_t REG_TRICKLE_DIODE_MASK = 0x0c;    //!< Trickle charger control register, diode mask
const uint8_t REG_TRICKLE_DIODE_0_6 = 0x08;     //!< Trickle charger control register, diode 0.6V drop
const uint8_t REG_TRICKLE_DIODE_0_3 = 0x04;     //!< Trickle charger control register, diode 0.3V drop
const uint8_t REG_TRICKLE_ROUT_MASK = 0x03;     //!< Trickle charger control register, rout mask
const uint8_t REG_TRICKLE_ROUT_11K = 0x03;      //!< Trickle charger control register, rout 11K
const uint8_t REG_TRICKLE_ROUT_6K = 0x02;       //!< Trickle charger control register, rout 6K
const uint8_t REG_TRICKLE_ROUT_3K = 0x01;       //!< Trickle charger control register, rout 3K
const uint8_t REG_TRICKLE_ROUT_DISABLE = 0x00;  //!< Trickle charger control register, rout disable
const uint8_t REG_BREF_CTRL = 0x21;             //!< Wakeup control system reference voltages
const uint8_t REG_BREF_CTRL_DEFAULT = 0xf0;     //!< Wakeup control system default 0b11110000
const uint8_t REG_BREF_CTRL_25_30 = 0x70;       //!< Wakeup control falling 2.5V rising 3.0V
const uint8_t REG_BREF_CTRL_21_25 = 0xb0;       //!< Wakeup control falling 2.1V rising 2.5V
const uint8_t REG_BREF_CTRL_18_22 = 0xd0;       //!< Wakeup control falling 1.8V rising 2.2V
const uint8_t REG_BREF_CTRL_14_16 = 0xf0;       //!< Wakeup control falling 1.4V rising 1.6V, default value
const uint8_t REG_AFCTRL = 0x26;                //!< Auto-calibration filter capacitor enable register
const uint8_t REG_AFCTRL_ENABLE = 0xa0;         //!< Auto-calibration filter capacitor enable
const uint8_t REG_AFCTRL_DISABLE = 0x00;        //!< Auto-calibration filter capacitor disable
const uint8_t REG_AFCTRL_DEFAULT = 0x00;        //!< Auto-calibration filter, default
const uint8_t REG_BATMODE_IO = 0x27;            //!< Brownout control for IO interface
const uint8_t REG_BATMODE_IO_DEFAULT = 0x80;    //!< Brownout control for IO interface, default value
const uint8_t REG_BATMODE_IO_IOBM = 0x80;       //!< Brownout control for IO interface, enable IO when on VBAT
const uint8_t REG_ID0 = 0x28;                   //!< Part number, upper (read-only)
const uint8_t REG_ID0_AB08XX = 0x18;            //!< Part number, upper, AB08xx
const uint8_t REG_ID0_AB18XX = 0x18;            //!< Part number, upper, AB18xx
const uint8_t REG_ID1 = 0x29;                   //!< Part number, lower (read-only)
const uint8_t REG_ID1_ABXX05 = 0x05;            //!< Part number, lower, AB1805 or AB0805 (I2C)
const uint8_t REG_ID1_ABXX15 = 0x05;            //!< Part number, lower, AB1815 or AB0815 (SPI)
const uint8_t REG_ID2 = 0x2a;                   //!< Part revision (read-only)
const uint8_t REG_ID3 = 0x2b;                   //!< Lot number, lower (read-only)
const uint8_t REG_ID4 = 0x2c;                   //!< Manufacturing unique ID upper (read-only)
const uint8_t REG_ID5 = 0x2d;                   //!< Manufacturing unique ID lower (read-only)
const uint8_t REG_ID6 = 0x2e;                   //!< Lot and wafer information (read-only)
const uint8_t REG_ASTAT = 0x2f;                 //!< Analog status register (read-only)
const uint8_t REG_ASTAT_BBOD = 0x80;            //!< Analog status register. VBAT is above BREF (read-only)
const uint8_t REG_ASTAT_BMIN = 0x40;            //!< Analog status register. VBAT is above minimum operating voltage 1.2V (read-only)
const uint8_t REG_ASTAT_VINIT = 0x02;           //!< Analog status register. VCC is about minimum 1.6V (read-only)
const uint8_t REG_OCTRL = 0x30;                 //!< Output control register at power-down
const uint8_t REG_OCTRL_WDBM = 0x80;            //!< Output control register, WDI enabled when powered from VBAT
const uint8_t REG_OCTRL_EXBM = 0x40;            //!< Output control register, EXTI enabled when powered from VBAT
const uint8_t REG_OCTRL_WDDS = 0x20;            //!< Output control register, WDI disabled in sleep
const uint8_t REG_OCTRL_EXDS = 0x10;            //!< Output control register, EXTI disabled in sleep
const uint8_t REG_OCTRL_RSEN = 0x08;            //!< Output control register, nRST output enabled in sleep
const uint8_t REG_OCTRL_O4EN = 0x04;            //!< Output control register, CLKOUT/nIRQ3 enabled in sleep
const uint8_t REG_OCTRL_O3EN = 0x02;            //!< Output control register, nTIRQ enabled in sleep
const uint8_t REG_OCTRL_O1EN = 0x01;            //!< Output control register, FOUT/nIRQ enabled in sleep
const uint8_t REG_OCTRL_DEFAULT = 0x00;         //!< Output control register, default
const uint8_t REG_EXT_ADDR = 0x3f;              //!< Extension RAM address
const uint8_t REG_EXT_ADDR_O4MB = 0x80;         //!< Extension RAM address, CLKOUT/nIRQ3 enabled when powered from VBAT
const uint8_t REG_EXT_ADDR_BPOL = 0x40;         //!< Extension RAM address, BL polarity
const uint8_t REG_EXT_ADDR_WDIN = 0x20;         //!< Extension RAM address, level of WDI pin (read-only)
const uint8_t REG_EXT_ADDR_EXIN = 0x10;         //!< Extension RAM address, level of EXTI pin (read-only)
const uint8_t REG_EXT_ADDR_XADA = 0x04;         //!< Extension RAM address, Upper bit of alternate RAM address
const uint8_t REG_EXT_ADDR_XADS = 0x03;         //!< Extension RAM address, Upper 2 bits of standard RAM address
const uint8_t REG_RAM = 0x40;                   //!< Standard RAM
const uint8_t REG_ALT_RAM = 0x80;               //!< Alternate RAM address
