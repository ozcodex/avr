#include <avr/io.h>
#include <util/delay.h>

void putBit(uint8_t);
void putHex(uint8_t byte);
void putWord(uint8_t first, uint8_t last);
void putByte(uint8_t byte);
void cycleClock();
void setup();


uint8_t hex[] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xE6,0xEE,0x3E,0x9C,0x7A,0x9E,0x8E};

int main(void) {
  int i=0,j=0;
  uint8_t byte=0x00;
  setup();

  while( 1 ){
    // Read Analog Data
    // 0100 0000 -> 0x40 ADSC Bit (start conversion)
    ADCSRA |= 0x40;
    byte = ADCH;
    putHex(byte);
    _delay_ms(100);
  }
  return 0;
}

void setup() {
  // set pcb0 and pcb1 to out mode for 7seg display with shift registers
  // 0000 0001 pcb0
  // 0000 0010 pcb1
  // 0000 0011 -> 0x03
  DDRB |= 0x03;
  // setup analog input changing the ADMUX register
  // 0X00 0000 refs0 Voltage Analog Reference, internal or vcc (0:Vcc,1:internal)
  // 0010 0000 ADLAR for 8 bit pressision instead of 10 bit
  // 0000 00XX Analog Pin 01:pb2 10:pb4 11:pb3
  // 0010 0001 -> 0x21
  ADMUX |= 0x21;
  // Setup Analog Read Method ADCSRA Register
  // (Analog to Digital Converter Control and Status Register A)
  // 1000 0000 ADC Enable
  // 0000 0110 Prescaler: 2^n to divide clock freq and keep it in 50-200 kHz
  // 1000 0110 -> 0x86
  ADCSRA |= 0x86;
}


void putHex(uint8_t byte){
  putWord( hex[ ((byte & 0xF0) >> 4) ], hex[ (byte & 0x0F) ] );
}

void putWord(uint8_t first, uint8_t last){
  putByte(last);  
  putByte(first);  
}


void putByte(uint8_t byte){
  int i;
  for(i=0; i < 8; i++ ){
    putBit( (byte >> i) & 0x01);
    
  }
}

void putBit(uint8_t bit) {
  // put the bit in the serial out (pcb0)
  // 0000 0001 -> 0x01
  if(bit == 1) PORTB &= ~(0x01);
  else PORTB |= (0x01);

  cycleClock();
}

void cycleClock() {
  // put a 1 then a 0 in clock (pcb1)
  // 0000 0010 -> 0x02
  PORTB |= (0x02);
  PORTB &= ~(0x02);
}
