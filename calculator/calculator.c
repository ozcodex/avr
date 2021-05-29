#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include "lib/tm1637.h"
#include "lib/chars.h"
#include "lib/random.h"
#include "lib/processor.h"

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

//Fuctions signatures
uint8_t byte_hi(uint8_t hex);
uint8_t byte_lo(uint8_t hex);
void render(uint8_t a, uint8_t b,  uint8_t c,  uint8_t d, uint8_t p );
void print_menu_option(uint8_t op);
void print_rps(uint8_t i, uint8_t j);
void print_loading(uint8_t status);
uint8_t read_button(uint8_t mode);
uint8_t read_input(uint8_t limit,uint8_t a, uint8_t b);
uint8_t get_menu_op();
uint8_t get_move();
uint8_t get_dice();
void power_off();
void hex_add();
uint16_t roll_random();
void check_dice(uint8_t dice, uint16_t roll);
void tally_counter();
void led_settings();
void edit_eeprom(uint16_t init, uint16_t end);
void run(uint16_t init, uint16_t end);
void clear_eeprom(uint16_t init, uint16_t end);

/*
 *
 * MAIN FUNCTION
 *
 */

int main (void){
  uint8_t i,j,k = 0;
  uint16_t x=0;
  uint8_t op = 0x0;

  //Setup Sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Setup display: enable (1: on, 0: off), brightness (0..7)
  i = eeprom_read_byte((const uint8_t *) (0x0002));
  i = i<8?i:0; //ensure that i is less than 8
  TM1637_init(1,i);

  // Setup random number generator
  
  random_init(eeprom_read_word(0x0000)); //setup seed
  eeprom_update_word(0x0000,random());
  eeprom_busy_wait();

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
  op = get_menu_op();
  while(1){
    //menu options
    switch(op){
      case 0:
          power_off();
        return 0;
      case 1:  //Addition
          hex_add();
        break;
      case 2: //Rock Paper Sissor
        i = get_move();
        j = (random() % 3); //generate numbers in range [0,3)
        print_rps(i,j);
        break;
      case 3: //Dice roller
        i = get_dice();
        x = roll_random();
        check_dice(i,x);
        break;
      case 4:
        tally_counter();
        break;
      case 5:
        led_settings();
        break;
      case 6:
        edit_eeprom(0x0100,0x01FF);
        break;
      case 7:
        run(0x0100,0x01FF);
        break;
      case 8:
        clear_eeprom(0x0100, 0x01FF);
        break;
      default:
        render(CHAR_SPC,CHAR_E,CHAR_R,CHAR_R,0);
        _delay_ms(500);
        break;
    }
  }
}

/*
 * 
 *   RENDER FUNCTIONS
 *
 */

void render(uint8_t a, uint8_t b,  uint8_t c,  uint8_t d, uint8_t p ){
  TM1637_display_segments(0,a);
  if(p==0)
    TM1637_display_segments(1,b&~0x80);
  else
    TM1637_display_segments(1,b|0x80);
  TM1637_display_segments(2,c);
  TM1637_display_segments(3,d);
}

void print_menu_option(uint8_t op){
  switch(op){
    case 0:
      render(CHAR_SPC,CHAR_O,CHAR_F,CHAR_F,0);
      break;
    case 1:
      render(CHAR_A,CHAR_D,CHAR_D,CHAR_SPC,0);
      break;
    case 2:
      render(CHAR_P,CHAR_L,CHAR_A,CHAR_Y,0);
      break;
    case 3:
      render(CHAR_D,CHAR_I,CHAR_C,CHAR_E,0);
      break;
    case 4:
      render(CHAR_T,CHAR_A,CHAR_L,CHAR_Y,0);
      break;
    case 5:
      render(CHAR_L,CHAR_E,CHAR_D,CHAR_SPC,0);
      break;
    case 6:
      render(CHAR_E,CHAR_D,CHAR_I,CHAR_T,0);
      break;
    case 7:
      render(CHAR_R,CHAR_U,CHAR_N,CHAR_SPC,0);
      break;
    case 8:
      render(CHAR_C,CHAR_L,CHAR_R,CHAR_SPC,0);
      break;
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

void print_loading(uint8_t status){
  uint8_t bar = 0;
  bar = (status)%12;
  if (bar == 0) render(0,0,0,LINE_UR,0);
  if (bar == 1) render(0,0,0,LINE_DR,0);
  if (bar == 2) render(0,0,0,LINE_D,0);
  if (bar == 3) render(0,0,LINE_D,0,0);
  if (bar == 4) render(0,LINE_D,0,0,0);
  if (bar == 5) render(LINE_D,0,0,0,0);
  if (bar == 6) render(LINE_DL,0,0,0,0);
  if (bar == 7) render(LINE_UL,0,0,0,0);
  if (bar == 8) render(LINE_U,0,0,0,0);
  if (bar == 9) render(0,LINE_U,0,0,0);
  if (bar == 10) render(0,0,LINE_U,0,0);
  if (bar == 11) render(0,0,0,LINE_U,0);
}

uint8_t byte_hi(uint8_t hex){
  return hex>>4;
}

uint8_t byte_lo(uint8_t hex){
  return hex&0xf;
}



/*
 *
 * INPUT FUNCTIONS
 *
 */

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
      case 0x7: //no button pressed
        if(mode==1) return 3;
        break;
      default:
        if(mode==1) return 4;
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
          if(byte_hi(op)>=byte_hi(limit)) op=0+byte_lo(op);
          else op += 0x10;
          break;
        case 2: //z button
          if(byte_lo(op)>=byte_lo(limit)) op=(0x10*byte_hi(op));
          else op++;
          break;
        case 1: //y button
          return op;
      }
    }
    render(a,b,number[byte_hi(op)],number[byte_lo(op)],1);
  }
}

uint8_t get_menu_op(){
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
          if(op<=0) op=0x8;
          else op--;
          break;
        case 2:
          if(op>=0xF) op=0x0;
          else op++;
          break;
        case 1:
          return op;
      }
    }
    print_menu_option(op);
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

uint8_t get_dice(){
  //r:read  pr:previous_r
  uint8_t r,pr = 0x7;
  //op:selected_option
  uint8_t op = 0x06;

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

/*
 *
 *  PROCESS FUNCTIONS
 *
 */

void power_off(){
  //disabled screen and set brightness to 0
  TM1637_init(0,0);
  sleep_mode();
}

void hex_add(){
  uint8_t i = 0;
  uint8_t j = 0;
  render(CHAR_SPC,CHAR_A,CHAR_D,CHAR_D,0);
  _delay_ms(500);
  i = read_input(0x7f,CHAR_N,CHAR_1);
  render(CHAR_DASH,CHAR_DASH,CHAR_DASH,CHAR_DASH,0);
  _delay_ms(500);
  j = read_input(0x7f,CHAR_N,CHAR_2);
  render(CHAR_DASH,CHAR_DASH,CHAR_DASH,CHAR_DASH,0);
  _delay_ms(500);
  render(CHAR_R,CHAR_E,number[byte_hi(i+j)],number[byte_lo(i+j)],1);
  _delay_ms(2000);
}

uint16_t roll_random(){
  //r:read
  uint8_t a,b,r = 3;
  uint16_t result = 0x0000;
  //roll several random numbers while the button is pressed
  render(CHAR_R,CHAR_O,CHAR_L,CHAR_L,0);
  _delay_ms(500);
  //whain until any button is pressed
  while(r == 3){
    r = read_button(1); //mode 1, return allways
  }
  //roll while any button is still pressed
  while(r != 3){
    r = read_button(1); //mode 1, return allways
    result = random();
    print_loading(a/12);
    a++;
    if (a >= 144) a = 0;
  }
  return result;
}


void check_dice(uint8_t dice, uint16_t roll){
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t result = 0;
  switch(dice){
    case 0:
      result = (roll % 4)+1;
      break;
    case 1:
      result = (roll % 6)+1;
      break;
    case 2:
      result = (roll % 8)+1;
      break;
    case 3:
      result = (roll % 10)+1;
      break;
    case 4:
      result = (roll % 12)+1;
      break;
    case 5:
      result = (roll % 20)+1;
      break;
    case 6:
      result = (roll % 100);
      break;
    }
  i = result % 10;
  i = number[i];
  j = (result / 10) % 10;
  j = j == 0 ? CHAR_SPC : number[j];
  render(CHAR_SPC,CHAR_SPC,j,i,0);
  _delay_ms(2000);
}

void tally_counter(){
  uint8_t i = 0;
  uint8_t j = 0;
  i = 0;
  while(1){
    render( 
        number[(i / 1000) % 10], 
        number[(i / 100) % 10], 
        number[(i / 10) % 10 ], 
        number[i % 10] ,0);
    j = read_button(0); //mode 0, wait for push
    switch(j){
      case 0:
        if (i>0) i--;
        break;
      case 1:
        if (i<9999) i++;
        break;
      case 2:
        i = 0;
      break;
    }
    //wait until no key is pressed
    while(read_button(1)!=3);
  }
}

void led_settings(){
  uint8_t i = 0;
  uint8_t j = 0;
  while(1){
    TM1637_init(1,i); //enabled and brightness i
    render(CHAR_L,CHAR_D,number[0],number[i],0);
    j = read_button(0); //mode 0, wait for push
    switch(j){
      case 0:
        if (i>0) i--;
        break;
      case 1:
        render(CHAR_5,CHAR_T,CHAR_O,CHAR_R,1);
        _delay_ms(500);
        eeprom_update_byte((uint8_t *) (0x02),i);
        eeprom_busy_wait();
        render(CHAR_D,CHAR_O,CHAR_N,CHAR_E,0);
        _delay_ms(500);
        break;
      case 2:
        if (i<7) i++;
      break;
    }
    //wait until no key is pressed
    while(read_button(1)!=3);
  }
}

void edit_eeprom(uint16_t init, uint16_t end){
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t k = 0;
  uint16_t x = 0;
  if (end - init > 0xFF){
    return;
  }
  while(1){
    k = eeprom_read_byte((uint8_t *)(init + x) );
    render( 
        number[byte_hi(x)], 
        number[byte_lo(x)], 
        number[byte_hi(k)], 
        number[byte_lo(k)], 1);
    j = read_button(0); //mode 0, wait for push
    switch(j){
      case 0:
        if (x>0) x-= 1;
        break;
      case 1:
        //wait until no key is pressed
        while(read_button(1)!=3);
        i = read_input(0xff, CHAR_U, CHAR_P ) ;
        render(CHAR_5,CHAR_T,CHAR_O,CHAR_R,0);
        _delay_ms(500);
        eeprom_update_byte((uint8_t *) (init + x),i);
        eeprom_busy_wait();
        render(CHAR_D,CHAR_O,CHAR_N,CHAR_E,0);
        _delay_ms(500);
        break;
      case 2:
        if ((init + x) < end) x+= 1;
        break;
    }
    //wait until no key is pressed
    while(read_button(1)!=3);
  }
}

void run(uint16_t init, uint16_t end){
  uint8_t flag = 0;
  uint8_t input = 0;
  uint8_t k = 0;
  uint8_t kp1 = 0;
  uint8_t res = 0;
  uint16_t x = 0;

  init_processor();
  while(1){ 
    x = get_next_instruction();
    k = eeprom_read_byte((uint8_t *)(init + x) );
    kp1 = eeprom_read_byte((uint8_t *)(init + x + 1) );
    //check if must stop
    if (k == 0x00 || init+x > end){
      _delay_ms(2000);
      render(CHAR_E,CHAR_N,CHAR_D,CHAR_SPC,0);
      _delay_ms(500);
      main();
    }
    //execute code according to params
    if(check_params(k))
      res = execute(k,kp1);
    else
      res = execute(k,0x0);
    //check response
    if(res != E_NO_ERROR){
      render(CHAR_E,CHAR_R,CHAR_R,CHAR_SPC,0);
      _delay_ms(500);
      main();  
    }
    //check if input is required
    if(get_input_flag()){
      input = read_input(0xFF,CHAR_I,CHAR_N);
      write_input_data(input);
    }
    //draw the screen
    render(get_output(0),get_output(1),get_output(2),get_output(3),0);
  }
}

void clear_eeprom(uint16_t init, uint16_t end){
  uint8_t r = 0;
  uint16_t i = 0;
  render(CHAR_5,CHAR_U,CHAR_R,CHAR_E,0);
  //wait until no key is pressed
  while(read_button(1)!=3);
  r = read_button(0); //mode 0, wait for push
  if(r != 1){
    render(CHAR_N,CHAR_O,CHAR_SPC,CHAR_SPC,0);
    _delay_ms(500);
    main();
    return;
  }
  for(i = 0;i <= (end - init); i++){
    print_loading(i);
    _delay_ms(12);
    eeprom_update_byte((uint8_t *) (init + i),0x00);
    eeprom_busy_wait();
  }
  render(CHAR_D,CHAR_O,CHAR_N,CHAR_E,0);
  _delay_ms(500);
  main();
}







