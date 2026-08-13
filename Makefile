
.PHONY: all version clean

SRC_LIST := $(wildcard src/**/*.cpp src/**/*.c)
INCLUDE_LIST := $(wildcard include/**/*.h)
INCLUDE_LIST := $(filter-out include/version.h, $(INCLUDE_LIST))

all: .pio/build/robot_stm32h743vit6_ota/firmware.bin .pio/build/station_vme213_ota/firmware.bin

version:
	python3 version.py

clean:
	rm -rf .pio

include/version.h: platformio.ini $(SRC_LIST) $(INCLUDE_LIST)
	python3 version.py

.pio/build/robot_stm32h743vit6_ota/firmware.bin: include/version.h
	~/.platformio/penv/bin/platformio run --environment robot_stm32h743vit6_ota

.pio/build/station_vme213_ota/firmware.bin: include/version.h
	~/.platformio/penv/bin/platformio run --environment station_vme213_ota
