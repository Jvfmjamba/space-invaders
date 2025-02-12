#include <stdio.h>
#include <stdlib.h>

int main() { 
  int *x = (int*)malloc(sizeof(int));
  int *y = x; 
  free(x); 
  //free(y);  
  return 0;
}
//l.8 -> a variavel y estava sendo desalocada, mas ela nunca havia sido alocada