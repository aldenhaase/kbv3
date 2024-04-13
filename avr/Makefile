build: src/main.c
	rm -rf build
	mkdir build
	${AVR_TOOLCHAIN_PATH}/avr-gcc \
		-Wall -Os -mmcu=attiny85 src/main.c -o build/led_driver.out
flash: build
	avrdude -c atmelice_isp -p t85 -U flash:w:build/led_driver.out
