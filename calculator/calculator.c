#include <avr/io.h>
#include <util/delay.h>
#include "lib/tm1637.h"

/*
The AtTiny85 have this pinout:
         __ __
 RESET -|  U  |- VCC
   PB3 -|     |- PB2
   PB4 -|     |- PB1
   GND -|_____|- PB0
 
*/

/*
 * Calculator
 *
 * This program takes a TM1637 4 digit 7 segment display as output and two 
 * buttons as input.
 * I will be adding some utities te this program. initially it has a hello 
 * world that counts the number op pushes on each of the buttons.
 *
 * Connection pins for display are:
 *  - DIO: PB0
 *  - CLK: PB1
 * */

int main (void){
  uint8_t x,y,z,r = 0;
  uint8_t pr = 0; //previous r (read)

  // Setup display: enable (1: on, 0: off), brightness (0..7)
  TM1637_init(1,1);

  //Setup Input buttons
  //unwrite ports to be sure that they are zeroes
  //this sets the port as inputs 
  DDRB &= ~(1 << DDB2);
  DDRB &= ~(1 << DDB3);
  DDRB &= ~(1 << DDB4);
  //set the internal pull-up resistors
  PORTB |= (1 << PORTB2);
  PORTB |= (1 << PORTB3);
  PORTB |= (1 << PORTB4);

  while(1){
    //get the pin values isolated
    // B00011100 = 0x1C
    // the ones idicates PB2,PB3 and PB4
    //then shift it 2 spaces
    r = (PINB & 0x1C) >> 2;
    //this will leave on r 00000ZYX where x is PB2, y is PB3, and z is PB4
    //whit this r will be:
    //0x00 = all button pressed
    //0x07 = no button pressed
    //0x06 = x button pressed
    //0x05 = y button pressed
    //0x04 = xy buttons pressed
    //0x03 = z button pressed
    //0x02 = xz buttons pressed
    //0x01 = yz buttons pressed
    
    if (pr != r){  //if previous read is different
      TM1637_display_digit(0,r);
      pr = r;
    }
  }
}
