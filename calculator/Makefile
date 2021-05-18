CPUCLOCK=8000000
CHIP=attiny85
PORT=/dev/ttyACM0
BAUDS=19200
PROGRAMMER=stk500v1
TARGET=calculator
SOURCES=calculator.c lib/tm1637.c lib/random.c

all: clean compile upload

compile: $(SOURCES)
	avr-gcc -Os -DF_CPU=$(CPUCLOCK) -mmcu=$(CHIP) -I. -o $(TARGET).o $(SOURCES)
	avr-ld -o $(TARGET).elf $(TARGET).o
	avr-objcopy -O ihex $(TARGET).elf $(TARGET).hex

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.hex

upload: $(TARGET).hex
	sudo avrdude -c $(PROGRAMMER) -p $(CHIP) -P$(PORT) -b $(BAUDS) -U flash:w:$(TARGET).hex

fuse: 
	sudo avrdude -c $(PROGRAMMER) -p $(CHIP) -P$(PORT) -b $(BAUDS) -U lfuse:w:0xe2:m -U hfuse:w:0xd7:m -U efuse:w:0xff:m
