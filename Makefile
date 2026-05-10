
.PHONY: all robot_weact_mini_stm32h743vit6 station_heltec_vision_master_e213

all: robot_weact_mini_stm32h743vit6 station_heltec_vision_master_e213

robot_weact_mini_stm32h743vit6:
	~/.platformio/penv/bin/platformio run --environment robot_weact_mini_stm32h743vit6

station_heltec_vision_master_e213:
	~/.platformio/penv/bin/platformio run --environment station_heltec_vision_master_e213
