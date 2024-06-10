################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/system_stm32u0xx.c 

OBJS += \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/system_stm32u0xx.o 

C_DEPS += \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/system_stm32u0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/%.o Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/%.su Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/%.cyclo: ../Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/%.c Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U073xx -c -I../Core/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-STM32U0xx_HAL_Driver-2f-CMSIS-2f-Device-2f-ST-2f-STM32U0xx-2f-Source-2f-Templates

clean-Drivers-2f-STM32U0xx_HAL_Driver-2f-CMSIS-2f-Device-2f-ST-2f-STM32U0xx-2f-Source-2f-Templates:
	-$(RM) ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/system_stm32u0xx.cyclo ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/system_stm32u0xx.d ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/system_stm32u0xx.o ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/system_stm32u0xx.su

.PHONY: clean-Drivers-2f-STM32U0xx_HAL_Driver-2f-CMSIS-2f-Device-2f-ST-2f-STM32U0xx-2f-Source-2f-Templates

