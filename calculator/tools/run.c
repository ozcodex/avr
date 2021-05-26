#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include"../lib/processor.h"

int main(){
  char inst,result = 0x00;
  char code[] = {
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
  0xEB, 0x12,  //1B:JMP .start
  0x00         //1D.end: NOP
  };
  printf("Hello!\n");
  printf("Running Program...\n");
  do{
    inst = get_next_instruction();
    if( check_params(inst)){
      result = execute(inst,0x00);
    }else{
      result = execute(inst,0x00);
    }
    if (result != E_NO_ERROR){
      printf("Error!");
      return result;
   }
  }while(inst != 0);
  printf("Executed Successfully");
}
