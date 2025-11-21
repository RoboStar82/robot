
.PHONY: archive build clean

archive:
	rm -f ../robot_2026.zip
	zip -r ../robot_2026.zip .git .gitignore src include platformio.ini version.py Makefile

build:
	~/.platformio/penv/bin/pio run -e robot
	~/.platformio/penv/bin/pio run -e navigation
	~/.platformio/penv/bin/pio run -e transceiver

clean:
	rm -f ../robot_2026.zip
