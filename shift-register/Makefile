CPUCLOCK=8000000
CHIP=attiny85
PORT=/dev/ttyACM0
BAUDS=19200
PROGRAMMER=stk500v1
FILE=shift

all: clean compile upload

compile:
	avr-gcc -Os -DF_CPU=$(CPUCLOCK) -mmcu=$(CHIP) -c $(FILE).c
	avr-gcc -DF_CPU=$(CPUCLOCK) -mmcu=$(CHIP) -o $(FILE).elf $(FILE).o
	avr-objcopy -O ihex $(FILE).elf $(FILE).hex

clean:
	rm $(FILE).o
	rm $(FILE).elf
	rm $(FILE).hex

upload:
	sudo avrdude -c $(PROGRAMMER) -p $(CHIP) -P$(PORT) -b $(BAUDS) -U flash:w:$(FILE).hex:i
