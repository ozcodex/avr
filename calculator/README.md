# Calculator
A small "calculator" with a lot of different functions

# Functions
When the program is running allows to loop trough the options, showing in the
screen the letters "oP:" and a number. To turn on the device or return to menu
use the reset button (closest to screen).

The options (oP) of the menu are:

- 00: (oFF) Turns off the device. disables the screen and put the processor in
deep sleep mode.

- 01: (Add) Allow to make the addition in base 16 (Hexadecimal) of two numbers,
the maximum allowed input is 7F, to avoid overflow (7F + 7F = FF)

- 02: (PLAY) Rock-Paper-Scissors game, the classical game, you must choose one
option: Rock(ro), Paper(PA), Scissors(Si) to play, an the device will show its
option, and declare if you move was good, bad or a tie.

- 03: (diCE) Allows to throw different kind of dices (d4, d6, d8, d10, d12, d20,
d100), while the word roLL is on the screen you can press and kepp pressed any
button to roll many dices before you roll, this improves the randomness of the
throw.

- 04: (tALY) a tally counter, the central and left buttons increases and 
decreases the counter, and the right button resets it to zero.

- 05: (LEd) Changes the bright level and stores (Stor) it on the EEPROM in the
0x02 register, when finish writing shows done (donE) on the screen.

- 06: (EdIt) Read and shows the content of the second block of the EEPROM (the
registers from 0x0100 to 0x01FF) in the format AA:BB where AA is the MSB of the
address and BB is the content of the eeprom, with the central button the 
content of that position can be changed

- 07 (rUn) Execute the program defined through the EdIt command. see below for
details.

- 08 (CLr) Clear the EEPROM, this should be use only if is stricted necessary,
avoid it because it reduces the lifetime of the EEPROM, and also, is a very 
slow procces and shouldn't be interrupted.

#Randomness
A random seed is stored in the first two bytes (0x00 and 0x01) of the EEPROM,
after each reset or turn on, a new seed is generated.

Trere are also a file called check rolls (and script in c) and dice rolls (a
text file) that are used to check the randomness of the dice throws of the 
device, in the text file are a lot of d100 rolls, and the script takes those
numbers to give a statistical ond frecuency analysis of the throws.

#Commands
The programming commands are hex codes that represents a binary number. It has
this structure:

00 1 222 33

- 00 The first two bits indicates the type of arguments that the command 
accepts, can be 00 for no argumments, 01 for an address, 10 for a register
and 11 for a constant.
- 1 the next bit indicates the type of operation, can be op3 o op5, the op3
operations have a op code of 3 bits and use the last two bits as the register
number, op5 didn't have a register number and use all the next bits as op code.
- 222 op3 codes
- 22233 op5 codes
- 33 if op3 this bits indicates the regiter that this operation uses.

###Op3 Codes

####ST - Store
01 1 000 00 Address (ST A,[addr]) 0x60
01 1 000 01 Address (ST B,[addr]) 0x61
01 1 000 10 Address (ST C,[addr]) 0x62
01 1 000 11 Address (ST D,[addr]) 0x63
11 1 000 00 Constant (ST A,const) 0xE0
11 1 000 01 Constant (ST B,const) 0xE1
11 1 000 10 Constant (ST C,const) 0xE2
11 1 000 11 Constant (ST D,const) 0xE3

Copy the content of the address or the value given as constant to the 
destination register.

####LD - LOAD
01 1 001 00 Address (LD A,[addr]) 0x64
01 1 001 01 Address (LD B,[addr]) 0x65
01 1 001 10 Address (LD C,[addr]) 0x66
01 1 001 11 Address (LD D,[addr]) 0x67

Copy the content of the source register in the destination address.

####MOV - MOVE
10 1 010 00 Register (MOV A,D) 0xA8
10 1 010 01 Register (MOV B,C) 0xA9
10 1 010 10 Register (MOV C,B) 0xAA
10 1 010 11 Register (MOV D,A) 0xAB

Copy the content of the source register (the second parameter) into the 
destination register (the first one).
This is the only operation that moves data from a register to other.

####INC - INCREMENT
00 1 011 00 (INC A) 0x2C
00 1 011 01 (INC B) 0x2D
00 1 011 10 (INC C) 0x2E
00 1 011 11 (INC D) 0x2F

Increments the value of the given register by one, this function didn't change
the carry flag.

####DEC - DECREMENT
00 1 100 00 (DEC A) 0x30
00 1 100 01 (DEC B) 0x31
00 1 100 10 (DEC C) 0x32
00 1 100 11 (DEC D) 0x33

Decrements by one the value of the given register, this function didn't change
the carry flag.

####CMP - COMPARE
10 1 111 00 Register (CMP A,D)     0xBC
10 1 111 01 Register (CMP B,C)     0xBD
10 1 111 10 Register (CMP C,B)     0xBE
10 1 111 11 Register (CMP D,A)     0xBF
11 1 111 00 Constant (CMP A,const) 0xFC
11 1 111 01 Constant (CMP B,const) 0xFD
11 1 111 10 Constant (CMP C,const) 0xFE
11 1 111 11 Constant (CMP D,const) 0xFF

Compare two registers or a register and a constant, and changes the flags 
according to the result.

###Op5 Codes

####NOP - NO OPERATION
00 0 00000 (NOP) 0x00

Does noting.

####ADD - ADDITION
10 0 00001 Register (ADD B)     0x81
11 0 00001 Constant (ADD conts) 0xC1

Adds to the A register the value of the given register or constant. Modifies the
flags according to the result.


