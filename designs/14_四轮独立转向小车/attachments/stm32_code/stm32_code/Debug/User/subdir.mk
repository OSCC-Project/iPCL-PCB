################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/data_processing.c \
../User/gear_motor.c \
../User/imu.c \
../User/in_flash.c \
../User/motion.c \
../User/nrf24l01.c \
../User/servo.c 

OBJS += \
./User/data_processing.o \
./User/gear_motor.o \
./User/imu.o \
./User/in_flash.o \
./User/motion.o \
./User/nrf24l01.o \
./User/servo.o 

C_DEPS += \
./User/data_processing.d \
./User/gear_motor.d \
./User/imu.d \
./User/in_flash.d \
./User/motion.d \
./User/nrf24l01.d \
./User/servo.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o User/%.su User/%.cyclo: ../User/%.c User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../User -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User

clean-User:
	-$(RM) ./User/data_processing.cyclo ./User/data_processing.d ./User/data_processing.o ./User/data_processing.su ./User/gear_motor.cyclo ./User/gear_motor.d ./User/gear_motor.o ./User/gear_motor.su ./User/imu.cyclo ./User/imu.d ./User/imu.o ./User/imu.su ./User/in_flash.cyclo ./User/in_flash.d ./User/in_flash.o ./User/in_flash.su ./User/motion.cyclo ./User/motion.d ./User/motion.o ./User/motion.su ./User/nrf24l01.cyclo ./User/nrf24l01.d ./User/nrf24l01.o ./User/nrf24l01.su ./User/servo.cyclo ./User/servo.d ./User/servo.o ./User/servo.su

.PHONY: clean-User

