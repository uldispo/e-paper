################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/AB1815.c \
../Core/Src/adc.c \
../Core/Src/adc_if.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/main.c \
../Core/Src/moving_average.c \
../Core/Src/printf.c \
../Core/Src/rtc.c \
../Core/Src/spi.c \
../Core/Src/stm32u0xx_hal_msp.c \
../Core/Src/stm32u0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32u0xx.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/AB1815.o \
./Core/Src/adc.o \
./Core/Src/adc_if.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/main.o \
./Core/Src/moving_average.o \
./Core/Src/printf.o \
./Core/Src/rtc.o \
./Core/Src/spi.o \
./Core/Src/stm32u0xx_hal_msp.o \
./Core/Src/stm32u0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32u0xx.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/AB1815.d \
./Core/Src/adc.d \
./Core/Src/adc_if.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/main.d \
./Core/Src/moving_average.d \
./Core/Src/printf.d \
./Core/Src/rtc.d \
./Core/Src/spi.d \
./Core/Src/stm32u0xx_hal_msp.d \
./Core/Src/stm32u0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32u0xx.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U073xx -c -I../Core/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/AB1815.cyclo ./Core/Src/AB1815.d ./Core/Src/AB1815.o ./Core/Src/AB1815.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/adc_if.cyclo ./Core/Src/adc_if.d ./Core/Src/adc_if.o ./Core/Src/adc_if.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/moving_average.cyclo ./Core/Src/moving_average.d ./Core/Src/moving_average.o ./Core/Src/moving_average.su ./Core/Src/printf.cyclo ./Core/Src/printf.d ./Core/Src/printf.o ./Core/Src/printf.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32u0xx_hal_msp.cyclo ./Core/Src/stm32u0xx_hal_msp.d ./Core/Src/stm32u0xx_hal_msp.o ./Core/Src/stm32u0xx_hal_msp.su ./Core/Src/stm32u0xx_it.cyclo ./Core/Src/stm32u0xx_it.d ./Core/Src/stm32u0xx_it.o ./Core/Src/stm32u0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32u0xx.cyclo ./Core/Src/system_stm32u0xx.d ./Core/Src/system_stm32u0xx.o ./Core/Src/system_stm32u0xx.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

