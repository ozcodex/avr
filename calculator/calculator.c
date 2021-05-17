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

//This are the displayable simbols, initially the hex numbers 0..f
uint8_t S[16] = {0x3f,0x06,0x5b,0x4f,
                 0x66,0x6d,0x7d,0x07,
                 0x7f,0x6f,0x77,0x7c,
                 0x39,0x5e,0x79,0x71};


void render(uint8_t aa, uint8_t bb, uint8_t p){
  //Render an hex duple in the screen in the format:
  //  aa:bb
  //the input is aa,bb = [0x00 .. 0xff]   p=[0,1]
  //where aa is the first number, bb is the second number
  //and p can be 1 to show the dots and 0 to not showing the dots

  //the values are shifted to use only the MSB
  TM1637_display_segments(0,S[aa>>4]);
  //the second segment controls the dots
  //
  if(p==0)
    TM1637_display_segments(1,S[aa&0xf]&~0x80);
  else
    TM1637_display_segments(1,S[aa&0xf]|0x80);
  TM1637_display_segments(2,S[bb>>4]);
  TM1637_display_segments(3,S[bb&0xf]);
}

uint8_t read_input(uint8_t limit){
  //r:read  pr:previous_r
  uint8_t r,pr = 0x7;
  //op:selected_option
  uint8_t op = 0x00;

  //infinite loop until read a value
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
      pr = r;
      switch (r){
        case 0x6: //x button
          if(op<=0) op=limit;
          else op--;
          break;
        case 0x3: //z button
          if(op>=limit) op=0x0;
          else op++;
          break;
        case 0x5: //y button
          return op;
      }
    }
    render(0x00,op,1);
  }
}

int main (void){
  uint8_t i,j = 0;
  uint8_t op = 0x0;

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
    //read menu option
    op = read_input(0xf);

    //menu options
    switch(op){
      case 1:
        render(0x0a,0xdd,0);
        _delay_ms(500);
        i = read_input(0xf);
        render(0x00,0x00,0);
        _delay_ms(500);
        j = read_input(0xf);
        render(0x00,0x00,0);
        _delay_ms(500);
        render(0xad,i+j,1);
        _delay_ms(1000);
        break;
      default:
        render(0xee,0xee,0);
        _delay_ms(500);
        break;
    }
  }
}
