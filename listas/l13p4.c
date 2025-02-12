#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* fc(char texto[]) {
    int *f = (int *)malloc(26 * sizeof(int));
    if (f == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    for (int i = 0; texto[i] != '\0'; i++) {
        f[texto[i] - 'a']++;
    }

    return f;
}

int main() {
    char t[] = "ola mundo isso e um teste para lista";
    int *f = fc(t);

    for (int i = 0; i < 26; i++) {
        printf("%c: %d\n", 'a' + i, f[i]);
    }

    free(f);
    return 0;
}
