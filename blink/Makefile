CPUCLOCK=8000000
CHIP=attiny85
PORT=/dev/ttyACM0
BAUDS=19200
PROGRAMMER=stk500v1

compile: blink.c
	avr-gcc -Os -DF_CPU=$(CPUCLOCK) -mmcu=$(CHIP) -c blink.c
	avr-gcc -DF_CPU=$(CPUCLOCK) -mmcu=$(CHIP) -o blink.elf blink.o
	avr-objcopy -O ihex blink.elf blink.hex

clean:
	rm blink.o
	rm blink.elf
	rm blink.hex

upload: blink.hex
	sudo avrdude -c $(PROGRAMMER) -p $(CHIP) -P$(PORT) -b $(BAUDS) -U flash:w:blink.hex
