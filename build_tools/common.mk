# DMOSDK - DecaWave Module Open Software Development Kit
# Copyright (C) 2018-2019 IRIT-RMESS
#
# This file is part of DMOSDK.
#
# DMOSDK is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DMOSDK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DMOSDK.  If not, see <http://www.gnu.org/licenses/>.

################################################################################
#  General configuration
################################################################################

CMD_ECHO = @

BIN ?= firmware
BUILD_DIR = build
CONFIG ?= .config

-include $(CONFIG)

PROJ_PATH = .
BOARD_PATH = $(DWM1001_FRAMEWORK_PATH)/boards/$(patsubst "%",%,$(CONFIG_BOARD))
EXTERNAL_PATH = $(DWM1001_FRAMEWORK_PATH)/external
NRFX_PATH = $(EXTERNAL_PATH)/nrfx
DECA_SDK_PATH = $(EXTERNAL_PATH)/deca_driver
FREERTOS_PATH = $(EXTERNAL_PATH)/FreeRTOS
CMSIS_PATH = $(EXTERNAL_PATH)/CMSIS
BUILD_TOOLS_PATH = $(DWM1001_FRAMEWORK_PATH)/build_tools
KCONFIG_PATH = $(BUILD_TOOLS_PATH)/kconfig

LINKER_SCRIPT = $(BUILD_TOOLS_PATH)/linker_script.ld
KCONFIG_FILE = $(DWM1001_FRAMEWORK_PATH)/configs/kconfig

TOOLCHAIN ?= arm-none-eabi-
CC      = $(TOOLCHAIN)gcc
AS      = $(TOOLCHAIN)gcc
LD      = $(TOOLCHAIN)gcc
NM      = $(TOOLCHAIN)nm
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE    = $(TOOLCHAIN)size
GDB     = $(TOOLCHAIN)gdb

OPENOCD  = openocd
OPENOCD_DIR = /usr/share/openocd/scripts

.DEFAULT_GOAL := all

################################################################################
#  Echo functions with colors
################################################################################
print_red = /bin/echo -e "\x1b[1m\x1b[31m$1\x1b[0m"
print_green = /bin/echo -e "\x1b[32m$1\x1b[0m"
print_yellow = /bin/echo -e "\x1b[33m$1\x1b[0m"
print_blue = /bin/echo -e "\x1b[36m$1\x1b[0m"

################################################################################
#  Compilation Flags
################################################################################

DEF = -DCUSTOM_BOARD_INC=dw1001_dev

# C flags common to all targets
CFLAGS += -O3 -g3
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52832_XXAA
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += $(DEFINES)

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASFLAGS += -g3
ASFLAGS += -mcpu=cortex-m4
ASFLAGS += -mthumb -mabi=aapcs
ASFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASFLAGS += -DFLOAT_ABI_HARD
ASFLAGS += -DNRF52832_XXAA
ASFLAGS += $(DEFINES)

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(NRFX_PATH)/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs
LDFLAGS += -u _printf_float

LIBS += -lc -lnosys -lm

CFLAGS += -D__HEAP_SIZE=8192
CFLAGS += -D__STACK_SIZE=8192
ASMFLAGS += -D__HEAP_SIZE=8192
ASMFLAGS += -D__STACK_SIZE=8192

################################################################################
#  Sources and includes
################################################################################

SRC_ASM += $(NRFX_PATH)/mdk/gcc_startup_nrf52.S

# Application
SRC_C += $(wildcard $(PROJ_PATH)/*.c)

INC += \
  $(PROJ_PATH) \

# board
INC += \
  $(BOARD_PATH) \

# dwm1001 framework

INC += \
  $(DWM1001_FRAMEWORK_PATH)/src

# nrfx port
INC += \
  $(DWM1001_FRAMEWORK_PATH)/src/nrfx

# serial

ifeq ($(CONFIG_SERIAL), y)
SRC_C += \
  $(DWM1001_FRAMEWORK_PATH)/src/serial/serial.c \
  $(DWM1001_FRAMEWORK_PATH)/src/serial/printf.c

INC += \
  $(DWM1001_FRAMEWORK_PATH)/src/serial
endif

# logger
ifeq ($(CONFIG_LOGGER), y)
SRC_C += \
  $(DWM1001_FRAMEWORK_PATH)/src/logger/logger.c

INC += \
  $(DWM1001_FRAMEWORK_PATH)/src/logger
endif

# nanoprintf
INC += \
  $(EXTERNAL_PATH)/nanoprintf

# reader
SRC_C += \
  $(DWM1001_FRAMEWORK_PATH)/src/reader/reader.c

INC += \
  $(DWM1001_FRAMEWORK_PATH)/src/reader

# json
SRC_C += \
  $(DWM1001_FRAMEWORK_PATH)/src/json/json.c

INC += \
  $(DWM1001_FRAMEWORK_PATH)/src/json

# commands
SRC_C += \
  $(DWM1001_FRAMEWORK_PATH)/src/commands/commands.c

INC += \
  $(DWM1001_FRAMEWORK_PATH)/src/commands

# Decawave driver

ifeq ($(CONFIG_DECA_DRIVER_ENABLED), y)
SRC_C += \
  $(DECA_SDK_PATH)/deca_device.c \
  $(DECA_SDK_PATH)/deca_params_init.c \
  $(DECA_SDK_PATH)/deca_range_tables.c \
  $(DECA_SDK_PATH)/deca_device.c \
  $(DECA_SDK_PATH)/port/port_platform.c

INC += \
  $(DECA_SDK_PATH) \
  $(DECA_SDK_PATH)/port
endif

# Freertos
SRC_C += \
  $(FREERTOS_PATH)/croutine.c \
  $(FREERTOS_PATH)/event_groups.c \
  $(FREERTOS_PATH)/list.c \
  $(FREERTOS_PATH)/portable/CMSIS/nrf52/port_cmsis.c \
  $(FREERTOS_PATH)/portable/CMSIS/nrf52/port_cmsis_systick.c \
  $(FREERTOS_PATH)/portable/GCC/nrf52/port.c \
  $(FREERTOS_PATH)/portable/MemMang/heap_4.c \
  $(FREERTOS_PATH)/queue.c \
  $(FREERTOS_PATH)/stream_buffer.c \
  $(FREERTOS_PATH)/tasks.c \
  $(FREERTOS_PATH)/timers.c \

INC += \
  $(FREERTOS_PATH)/include \
  $(FREERTOS_PATH)/portable/CMSIS/nrf52 \
  $(FREERTOS_PATH)/portable/GCC/nrf52 \

# CMSIS
INC += \
  $(CMSIS_PATH) \

# nrfx

SRC_C += \
  $(NRFX_PATH)/drivers/src/nrfx_adc.c \
  $(NRFX_PATH)/drivers/src/nrfx_clock.c \
  $(NRFX_PATH)/drivers/src/nrfx_comp.c \
  $(NRFX_PATH)/drivers/src/nrfx_dppi.c \
  $(NRFX_PATH)/drivers/src/nrfx_gpiote.c \
  $(NRFX_PATH)/drivers/src/nrfx_i2s.c \
  $(NRFX_PATH)/drivers/src/nrfx_lpcomp.c \
  $(NRFX_PATH)/drivers/src/nrfx_nfct.c \
  $(NRFX_PATH)/drivers/src/nrfx_nvmc.c \
  $(NRFX_PATH)/drivers/src/nrfx_pdm.c \
  $(NRFX_PATH)/drivers/src/nrfx_power.c \
  $(NRFX_PATH)/drivers/src/nrfx_ppi.c \
  $(NRFX_PATH)/drivers/src/nrfx_pwm.c \
  $(NRFX_PATH)/drivers/src/nrfx_qdec.c \
  $(NRFX_PATH)/drivers/src/nrfx_qspi.c \
  $(NRFX_PATH)/drivers/src/nrfx_rng.c \
  $(NRFX_PATH)/drivers/src/nrfx_rtc.c \
  $(NRFX_PATH)/drivers/src/nrfx_saadc.c \
  $(NRFX_PATH)/drivers/src/nrfx_spi.c \
  $(NRFX_PATH)/drivers/src/nrfx_spim.c \
  $(NRFX_PATH)/drivers/src/nrfx_spis.c \
  $(NRFX_PATH)/drivers/src/nrfx_swi.c \
  $(NRFX_PATH)/drivers/src/nrfx_systick.c \
  $(NRFX_PATH)/drivers/src/nrfx_temp.c \
  $(NRFX_PATH)/drivers/src/nrfx_timer.c \
  $(NRFX_PATH)/drivers/src/nrfx_twi.c \
  $(NRFX_PATH)/drivers/src/nrfx_twim.c \
  $(NRFX_PATH)/drivers/src/nrfx_twis.c \
  $(NRFX_PATH)/drivers/src/nrfx_uart.c \
  $(NRFX_PATH)/drivers/src/nrfx_uarte.c \
  $(NRFX_PATH)/drivers/src/nrfx_usbd.c \
  $(NRFX_PATH)/drivers/src/nrfx_wdt.c \
  $(NRFX_PATH)/drivers/src/prs/nrfx_prs.c \
  $(NRFX_PATH)/hal/nrf_ecb.c \
  $(NRFX_PATH)/hal/nrf_nvmc.c \
  $(NRFX_PATH)/mdk/system_nrf52.c \
  $(NRFX_PATH)/soc/nrfx_atomic.c \

INC += \
  $(NRFX_PATH) \
  $(NRFX_PATH)/drivers \
  $(NRFX_PATH)/drivers/include \
  $(NRFX_PATH)/hal \
  $(NRFX_PATH)/mdk \
  $(NRFX_PATH)/soc \

# JSMN
INC += \
  $(EXTERNAL_PATH)/jsmn

################################################################################
#  Move objects files in build directory
################################################################################

FILENAMES_ASM = $(notdir $(SRC_ASM))
OBJS_ASM = $(addprefix $(BUILD_DIR)/, $(FILENAMES_ASM:.S=.o))
vpath %.S $(dir $(SRC_ASM))

FILENAMES_C = $(notdir $(SRC_C))
OBJS_C = $(addprefix $(BUILD_DIR)/, $(FILENAMES_C:.c=.o))
vpath %.c $(dir $(SRC_C))

INCLUDES = $(addprefix -I, $(INC))

################################################################################
#  PreBuild targets section
################################################################################

menuconfig:
	$(KCONFIG_PATH)/menuconfig.py $(KCONFIG_FILE)

generate_config_header:
	@$(call print_red,"--- Generate configuration header file")
	$(KCONFIG_PATH)/genheader.py $(KCONFIG_FILE)

################################################################################
#  Build targets section
################################################################################
ifneq ($(wildcard $(CONFIG)),)

all: generate_config_header $(BUILD_DIR) start_compilation $(BUILD_DIR)/$(BIN).hex
	$(CMD_ECHO) @$(SIZE) $(BUILD_DIR)/$(BIN).elf

start_compilation:
	@$(call print_red,"--- Compilation of C files")

$(BUILD_DIR):
	$(CMD_ECHO) mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.S
	@$(call print_yellow,"ASM  $(notdir $<)")
	$(CMD_ECHO) $(AS) $(ASFLAGS) $(DEF) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: %.c
	@$(call print_green,"CC   $(notdir $<)")
	$(CMD_ECHO) $(CC) $(CFLAGS) $(DEF) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/$(BIN).hex: $(BUILD_DIR)/$(BIN).elf
	@$(call print_red,"--- Generate hex from elf")
	@$(call print_red,"--- Show number of bytes used by section")
	$(CMD_ECHO) $(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(BIN).elf: $(OBJS_ASM) $(OBJS_C)
	@$(call print_blue,"LD   $(notdir $@)")
	$(CMD_ECHO) $(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	@$(call print_red,"--- Clean all files")
	$(CMD_ECHO) rm -rf $(BUILD_DIR)

-include $(BOARD_PATH)/board.mk

endif
