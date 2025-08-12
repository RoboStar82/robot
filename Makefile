
clean:
	rm -f ../robot_esp32.zip

zip:
	rm -f ../robot_esp32.zip
	zip -r ../robot_esp32.zip .git .gitignore src include platformio.ini version.py Makefile
