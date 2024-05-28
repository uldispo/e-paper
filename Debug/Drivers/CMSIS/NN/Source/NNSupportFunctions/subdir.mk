################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_kernel_s16.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.c \
../Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.c 

OBJS += \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_kernel_s16.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.o \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.o 

C_DEPS += \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_kernel_s16.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.d \
./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/NN/Source/NNSupportFunctions/%.o Drivers/CMSIS/NN/Source/NNSupportFunctions/%.su Drivers/CMSIS/NN/Source/NNSupportFunctions/%.cyclo: ../Drivers/CMSIS/NN/Source/NNSupportFunctions/%.c Drivers/CMSIS/NN/Source/NNSupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U073xx -c -I../Core/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc -I../Drivers/STM32U0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS-2f-NN-2f-Source-2f-NNSupportFunctions

clean-Drivers-2f-CMSIS-2f-NN-2f-Source-2f-NNSupportFunctions:
	-$(RM) ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_kernel_s16.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_kernel_s16.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_kernel_s16.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mul_kernel_s16.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.su ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.cyclo ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.d ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.o ./Drivers/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.su

.PHONY: clean-Drivers-2f-CMSIS-2f-NN-2f-Source-2f-NNSupportFunctions

