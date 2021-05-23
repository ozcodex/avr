#include "processor.h"

//the 4 registers (ABCD)
static uint8_t reg[4] = {0};
static uint8_t stack[MAX_STACK] = {0};

static uint8_t next_instruction = {0};




