#include <avr/io.h>
#include <util/delay.h>
#include "lib/tm1637.h"
#include "lib/random.h"

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
uint8_t Sym[32] = {0x3f,0x06,0x5b,0x4f,  //0,1,2,3
                   0x66,0x6d,0x7d,0x07,  //4,5,6,7
                   0x7f,0x6f,0x77,0x7c,  //8,9,A,b
                   0x39,0x5e,0x79,0x71,  //C,d,E,F
                   0x00,0x73,0x50,0x5c,  // ,P,r,o
                   0x30,0x40,0x54,0x4d,  //i,-,n,G
                   0x00,0x00,0x00,0x00,  //
                   0x00,0x00,0x00,0x00}; //

uint8_t byte_hi(uint8_t hex){
  return hex>>4;
}
uint8_t byte_lo(uint8_t hex){
  return hex&0xf;
}

void render(uint8_t a, uint8_t b,  uint8_t c,  uint8_t d, uint8_t p ){
  TM1637_display_segments(0,Sym[a]);
  if(p==0)
    TM1637_display_segments(1,Sym[b]&~0x80);
  else
    TM1637_display_segments(1,Sym[b]|0x80);
  TM1637_display_segments(2,Sym[c]);
  TM1637_display_segments(3,Sym[d]);
}

uint8_t read_button(uint8_t mode){
  //modes:
  //  0: return only after a key is pressed
  //  1: return after a change
  //r:read
  uint8_t r = 0x00;
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
    switch (r){
      case 0x6: //x button
        return 0;
      case 0x5: //y button
        return 1;
      case 0x3: //z button
        return 2;
      default:
        if(mode==1) return 3;
        break;
    }
  }
}

uint8_t read_input(uint8_t limit){
  //r:read  pr:previous_r
  uint8_t r,pr = 0x7;
  //op:selected_option
  uint8_t op = 0x00;

  while(1){
    r = read_button(1); //mode 1, return always 
    if(r != pr){
      pr = r;
      switch (r){
        case 0: //x button
          if(op<=0) op=limit;
          else op--;
          break;
        case 2: //z button
          if(op>=limit) op=0x0;
          else op++;
          break;
        case 1: //y button
          return op;
      }
    }
    render(0xf+4,0xf+2,byte_hi(op),byte_lo(op),1);
  }
}

int main (void){
  uint8_t i,j = 0;
  uint8_t op = 0x0;

  // Setup display: enable (1: on, 0: off), brightness (0..7)
  TM1637_init(1,1);

  // Setup random number generator
  random_init(0xa83f);

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
      case 1:  //Addition
        render(0xf+1,0xa,0xd,0xd,0);
        _delay_ms(500);
        i = read_input(0xf);
        render(0xf+6,0xf+6,0xf+6,0xf+6,0);
        _delay_ms(500);
        j = read_input(0xf);
        render(0xf+6,0xf+6,0xf+6,0xf+6,0);
        _delay_ms(500);
        render(0xf+3,0xe,byte_hi(i+j),byte_lo(i+j),1);
        _delay_ms(2000);
        break;
      case 2: //Rock Paper Sissor
        i = read_button(0); //mode 0, wait for key press
        j = (random() % 3); //generate numbers in range [0,3)
        render(0xf+1,i,0xf+1,j,1);
        _delay_ms(2000);
        break;
      default:
        render(0xf+1,0xe,0xf+3,0xf+3,0);
        _delay_ms(500);
        break;
    }
  }
}
