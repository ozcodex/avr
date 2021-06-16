CPUCLOCK=8000000
CHIP=attiny85
PORT=/dev/ttyACM0
BAUDS=19200
PROGRAMMER=stk500v1
SOURCES=micronucleus.hex

all: upload fuse

upload: $(TARGET).hex
	sudo avrdude -c $(PROGRAMMER) -p $(CHIP) -P$(PORT) -b $(BAUDS) -U flash:w:$(TARGET).hex:i -U lfusew:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m

fuse: 
	sudo avrdude -c $(PROGRAMMER) -p $(CHIP) -P$(PORT) -b $(BAUDS) -U lfuse:w:0xe2:m -U hfuse:w:0xd7:m -U efuse:w:0xff:m
