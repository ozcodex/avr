#include <stdio.h>
#include <stdlib.h>

int main(){
  FILE *file;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int count, total, n, i,j,k, mode, max = 0;
  int nums[100] = {0};

  file = fopen("dice_rolls.txt","r");
  if (file == NULL) exit(EXIT_FAILURE);

  while((read = getline(&line, &len, file))!=-1){
    n = atoi(line);
    nums[n]++;
    total += n;
    count++;
  }
  for (i=0; i<100; i++){
    if (nums[i]>max){
      mode = i;
      max = nums[i];
    } 
  }

  printf("Rolls: %d\n",count);
  printf("Total: %d\n",total);
  printf("Mean: %d\n",(total/count));
  printf("Mode: %d",mode);
  for(i=max; i>=0; i--){
    k = 0;
    for(j=0;j<100;j++) if (nums[j] == i) k++;
    printf("\n%d Numbers have apeared %d times",k,i);
  }
  printf("\nNumbers that didn't appear: ");
  for (i=0; i<100; i++){
    if (nums[i] == 0){
      printf("%d ",i);
    } 
  }
  printf("\n");
}
