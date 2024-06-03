################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Flash/flash.c 

C_DEPS += \
./Flash/flash.d 

OBJS += \
./Flash/flash.o 


# Each subdirectory must supply rules for building sources it contributes
Flash/%.o Flash/%.su Flash/%.cyclo: ../Flash/%.c Flash/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/Admin/Documents/ARM/ALARM SYSTEM/encoder" -I"C:/Users/Admin/Documents/ARM/ALARM SYSTEM/common" -I"C:/Users/Admin/Documents/ARM/ALARM SYSTEM/ST7920_SERIAL" -I"C:/Users/Admin/Documents/ARM/ALARM SYSTEM/BA_BITMAP" -I"C:/Users/Admin/Documents/ARM/ALARM SYSTEM/Flash" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Flash

clean-Flash:
	-$(RM) ./Flash/flash.cyclo ./Flash/flash.d ./Flash/flash.o ./Flash/flash.su

.PHONY: clean-Flash

