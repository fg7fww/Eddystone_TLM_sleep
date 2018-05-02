################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/app_init.c \
../code/app_process.c \
../code/ble_bass.c \
../code/ble_custom.c \
../code/ble_std.c \
../code/calibration.c \
../code/i2c.c \
../code/nct375.c 

S_UPPER_SRCS += \
../code/wakeup_asm.S 

OBJS += \
./code/app_init.o \
./code/app_process.o \
./code/ble_bass.o \
./code/ble_custom.o \
./code/ble_std.o \
./code/calibration.o \
./code/i2c.o \
./code/nct375.o \
./code/wakeup_asm.o 

S_UPPER_DEPS += \
./code/wakeup_asm.d 

C_DEPS += \
./code/app_init.d \
./code/app_process.d \
./code/ble_bass.d \
./code/ble_custom.d \
./code/ble_std.d \
./code/calibration.d \
./code/i2c.d \
./code/nct375.d 


# Each subdirectory must supply rules for building sources it contributes
code/%.o: ../code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -Wall -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRSL10_CID=101 -DCFG_LIGHT_STACK -DCFG_ADV_INTERVAL_MS=2000 -DCFG_BLE=1 -DCFG_SLEEP -DCFG_HW_AUDIO -DCFG_ALLROLES=1 -DCFG_APP -DCFG_APP_BATT -DCFG_ATTS=1 -DCFG_CON=1 -DCFG_EMB=1 -DCFG_HOST=1 -DCFG_RF_ATLAS=1 -DCFG_ALLPRF=1 -DCFG_PRF=1 -DCFG_NB_PRF=2 -DCFG_CHNL_ASSESS=1 -DCFG_SEC_CON=1 -DCFG_EXT_DB -DCFG_PRF_BASS=1 -I"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../include" -I"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../include/bb" -I"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../include/ble" -I"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../include/kernel" -I"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../include/ble/profiles" -I"C:\Users\fg7fww\_Install\App\DemoBoards\RSL10\workspace_r1.4_be\Eddystone_TLM_sleep\include" -std=gnu11 -Wmissing-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ../code/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


