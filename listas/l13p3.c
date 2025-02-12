#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* duplica_string(char str[]){
    char *p = (char*)malloc((strlen(str) + 1) * sizeof(char));
    strcpy(p, str);
    return p;
}

int main(){
    char s[10] = "string1";
    char *duplicada = duplica_string(s);

      printf("%s", s);

    free(duplicada);
    return 0;
}