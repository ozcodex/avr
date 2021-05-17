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
}
