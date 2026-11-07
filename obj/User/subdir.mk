################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v00X_it.c \
../User/lib_i2c.c \
../User/main.c \
../User/qma6100p.c \
../User/system_ch32v00X.c \
../User/ws2812b.c 

OBJS += \
./User/ch32v00X_it.o \
./User/lib_i2c.o \
./User/main.o \
./User/qma6100p.o \
./User/system_ch32v00X.o \
./User/ws2812b.o 

C_DEPS += \
./User/ch32v00X_it.d \
./User/lib_i2c.d \
./User/main.d \
./User/qma6100p.d \
./User/system_ch32v00X.d \
./User/ws2812b.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-elf-gcc -march=rv32ec_zmmul_xw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\LIGHTSABER\Debug" -I"C:\MRS_DATA\workspace\LIGHTSABER\Core" -I"C:\MRS_DATA\workspace\LIGHTSABER\User" -I"C:\MRS_DATA\workspace\LIGHTSABER\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

