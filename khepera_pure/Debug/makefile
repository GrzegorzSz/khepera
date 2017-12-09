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
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: khepera_pure

# Tool invocations
khepera_pure: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	/usr/local/khepera4-oetools/tmp/sysroots/i686-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-gcc -L/usr/local/khepera4-oetools/tmp/sysroots/armv7a-angstrom-linux-gnueabi/lib -o "khepera_pure" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) khepera_pure
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
