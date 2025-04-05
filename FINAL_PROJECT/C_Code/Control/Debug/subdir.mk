################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../I2C.c \
../Main_App_Control.c \
../PIR.c \
../PWM.c \
../Timer.c \
../UART.c \
../buzzer.c \
../external_eeprom.c \
../gpio.c \
../motor.c 

OBJS += \
./I2C.o \
./Main_App_Control.o \
./PIR.o \
./PWM.o \
./Timer.o \
./UART.o \
./buzzer.o \
./external_eeprom.o \
./gpio.o \
./motor.o 

C_DEPS += \
./I2C.d \
./Main_App_Control.d \
./PIR.d \
./PWM.d \
./Timer.d \
./UART.d \
./buzzer.d \
./external_eeprom.d \
./gpio.d \
./motor.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


