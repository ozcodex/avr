#ifndef _PROCESSOR_
#define	_PROCESSOR_

#define MAX_STACK 16
#define MAX_RAM 16

//ERROR CODES
#define E_NO_ERROR 0x00
#define E_UNDEF_ERROR 0x01
#define E_SEG_FAULT 0x01

//get the address of the next instruction
uint8_t get_next_instruction();

//check if the given instruction needs or not a parameter
uint8_t check_params(uint8_t instruction);

//execute the given instruction, returns an error code
uint8_t execute(uint8_t instruction, uint8_t argument){

#endif

