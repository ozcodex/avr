#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include"../lib/processor.h"

void print4digits7segments(
    unsigned char d1, 
    unsigned char d2, 
    unsigned char d3,
    unsigned char d4){
  /*  _  _  _  _
   * |_||_||_||_|
   * |_||_||_||_|
   */
  printf(" %c  %c  %c  %c \n",
      d1&1?'_':' ',
      d2&1?'_':' ',
      d3&1?'_':' ',
      d4&1?'_':' '
      );
  printf("%c%c%c%c%c%c%c%c%c%c%c%c\n",
      (d1>>5)&1?'|':' ',
      (d1>>6)&1?'_':' ',
      (d1>>1)&1?'|':' ',
      (d2>>5)&1?'|':' ',
      (d2>>6)&1?'_':' ',
      (d2>>1)&1?'|':' ',
      (d3>>5)&1?'|':' ',
      (d3>>6)&1?'_':' ',
      (d3>>1)&1?'|':' ',
      (d4>>5)&1?'|':' ',
      (d4>>6)&1?'_':' ',
      (d4>>1)&1?'|':' '
      );
  printf("%c%c%c%c%c%c%c%c%c%c%c%c\n",
      (d1>>4)&1?'|':' ',
      (d1>>3)&1?'_':' ',
      (d1>>2)&1?'|':' ',
      (d2>>4)&1?'|':' ',
      (d2>>3)&1?'_':' ',
      (d2>>2)&1?'|':' ',
      (d3>>4)&1?'|':' ',
      (d3>>3)&1?'_':' ',
      (d3>>2)&1?'|':' ',
      (d4>>4)&1?'|':' ',
      (d4>>3)&1?'_':' ',
      (d4>>2)&1?'|':' '
      );

}

int main(){
  unsigned char inst = 0x00;
  unsigned char result = 0x00;
  unsigned char code[] = {
  0xE0, 0x76,  //00:ST A, 0x76
  0x64, 0x00,  //02:LD A, 0x00
  0xE0, 0x3F,  //04:ST A, 0x3F
  0x64, 0x01,  //06:LD A, 0x01
  0xE0, 0x38,  //08:ST A, 0x38
  0x64, 0x02,  //0A:LD A, 0x02
  0xE0, 0x77,  //0C:ST A, 0x77
  0x64, 0x03,  //0E:LD A, 0x03
  0xE2, 0x00,  //10:ST C, 0X00
  0xFE, 0x04,  //12.start: CMP C, 0x04
  0xCC, 0x1D,  //14:JE .end
  0xAB, 0x02,  //16:MV D, [C]
  0x98, 0x02,  //18:OUT C
  0x2E,        //1A:INC C
  0xCB, 0x12,  //1B:JMP .start
  0x00         //1D.end: NOP
  };
  printf("Hello!\n");
  printf("Running Program...\n");
  
  while(code[inst] != 0x00){
    inst = get_next_instruction();
    printf("Executing instruction %3x: ",inst);
    if( check_params(code[inst])){
      printf("%2x %2x\n",code[inst],code[inst+1]);
      result = execute(code[inst],code[inst+1]);
    }else{
      printf("%2x\n",code[inst]);
      printf("Executing instruction %3x\n",inst);
      result = execute(code[inst],0x00);
    }
    if (result != E_NO_ERROR){
      printf("Error!");
      return result;
   }
  }
  printf("Display Output:\n");

  print4digits7segments(
      get_output(0),
      get_output(1),
      get_output(2),
      get_output(3));
  printf("\nExecuted Successfully!\n");
}
