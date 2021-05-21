#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include "lib/tm1637.h"
#include "lib/chars.h"
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

//This are the displayable numbers including hex [0..f]
uint8_t number[16] = {
  CHAR_0, CHAR_1, CHAR_2, CHAR_3,
  CHAR_4, CHAR_5, CHAR_6, CHAR_7,
  CHAR_8, CHAR_9, CHAR_A, CHAR_B,
  CHAR_C, CHAR_D, CHAR_E, CHAR_F
  };

uint8_t byte_hi(uint8_t hex){
  return hex>>4;
}
uint8_t byte_lo(uint8_t hex){
  return hex&0xf;
}

void render(uint8_t a, uint8_t b,  uint8_t c,  uint8_t d, uint8_t p ){
  TM1637_display_segments(0,a);
  if(p==0)
    TM1637_display_segments(1,b&~0x80);
  else
    TM1637_display_segments(1,b|0x80);
  TM1637_display_segments(2,c);
  TM1637_display_segments(3,d);
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
    render(a,b,number[byte_hi(op)],number[byte_lo(op)],1);
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
        render(CHAR_R,CHAR_O,CHAR_DASH,CHAR_DASH,1);
        break;
      case 1:
        render(CHAR_P,CHAR_A,CHAR_DASH,CHAR_DASH,1);
        break;
      case 2:
        render(CHAR_5,CHAR_I,CHAR_DASH,CHAR_DASH,1);
        break;
    }
  }
}


uint16_t roll_random(){
  //r:read
  uint8_t r = 0x07;
  uint16_t result = 0x0000;
  //roll several random numbers while the button is pressed
  render(CHAR_R,CHAR_O,CHAR_L,CHAR_L,0);
  _delay_ms(500);
  //whain until any button is pressed
  while(r == 0x07){
    r = (PINB & 0x1C) >> 2; //see read_button() to see description
  }
  //roll while any button is still pressed
  while(r != 0x07){
    r = (PINB & 0x1C) >> 2; //see read_button() to see description
    result = random();
    render(number[(result>>12)&0xf],number[(result>>8)&0xf],number[(result>>4)&0xf],number[result&0xf],0);
  }
  return result;
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
        render(CHAR_SPC,CHAR_SPC,CHAR_D,CHAR_4,0);
        break;
      case 1:
        render(CHAR_SPC,CHAR_SPC,CHAR_D,CHAR_6,0);
        break;
      case 2:
        render(CHAR_SPC,CHAR_SPC,CHAR_D,CHAR_8,0);
        break;
      case 3:
        render(CHAR_SPC,CHAR_D,CHAR_1,CHAR_0,0);
        break;
      case 4:
        render(CHAR_SPC,CHAR_D,CHAR_1,CHAR_2,0);
        break;
      case 5:
        render(CHAR_SPC,CHAR_D,CHAR_2,CHAR_0,0);
        break;
      case 6:
        render(CHAR_D,CHAR_1,CHAR_0,CHAR_0,0);
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
      render(CHAR_R,CHAR_O,CHAR_R,CHAR_O,1); //ro:ro
      _delay_ms(2000);
      render(CHAR_SPC,CHAR_T,CHAR_I,CHAR_E,0); //tie
    } else if (j == 1){
      //system choose paper
      render(CHAR_R,CHAR_O,CHAR_P,CHAR_A,1); //ro:PA
      _delay_ms(2000);
      render(CHAR_SPC,CHAR_B,CHAR_A,CHAR_D,0); //bad
    } else if (j == 2){ 
      //system choose sissors
      render(CHAR_R,CHAR_O,CHAR_5,CHAR_I,1); //ro:Si
      _delay_ms(2000);
      render(CHAR_G,CHAR_O,CHAR_O,CHAR_D,0); //good
    }
  }else if(i == 1){
    //user choose paper
    if (j == 0){
      //system choose rock
      render(CHAR_P,CHAR_A,CHAR_R,CHAR_O,1); //PA:ro
      _delay_ms(2000);
      render(CHAR_G,CHAR_O,CHAR_O,CHAR_D,0); //good
    } else if (j == 1){
      //system choose paper
      render(CHAR_P,CHAR_A,CHAR_P,CHAR_A,1); //PA:PA
      _delay_ms(2000);
      render(CHAR_SPC,CHAR_T,CHAR_I,CHAR_E,0); //tie
    } else if (j == 2){ 
      //system choose sissors
      render(CHAR_P,CHAR_A,CHAR_5,CHAR_I,1); //PA:Si
      _delay_ms(2000);
      render(CHAR_SPC,CHAR_B,CHAR_A,CHAR_D,0); //bad
    }
  }else if(i == 2){
    //user choose sissors
    if (j == 0){
      //system choose rock
      render(CHAR_5,CHAR_I,CHAR_R,CHAR_O,1); //Si:ro
      _delay_ms(2000);
      render(CHAR_SPC,CHAR_B,CHAR_A,CHAR_D,0); //bad
    } else if (j == 1){
      //system choose paper
      render(CHAR_5,CHAR_I,CHAR_P,CHAR_A,1); //Si:PA
      _delay_ms(2000);
      render(CHAR_G,CHAR_O,CHAR_O,CHAR_D,0); //good
    } else if (j == 2){
      //system choose sissors
      render(CHAR_5,CHAR_I,CHAR_5,CHAR_I,1); //Si:Si
      _delay_ms(2000);
      render(CHAR_SPC,CHAR_T,CHAR_I,CHAR_E,0); //tie
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
  
  random_init(eeprom_read_word(0x0000)); //setup seed
  eeprom_update_word(0x0000,random());

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
  op = read_input(0xf,CHAR_O,CHAR_P); //limit f and "oP" as tag 
  while(1){
    x++;
    //menu options
    switch(op){
      case 0:
        render(CHAR_SPC,CHAR_O,CHAR_F,CHAR_F,0);
        _delay_ms(500);
        TM1637_init(0,0); //disabled and brightness 0
        sleep_mode();
        return 0;
      case 1:  //Addition
        render(CHAR_SPC,CHAR_A,CHAR_D,CHAR_D,0);
        _delay_ms(500);
        i = read_input(0xf,CHAR_N,1);
        render(CHAR_DASH,CHAR_DASH,CHAR_DASH,CHAR_DASH,0);
        _delay_ms(500);
        j = read_input(0xf,CHAR_N,2);
        render(CHAR_DASH,CHAR_DASH,CHAR_DASH,CHAR_DASH,0);
        _delay_ms(500);
        render(CHAR_R,0xe,byte_hi(i+j),byte_lo(i+j),1);
        _delay_ms(2000);
        break;
      case 2: //Rock Paper Sissor
        render(CHAR_P,CHAR_L,CHAR_A,CHAR_Y,0);
        _delay_ms(500);
        i = get_move();
        j = (random() % 3); //generate numbers in range [0,3)
        print_rps(i,j);
        break;
      case 3:
        render(CHAR_D,CHAR_I,CHAR_C,CHAR_E,0);
        _delay_ms(500);
        i = get_dice();
        x = roll_random();
        switch(i){
          case 0:
            j = (x % 4)+1;
            break;
          case 1:
            j = (x % 6)+1;
            break;
          case 2:
            j = (x % 8)+1;
            break;
          case 3:
            j = (x % 10)+1;
            break;
          case 4:
            j = (x % 12)+1;
            break;
          case 5:
            j = (x % 20)+1;
            break;
          case 6:
            j = (x % 100);
            break;
          }
        i = j % 10;
        j = (j / 10) % 10;
        i = number[i];
        j = j == 0 ? CHAR_SPC : number[j];
        render(CHAR_SPC,CHAR_SPC,j,i,0);
        _delay_ms(2000);
        break;
      default:
        render(CHAR_SPC,0xe,CHAR_R,CHAR_R,0);
        _delay_ms(500);
        break;
    }
  }
}
