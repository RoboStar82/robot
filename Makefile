
.PHONY: archive build clean

archive:
	rm -f ../robot_esp32.zip
	zip -r ../robot_esp32.zip .git .gitignore src include platformio.ini version.py Makefile

build:
	~/.platformio/penv/bin/pio run -e robot
	~/.platformio/penv/bin/pio run -e navigation
	~/.platformio/penv/bin/pio run -e transceiver

clean:
	rm -f ../robot_esp32.zip
