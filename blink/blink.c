#include <avr/io.h>
#include <util/delay.h>

/*
The AtTiny85 have this pinout:
         __ __
 RESET -|  U  |- VCC
   PB3 -|     |- PB2
   PB4 -|     |- PB1
   GND -|_____|- PB0
 
*/

int main (void){
  //This DDRx defines the direction (on/off) in this case of register(port) B
  //O is input and 1 means output
  //the bitwise OR operation leaves all other ports unchanged.
  //each PBx indicates the bit of that port in the register B byte 
  // PB0 = 0000 0001 = 0x01
  // PB1 = 0000 0010 = 0x02
  // PB2 = 0000 0100 = 0x04
  // PB3 = 0000 1000 = 0x08
  // PB4 = 0001 0000 = 0x10
  DDRB |= 0x10;
  while(1){
    //The port function defines the state of the register(port), in this case B
    //The exclusive or leaves unchanged the other values
    PORTB ^= 0x10;
    _delay_ms(1000);
  }
}
