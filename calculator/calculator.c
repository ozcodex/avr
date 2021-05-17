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
 * */

int main (void){
  uint8_t n, k = 0;

  // Setup: enable (1: on, 0: off), brightness (0..7)
  TM1637_init(1,5);

  while(1){
    for( n=0; n< TM1637_POSITION_MAX; ++n)
      TM1637_display_digit(n, (k + n)% 0x10);
    TM1637_display_colon(1);
    _delay_ms(200);
    TM1637_display_colon(0);
    _delay_ms(200);
    k++;
  }
}
