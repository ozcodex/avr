#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
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
 *
 *  The other ones are buttons PB2, PB3, PB4
 * */

//This are the displayable simbols, initially the hex numbers 0..f
uint8_t Sym[32] = {0x3f,0x06,0x5b,0x4f,  //0,1,2,3
                   0x66,0x6d,0x7d,0x07,  //4,5,6,7
                   0x7f,0x6f,0x77,0x7c,  //8,9,A,b
                   0x39,0x5e,0x79,0x71,  //C,d,E,F
                   0x00,0x73,0x50,0x5c,  // ,P,r,o
                   0x30,0x40,0x54,0x3d,  //i,-,n,G
                   0x78,0x38,0x6e,0x00,  //t,L,Y
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
    //0x05 = a button pressed
    //0x03 = b button pressed
    //0x06 = c button pressed
    //0x01 = ab buttons pressed
    //0x04 = ac buttons pressed
    //0x02 = bc buttons pressed
    switch (r){
      case 0x5: //a button
        return 0;
      case 0x3: //b button
        return 1;
      case 0x6: //c button
        return 2;
      default:
        if(mode==1) return 3;
        break;
    }
  }
}

uint8_t read_input(uint8_t limit,uint8_t a, uint8_t b){
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
    render(a,b,byte_hi(op),byte_lo(op),1);
  }
}

uint8_t get_move(){
  //r:read  pr:previous_r
  uint8_t r,pr = 0x7;
  //op:selected_option
  uint8_t op = 0x00;

  while(1){
    r = read_button(1); //mode 1, return always 
    if(r != pr){
      pr = r;
      switch (r){
        case 0:
          if(op<=0) op=2;
          else op--;
          break;
        case 2:
          if(op>=2) op=0x0;
          else op++;
          break;
        case 1:
          return op;
      }
    }
    switch(op){
      case 0:
        render(0xf+3,0xf+4,0xf+6,0xf+6,1);
        break;
      case 1:
        render(0xf+2,0xa,0xf+6,0xf+6,1);
        break;
      case 2:
        render(0x5,0xf+5,0xf+6,0xf+6,1);
        break;
    }
  }
}

uint8_t get_dice(){
  //r:read  pr:previous_r
  uint8_t r,pr = 0x7;
  //op:selected_option
  uint8_t op = 0x00;

  while(1){
    r = read_button(1); //mode 1, return always 
    if(r != pr){
      pr = r;
      switch (r){
        case 0:
          if(op<=0) op=6;
          else op--;
          break;
        case 2:
          if(op>=6) op=0x0;
          else op++;
          break;
        case 1:
          return op;
      }
    }
    switch(op){
      case 0:
        render(0xf+1,0xf+1,0xd,0x4,0);
        break;
      case 1:
        render(0xf+1,0xf+1,0xd,0x6,0);
        break;
      case 2:
        render(0xf+1,0xf+1,0xd,0x8,0);
        break;
      case 3:
        render(0xf+1,0xd,0x1,0x0,0);
        break;
      case 4:
        render(0xf+1,0xd,0x1,0x2,0);
        break;
      case 5:
        render(0xf+1,0xd,0x2,0x0,0);
        break;
      case 6:
        render(0xd,0x1,0x0,0x0,0);
        break;
    }
  }
}


void print_rps(uint8_t i, uint8_t j){
  //Print the rock paper sissors result
  if(i == 0){
    // user choose rock
    if (j == 0){
      //system choose rock
      render(0xf+3,0xf+4,0xf+3,0xf+4,1); //ro:ro
      _delay_ms(2000);
      render(0xf+1,0xf+9,0xf+5,0xe,0); //tie
    } else if (j == 1){
      //system choose paper
      render(0xf+3,0xf+4,0xf+2,0xa,1); //ro:PA
      _delay_ms(2000);
      render(0xf+1,0xb,0xa,0xd,0); //bad
    } else if (j == 2){ 
      //system choose sissors
      render(0xf+3,0xf+4,0x5,0xf+5,1); //ro:Si
      _delay_ms(2000);
      render(0xf+8,0xf+4,0xf+4,0xd,0); //good
    }
  }else if(i == 1){
    //user choose paper
    if (j == 0){
      //system choose rock
      render(0xf+2,0xa,0xf+3,0xf+4,1); //PA:ro
      _delay_ms(2000);
      render(0xf+8,0xf+4,0xf+4,0xd,0); //good
    } else if (j == 1){
      //system choose paper
      render(0xf+2,0xa,0xf+2,0xa,1); //PA:PA
      _delay_ms(2000);
      render(0xf+1,0xf+9,0xf+5,0xe,0); //tie
    } else if (j == 2){ 
      //system choose sissors
      render(0xf+2,0xa,0x5,0xf+5,1); //PA:Si
      _delay_ms(2000);
      render(0xf+1,0xb,0xa,0xd,0); //bad
    }
  }else if(i == 2){
    //user choose sissors
    if (j == 0){
      //system choose rock
      render(0x5,0xf+5,0xf+3,0xf+4,1); //Si:ro
      _delay_ms(2000);
      render(0xf+1,0xb,0xa,0xd,0); //bad
    } else if (j == 1){
      //system choose paper
      render(0x5,0xf+5,0xf+2,0xa,1); //Si:PA
      _delay_ms(2000);
      render(0xf+8,0xf+4,0xf+4,0xd,0); //good
    } else if (j == 2){
      //system choose sissors
      render(0x5,0xf+5,0x5,0xf+5,1); //Si:Si
      _delay_ms(2000);
      render(0xf+1,0xf+9,0xf+5,0xe,0); //tie
    }
  }
  _delay_ms(2000);
}

int main (void){
  uint8_t i,j = 0;
  uint16_t x=0;
  uint8_t op = 0x0;

  //Setup Sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Setup display: enable (1: on, 0: off), brightness (0..7)
  TM1637_init(1,0);

  // Setup random number generator
  
  random_init(eeprom_read_byte(0x00)); //setup seed
  eeprom_update_byte(0x00,eeprom_read_byte(0x00)+1);

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

  //read menu option
  op = read_input(0xf,0xf+4,0xf+2); //limit f and "oP" as tag 
  while(1){
    x++;
    //menu options
    switch(op){
      case 0:
        render(0xf+1,0xf+4,0xf,0xf,0);
        _delay_ms(500);
        TM1637_init(0,0); //disabled and brightness 0
        sleep_mode();
        return 0;
      case 1:  //Addition
        render(0xf+1,0xa,0xd,0xd,0);
        _delay_ms(500);
        i = read_input(0xf,0xf+7,1);
        render(0xf+6,0xf+6,0xf+6,0xf+6,0);
        _delay_ms(500);
        j = read_input(0xf,0xf+7,2);
        render(0xf+6,0xf+6,0xf+6,0xf+6,0);
        _delay_ms(500);
        render(0xf+3,0xe,byte_hi(i+j),byte_lo(i+j),1);
        _delay_ms(2000);
        break;
      case 2: //Rock Paper Sissor
        render(0xf+2,0xf+0xa,0xa,0xf+0xb,0);
        _delay_ms(500);
        i = get_move();
        j = (random() % 3); //generate numbers in range [0,3)
        print_rps(i,j);
        break;
      case 3:
        render(0xd,0xf+5,0xc,0xe,0);
        _delay_ms(500);
        i = get_dice();
        switch(i){
          case 0:
            j = (random() % 4)+1;
            break;
          case 1:
            j = (random() % 6)+1;
            break;
          case 2:
            j = (random() % 8)+1;
            break;
          case 3:
            j = (random() % 10)+1;
            break;
          case 4:
            j = (random() % 12)+1;
            break;
          case 5:
            j = (random() % 20)+1;
            break;
          case 6:
            j = (random() % 100);
            break;
          }
        render(0xf+3,0xf+4,0xf+0xa,0xf+0xa,0);
        _delay_ms(500);
        i = j % 10;
        j = (j / 10) % 10;
        render(0xf+1,0xf+1,j==0?0xf+1:j,i,0);
        _delay_ms(2000);
        break;
      case 4:
        render(0x5,0xe,0xe,0xd,0);
        _delay_ms(500);
        render(0xf+1,0xf+1,byte_hi(i),byte_lo(i),0);
        _delay_ms(2000);
        break;
      default:
        render(0xf+1,0xe,0xf+3,0xf+3,0);
        _delay_ms(500);
        break;
    }
  }
}
