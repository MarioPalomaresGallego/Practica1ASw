################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Practica1ASw.exe

# Tool invocations
Practica1ASw.exe: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: RTEMS Linker'
	/home/mario/development/rtems/compiler/5/bin/arm-rtems5-gcc -B/home/mario/development/rtems/bsps/5/arm-rtems5/raspberrypi2/lib/ -specs bsp_specs -qrtems -ffunction-sections -fdata-sections -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -mtune=cortex-a7 -Wl,--gc-sections -o "Practica1ASw.exe" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Practica1ASw.exe
	-@echo ' '

post-build:
	-arm-rtems5-objcopy Practica1ASw.exe -O binary Practica1ASw.bin; gzip -9 -f -c Practica1ASw.bin > Practica1ASw.bin.gz; mkimage -A arm -O linux -T kernel -a 0x200000 -e 0x200000 -n RTEMS -d Practica1ASw.bin.gz kernel.img; mv kernel.img /svr/tftp
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
