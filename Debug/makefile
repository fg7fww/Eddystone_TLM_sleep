################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include code/subdir.mk
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
SECONDARY_FLASH += \
Eddystone_TLM_sleep.hex \

SECONDARY_SIZE += \
Eddystone_TLM_sleep.siz \


# All Target
all: Eddystone_TLM_sleep.elf secondary-outputs

# Tool invocations
Eddystone_TLM_sleep.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "C:\Users\fg7fww\_Install\App\DemoBoards\RSL10\workspace_r1.4_be\Eddystone_TLM_sleep\sections.ld" -nostartfiles -Xlinker --gc-sections -L"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../lib/release" -L"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../lib/ble_core/release" -L"C:\Program Files (x86)\ON Semiconductor\RSL10 EDK_1.4\eclipse\../lib/ble_profiles/release" -Wl,-Map,"Eddystone_TLM_sleep.map" --specs=nano.specs -o "Eddystone_TLM_sleep.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Eddystone_TLM_sleep.hex: Eddystone_TLM_sleep.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "Eddystone_TLM_sleep.elf"  "Eddystone_TLM_sleep.hex"
	@echo 'Finished building: $@'
	@echo ' '

Eddystone_TLM_sleep.siz: Eddystone_TLM_sleep.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "Eddystone_TLM_sleep.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Eddystone_TLM_sleep.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
