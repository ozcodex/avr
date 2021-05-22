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

#Randomness
A random seed is stored in the first two bytes (0x00 and 0x01) of the EEPROM,
after each reset or turn on, a new seed is generated.

Trere are also a file called check rolls (and script in c) and dice rolls (a
text file) that are used to check the randomness of the dice throws of the 
device, in the text file are a lot of d100 rolls, and the script takes those
numbers to give a statistical ond frecuency analysis of the throws.

#Commands
