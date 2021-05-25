#include "processor.h"

//the 4 registers (ABCD)
static uint8_t registers[4] = {0};
static uint8_t stack[MAX_STACK] = {0};
static uint8_t ram[MAX_RAM] = {0};

static uint8_t next_instruction = 0x00;
static uint8_t flags = 0x00;
static uint8_t error = E_NO_ERROR;

//STATIC FUNCTIONS
static uint8_t _read_argument(uint8_t type, uint8_t argument){
  switch(type){
    case 0x01: //argument is an RAM Address
      if(argument<MAX_RAM)
        return ram[argument];
      else
        error = E_SEG_FAULT;
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

static void _execute_op3(uint8_t instruction, uint8_t argument){
  uint8_t selected_register = registers[instruction&0x03];
  uint8_t parsed_argument = _read_argument((instruction>>6)&0x03,argument); 
  if(error) return;
  //Choose the matching action!
  switch((instruction>>2)&0x07){
    case 0x00: //STORE
    case 0x02: //MOVE
      registers[selected_register] = parsed_argument;
      break;
    case 0x01: //LOAD
      ram[argument] = registers[selected_register];
      break;
    case 0x03: //INCREMENT
      registers[selected_register]++;
      break;
    case 0x04: //DECREMENT
      registers[selected_register]--;
      break;
    case 0x07: //COMPARE
      if(registers[selected_register] == parsed_argument) 
        flags = 0x01; //set Equal flag
      break;
    default:
      error = E_OP_UNDEF;
      break;
  }
}

static void _execute_op5(uint8_t instruction, uint8_t argument){
  uint8_t parsed_argument = _read_argument((instruction>>6)&0x03,argument); 
  if(error) return;
  //Choose the matching action!
  switch(instruction&0x1F){
    case 0x00: //NOOP
      //do nothing
      break;
    case 0x01: //ADD
      registers[0] += parsed_argument; 
      break;
    case 0x02: //SUB
      registers[0] -= parsed_argument; 
      break;
    case 0x03: //MUL
      registers[0] *= parsed_argument; 
      break;
    case 0x04: //DIV
      registers[0] /= parsed_argument; 
      break;
    case 0x07: //NOT
      registers[0] = ~registers[0]; 
      break;
    case 0x08: //AND
      registers[0] = registers[0] & parsed_argument; 
      break;
    case 0x09: //OR
      registers[0] = registers[0] | parsed_argument; 
      break;
    case 0x0A: //XOR
      registers[0] = registers[0] ^ parsed_argument; 
      break;
    case 0x0B: //JMP
      next_instruction = parsed_argument;
      break;
    case 0x14: //IN
      //TODO
      break;
    default:
      error = E_OP_UNDEF;
      break;
  }
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
  next_instruction++;
  error = E_NO_ERROR;
  if( (instruction>>5)&0x01 == 0 ){  //op3
    _execute_op3(instruction,argument);
  }else{ //op5
    _execute_op5(instruction,argument);
  }
  return error;
}
