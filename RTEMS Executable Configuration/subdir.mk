################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../controladorA.c \
../displayA.c \
../myTimer.c 

OBJS += \
./controladorA.o \
./displayA.o \
./myTimer.o 

C_DEPS += \
./controladorA.d \
./displayA.d \
./myTimer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: RTEMS C Compiler'
	/home/mario/development/rtems/compiler/5/bin/arm-rtems5-gcc -B/home/mario/development/rtems/bsps/5/arm-rtems5/raspberrypi2/lib/ -specs bsp_specs -qrtems -ffunction-sections -fdata-sections -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -mtune=cortex-a7 -O0 -g -Wall -c -fmessage-length=0 -pipe -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


