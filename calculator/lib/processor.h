#ifndef _PROCESSOR_
#define	_PROCESSOR_

#define MAX_STACK 16

//get the address of the next instruction
uint8_t next_instruction();

//check if the given instruction needs or not a parameter
uint8_t check_params(uint8_t instruction);


uint16_t random(void);

#endif

