/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    AB1815.c
 * @brief   AB1815 drivers
 ******************************************************************************
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "main.h"
#include "AB1815.h"
#include <stdbool.h>

extern I2C_HandleTypeDef hi2c1;
// uint32_t primask_bit;
static inline uint32_t utils_enter_critical_section(void);
static inline void utils_exit_critical_section(uint32_t primask_bit);
uint16_t foutPin = 0;
uint8_t i2cAddr = 0xD2;


bool detectChip()
{
    bool bResult, finalResult = false;
    uint8_t value = 0;

    // FOUT/nIRQ  will go HIGH when the chip is ready to respond
    if (foutPin != 0)
    {
        unsigned long start = HAL_GetTick();
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
            }
        }
    }

    bResult = readRegister(REG_ID0, value, 1);
    if (bResult && value == REG_ID0_AB18XX)
    {
        bResult = readRegister(REG_ID1, value, 1);
        if (bResult && value == REG_ID1_ABXX05)
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

bool readRegister(uint8_t regAddr, uint8_t value, bool lock)
{
    return readRegisters(regAddr, &value, 1, lock);
}

bool readRegisters(uint8_t regAddr, uint8_t *array, uint8_t num, bool lock)
{
    bool bResult = false;
    uint32_t primask_bit;

    if (lock)
    {
    	 primask_bit = utils_enter_critical_section();
    }

    // write(regAddr);
    // HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t *)aTxBuffer, TXBUFFERSIZE, 10000)
    while (HAL_I2C_Master_Transmit(&hi2c1, i2cAddr, &regAddr, 1, 3000) != HAL_OK)
    {
        /* Error_Handler() function is called when Timeout error occurs.
        When Acknowledge failure occurs (Slave don't acknowledge its address)
        Master restarts communication */
        if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_NONE) // HAL_I2C_ERROR_NONE
        {
            Error_Handler();
        }
    }

    while (HAL_I2C_Master_Receive(&hi2c1, i2cAddr, array, num, 3000) != HAL_OK)
    {
        if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_NONE) // ??? HAL_I2C_ERROR_NONE
        {
            Error_Handler();
        }
    }
    bResult = true;

    if (lock)
    {
    	utils_exit_critical_section(primask_bit);
    }
    return bResult;
}

uint8_t readRegister_value(uint8_t regAddr, bool lock)
{
    uint8_t value = 0;

    (void)readRegister(regAddr, value, lock);

    return value;
}

bool writeRegister(uint8_t regAddr, uint8_t value, bool lock)
{
    return writeRegisters(regAddr, &value, 1, lock);
}

bool writeRegisters(uint8_t regAddr, uint8_t *array, size_t num, bool lock)
{
    bool bResult = false;
    uint32_t primask_bit;

    if (lock)
    {
    	primask_bit = utils_enter_critical_section();
    }

    // wire.beginTransmission(i2cAddr);
    // wire.write(regAddr);
    while (HAL_I2C_Master_Transmit(&hi2c1, i2cAddr, &regAddr, 1, 3000) != HAL_OK)
    {
        if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_NONE) // HAL_I2C_ERROR_NONE
        {
            Error_Handler();
        }
    }

    while (HAL_I2C_Master_Transmit(&hi2c1, i2cAddr, array, num, 3000) != HAL_OK)
    {
        if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_NONE) // HAL_I2C_ERROR_NONE
        {
            Error_Handler();
        }
    }

    if (lock)
    {
    	utils_exit_critical_section(primask_bit);
    }
    return bResult;
}

bool maskRegister(uint8_t regAddr, uint8_t andValue, uint8_t orValue, bool lock)
{
    bool bResult = false;
    uint32_t primask_bit;
    if (lock)
    {
    	primask_bit = utils_enter_critical_section();
    }

    uint8_t value = 0;

    bResult = readRegister(regAddr, value, 1);
    if (bResult)
    {
        uint8_t newValue = (value & andValue) | orValue;

        if (newValue != value)
        {
            bResult = writeRegister(regAddr, newValue, 1);
        }
    }

    if (lock)
    {
    	utils_exit_critical_section(primask_bit);
    }
    return bResult;
}

// ************************ critical *****************************************

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



