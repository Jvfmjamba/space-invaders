#include <stdio.h>
#include <stdlib.h>

int *cria_inteiro(int n){
    int *p = malloc(sizeof(int));
    *p = n;
    printf("%d", *p);
    free(p);
}

int main(){
int x;
x = 5;
cria_inteiro(x);
    return 0;
}