CPUCLOCK=8000000
CHIP=attiny85
PORT=/dev/ttyACM0
BAUDS=19200
PROGRAMMER=stk500v1
PROYECT=calculator

compile: $(PROYECT).c
	avr-gcc -Os -DF_CPU=$(CPUCLOCK) -mmcu=$(CHIP) -c $(PROYECT).c
	avr-gcc -DF_CPU=$(CPUCLOCK) -mmcu=$(CHIP) -o $(PROYECT).elf $(PROYECT).o
	avr-objcopy -O ihex $(PROYECT).elf $(PROYECT).hex

clean:
	rm $(PROYECT).o
	rm $(PROYECT).elf
	rm $(PROYECT).hex

upload: $(PROYECT).hex
	sudo avrdude -c $(PROGRAMMER) -p $(CHIP) -P$(PORT) -b $(BAUDS) -U flash:w:$(PROYECT).hex
