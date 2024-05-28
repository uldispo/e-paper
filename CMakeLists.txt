
cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_COMPILER "C:/SysGCC/13.2 Rel1/bin/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "C:/SysGCC/13.2 Rel1/bin/arm-none-eabi-g++.exe")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
project(e-paper_18x5 LANGUAGES C ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/main.c
    Core/Src/gpio.c
    Core/Src/adc.c
    Core/Src/i2c.c
    Core/Src/rtc.c
    Core/Src/spi.c
    Core/Src/usart.c
    Core/Src/stm32u0xx_it.c
    Core/Src/stm32u0xx_hal_msp.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_adc.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_adc_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_ll_adc.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rcc.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rcc_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_flash.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_flash_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_gpio.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_dma.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_dma_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_pwr.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_pwr_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_cortex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_exti.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_i2c.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_i2c_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_ll_i2c.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rtc.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_rtc_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_spi.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_spi_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_tim.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_tim_ex.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_uart.c
    Drivers/STM32U0xx_HAL_Driver/Src/stm32u0xx_hal_uart_ex.c
    Core/Src/system_stm32u0xx.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
    startup_stm32u073xx.s
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
    Drivers/STM32U0xx_HAL_Driver/Inc
    Drivers/STM32U0xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Device/ST/STM32U0xx/Include
    Drivers/CMSIS/Include
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Specify linker script
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32U073CCTx_FLASH.ld")
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-T${LINKER_SCRIPT}")
