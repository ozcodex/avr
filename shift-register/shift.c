#include <avr/io.h>
#include <util/delay.h>

void putBit(uint8_t);
void putWord(uint8_t first, uint8_t last);
void putByte(uint8_t byte);
void cycleClock();
void setup();

int main(void) {
  int i=0,j=0;
  uint8_t flag=0;
  uint8_t hex[] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xE6,0xEE,0x3E,0x9C,0x7A,0x9E,0x8E};
  uint8_t load[] = {0x80,0x40,0x20,0x10,0x10,0x08,0x04,0x80};
  setup();

    while( 1 ){
      i = 0;
      while( i < 8 ){
        if( (i & 0x4) >> 2 ) putWord(load[i],0x00);
        else putWord(0x00,load[i]);
        _delay_ms(100);
        i++;
      }
    }
    return 0;
}

void setup() {
  // set pcb0 and pcb1 to out mode
  // 0000 0011 -> 0x03
    DDRB |= 0x03;
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
