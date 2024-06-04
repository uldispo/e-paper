

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

enum WakeReason wakeReason;

static inline void spi_select_slave(bool select)
{
    if (select)
    {
        RTC_L();
    }
    else
    {
        RTC_H();
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
        printf("failed to detect AB1805");
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
    printf("resetConfig(0x%08lx)", flags);

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
    write_rtc_register(REG_OSC_CTRL, oscCtrl);
    write_rtc_register(REG_TRICKLE, REG_TRICKLE_DEFAULT);
    write_rtc_register(REG_BREF_CTRL, REG_BREF_CTRL_DEFAULT);
    write_rtc_register(REG_AFCTRL, REG_AFCTRL_DEFAULT);
    write_rtc_register(REG_BATMODE_IO, REG_BATMODE_IO_DEFAULT);
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
        printf("wake reason = %s", reason);
    }

    return true;
}

bool setWDT(int seconds)
{
    bool bResult = false;
    printf("setWDT %d", seconds);

    if (seconds < 0)
    {
        seconds = watchdogSecs;
    }

    if (seconds == 0)
    {
        // Disable WDT
        bResult = write_rtc_register(REG_WDT, 0x00);

        printf("watchdog cleared bResult=%d", bResult);

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

        printf("watchdog set fourSecs=%d bResult=%d", fourSecs, bResult);

        watchdogSecs = seconds;

        // Update watchdog half way through period
        watchdogUpdatePeriod = (fourSecs * 2000);
    }

    return bResult;
}

bool clearRepeatingInterrupt()
{
    static const char *errorMsg = "failure in clearRepeatingInterrupt %d";
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
    static const char *errorMsg = "failure in interruptCountdownTimer %d";
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
    static const char *errorMsg = "failure in deepPowerDown %d";
    bool bResult;

    printf("deepPowerDown %d", seconds);

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

    // Enter sleep mode and set nRST low
    bResult = write_rtc_register(REG_SLEEP_CTRL, REG_SLEEP_CTRL_SLP | REG_SLEEP_CTRL_SLRES);
    if (!bResult)
    {
        printf(errorMsg, __LINE__);
        return false;
    }

    // _log.trace("delay in case we didn't power down");
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < (uint32_t)(seconds * 1000))
    {
        printf("REG_SLEEP_CTRL=0x%2x", read_rtc_register(REG_SLEEP_CTRL));
        HAL_Delay(1000);
    }

    printf("didn't power down");

    return true;
}

bool setCountdownTimer(int value, bool minutes)
{
    static const char *errorMsg = "failure in setCountdownTimer %d";
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
            printf("erase failed addr=%u", ii * sizeof(array));
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
    uint8_t address = AB1815_SPI_WRITE(offset);
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
