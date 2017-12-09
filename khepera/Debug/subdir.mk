################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bit_conv_tab.c \
../i2ccom.c \
../kb_battery.c \
../kb_camera.c \
../kb_cmdparser.c \
../kb_config.c \
../kb_error.c \
../kb_fifo.c \
../kb_gpio.c \
../kb_gripper.c \
../kb_init.c \
../kb_khepera4.c \
../kb_lrf.c \
../kb_memory.c \
../kb_pwm.c \
../kb_socket.c \
../kb_sound.c \
../kb_stargazer.c \
../kb_symbol.c \
../kb_time.c \
../kb_utils.c \
../kmot.c \
../knet.c \
../knet_i2c.c \
../koreio.c \
../test.c 

OBJS += \
./bit_conv_tab.o \
./i2ccom.o \
./kb_battery.o \
./kb_camera.o \
./kb_cmdparser.o \
./kb_config.o \
./kb_error.o \
./kb_fifo.o \
./kb_gpio.o \
./kb_gripper.o \
./kb_init.o \
./kb_khepera4.o \
./kb_lrf.o \
./kb_memory.o \
./kb_pwm.o \
./kb_socket.o \
./kb_sound.o \
./kb_stargazer.o \
./kb_symbol.o \
./kb_time.o \
./kb_utils.o \
./kmot.o \
./knet.o \
./knet_i2c.o \
./koreio.o \
./test.o 

C_DEPS += \
./bit_conv_tab.d \
./i2ccom.d \
./kb_battery.d \
./kb_camera.d \
./kb_cmdparser.d \
./kb_config.d \
./kb_error.d \
./kb_fifo.d \
./kb_gpio.d \
./kb_gripper.d \
./kb_init.d \
./kb_khepera4.d \
./kb_lrf.d \
./kb_memory.d \
./kb_pwm.d \
./kb_socket.d \
./kb_sound.d \
./kb_stargazer.d \
./kb_symbol.d \
./kb_time.d \
./kb_utils.d \
./kmot.d \
./knet.d \
./knet_i2c.d \
./koreio.d \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/usr/local/khepera4-oetools/tmp/sysroots/i686-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-gcc -I/usr/local/khepera4-oetools/tmp/sysroots/i686-linux/usr/include -O0 -g3 -Wall -c -march=armv7-a -mtune=cortex-a8 -Wa,-mcpu=cortex-a8 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


