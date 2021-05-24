#include "processor.h"

//the 4 registers (ABCD)
static uint8_t registers[4] = {0};
static uint8_t stack[MAX_STACK] = {0};
static uint8_t ram[MAX_RAM] = {0};

static uint8_t next_instruction = 0x00;
static uint8_t error = E_NO_ERROR;

//STATIC FUNCTIONS
static uint8_t _read_argument(uint8_t type, uint8_t argument){
  switch(type){
    case 0x01: //argument is an RAM Address
      if(argument<MAX_RAM)
        return ram[argument];
      else
        error = E_SEG_FAULT;
      //TODO
    break;
    case 0x02: //argument is a Register
      if(argument<4)
        return registers[argument];
      else
        error = E_SEG_FAULT;
    break;
    case 0x03: //argument is a Constant
      return argument;
    break;
  }
  return 0;
}

static uint8_t _execute_op3(uint8_t instruction, uint8_t argument){
  uint8_t selected_register = registers[instruction&0x03];
  _read_argument((instruction>>6)&0x03,argument); 
}

static uint8_t _execute_op3(){
  //TODO
}

//PUBLIC FUNCTIONS
uint8_t get_next_instruction(){
  return next_instruction;
}

uint8_t check_params(uint8_t instruction){
  //take the two LSB bytes of the instruction
  if( (instruction>>6)&0x03 == 0 ) return 0; //take no arguments
  else return 1; //it takes arguments
}

uint8_t execute(uint8_t instruction, uint8_t argument){
  if( (instruction>>5)&0x01 == 0 ){  //op3
    return _execute_op3(instruction,argument);
  }else{ //op5
    return _execute_op5(instruction,argument);
  }
}
