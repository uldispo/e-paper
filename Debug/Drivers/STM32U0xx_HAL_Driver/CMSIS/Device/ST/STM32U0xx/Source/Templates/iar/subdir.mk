################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u031xx.s \
../Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u073xx.s \
../Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u083xx.s 

OBJS += \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u031xx.o \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u073xx.o \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u083xx.o 

S_DEPS += \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u031xx.d \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u073xx.d \
./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u083xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/%.o: ../Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/%.s Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0plus -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Drivers-2f-STM32U0xx_HAL_Driver-2f-CMSIS-2f-Device-2f-ST-2f-STM32U0xx-2f-Source-2f-Templates-2f-iar

clean-Drivers-2f-STM32U0xx_HAL_Driver-2f-CMSIS-2f-Device-2f-ST-2f-STM32U0xx-2f-Source-2f-Templates-2f-iar:
	-$(RM) ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u031xx.d ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u031xx.o ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u073xx.d ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u073xx.o ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u083xx.d ./Drivers/STM32U0xx_HAL_Driver/CMSIS/Device/ST/STM32U0xx/Source/Templates/iar/startup_stm32u083xx.o

.PHONY: clean-Drivers-2f-STM32U0xx_HAL_Driver-2f-CMSIS-2f-Device-2f-ST-2f-STM32U0xx-2f-Source-2f-Templates-2f-iar

