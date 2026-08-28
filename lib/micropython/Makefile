
include micropython/py/mkenv.mk

PORT_STM32 = micropython/ports/stm32

QSTR_DEFS += $(PORT_STM32)/qstrdefsport.h

QSTR_GLOBAL_DEPENDENCIES += port/mpconfigboard.h port/mpconfigboard_common.h build/genhdr/modstm_const.h

# FROZEN_MANIFEST ?= $(PORT_STM32)/boards/manifest.py

MICROPY_ROM_TEXT_COMPRESSION ?= 1

MBOOT_TEXT0_ADDR ?= 0x08000000

include $(TOP)/py/py.mk

# include $(TOP)/extmod/extmod.mk

CROSS_COMPILE ?= ~/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-

CFLAGS += -I. -Iport -I$(BUILD) -I$(TOP)
CFLAGS += -I$(HOME)/.platformio/packages/framework-arduinoststm32/system/STM32H7xx
CFLAGS += -I$(HOME)/.platformio/packages/framework-arduinoststm32/system/Drivers/STM32H7xx_HAL_Driver/Inc
CFLAGS += -I$(HOME)/.platformio/packages/framework-arduinoststm32/system/Drivers/CMSIS/Device/ST/STM32H7xx/Include
CFLAGS += -I$(HOME)/.platformio/packages/framework-arduinoststm32/cores/arduino/stm32
CFLAGS += -I$(HOME)/.platformio/packages/framework-arduinoststm32/cores/arduino
CFLAGS += -I$(HOME)/.platformio/packages/framework-cmsis/CMSIS/Core/Include
CFLAGS += -I$(PORT_STM32)
CFLAGS += -DSTM32H743xx -DUSE_FULL_LL_DRIVER
CFLAGS += -DSTM32_HAL_H="<stm32h7xx_hal.h>"
CFLAGS += -DVARIANT_H="\"../../variants/weact_mini_stm32h743vit6/variant_WeActMiniH7xx.h\""
CFLAGS += -Wall -Werror -Wpointer-arith -Wfloat-conversion -Wdouble-promotion -std=gnu99
CFLAGS += -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -mfp16-format=ieee -mtune=cortex-m7 -mcpu=cortex-m7

include $(TOP)/py/mkrules.mk

MAKE_PINS = $(PORT_STM32)/boards/make-pins.py
BOARD_PINS = $(PORT_STM32)/boards/WEACTSTUDIO_MINI_STM32H743/pins.csv
AF_FILE = $(PORT_STM32)/boards/stm32h743_af.csv
PREFIX_FILE = $(PORT_STM32)/boards/stm32f4xx_prefix.c

GEN_PINS_SRC = $(HEADER_BUILD)/pins.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_DEFS = $(HEADER_BUILD)/pins_af_defs.h

$(GEN_PINS_SRC) $(GEN_PINS_HDR) $(GEN_PINS_AF_CONST) $(GEN_PINS_AF_DEFS): $(MAKE_PINS) $(BOARD_PINS) $(AF_FILE) $(PREFIX_FILE)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) \
	--board-csv $(BOARD_PINS) \
	--af-csv $(AF_FILE) \
	--prefix $(PREFIX_FILE) \
	--output-source $(GEN_PINS_SRC) \
	--output-header $(GEN_PINS_HDR) \
	--output-af-const $(GEN_PINS_AF_CONST) \
	--output-af-defs $(GEN_PINS_AF_DEFS)

MAKE_STMCONST = micropython/ports/stm32/make-stmconst.py
GEN_STMCONST_HDR = $(HEADER_BUILD)/modstm_const.h
GEN_STMCONST_MPZ = $(HEADER_BUILD)/modstm_mpz.h
CMSIS_MCU_HDR = $(HOME)/.platformio/packages/framework-arduinoststm32/system/Drivers/CMSIS/Device/ST/STM32H7xx/Include/stm32h743xx.h

$(HEADER_BUILD)/modstm_const.h: $(MAKE_STMCONST) $(CMSIS_MCU_HDR)
	$(ECHO) "GEN stmconst $@"
	$(Q)$(PYTHON) $(MAKE_STMCONST) --mpz $(GEN_STMCONST_MPZ) $(CMSIS_MCU_HDR) \
	-mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -mfp16-format=ieee -mtune=cortex-m7 -mcpu=cortex-m7 \
	> $(GEN_STMCONST_HDR)

all: \
	$(BUILD)/genhdr/mpversion.h \
	$(BUILD)/genhdr/pins.h \
	$(BUILD)/genhdr/modstm_const.h \
	$(BUILD)/genhdr/qstrdefs.generated.h \
	$(BUILD)/genhdr/moduledefs.h \
	$(BUILD)/genhdr/root_pointers.h \
 	$(BUILD)/genhdr/compressed.data.h \

