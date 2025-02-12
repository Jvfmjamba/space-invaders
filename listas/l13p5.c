#include <stdio.h>
#include <stdlib.h>

int** duplica_matriz(int **m, int num_l, int num_c) {
    int **matriz_copia = (int **)malloc(num_l * sizeof(int *));
    for (int i = 0; i < num_l; i++) {
        matriz_copia[i] = (int *)malloc(num_c * sizeof(int));
    }
    for (int i = 0; i < num_l; i++) {
        for (int j = 0; j < num_c; j++) {
            matriz_copia[i][j] = m[i][j];
        }
    }
    return matriz_copia;
}

int main() {
    int num_l = 3, num_c = 3;
    int **matriz = (int **)malloc(num_l * sizeof(int *));
    for (int i = 0; i < num_l; i++) {
        matriz[i] = (int *)malloc(num_c * sizeof(int));
    }

    for (int i = 0; i < num_l; i++) {
        for (int j = 0; j < num_c; j++) {
            matriz[i][j] = i * num_c + j + 1;
        }
    }

    int **matriz_copia = duplica_matriz(matriz, num_l, num_c);

    for (int i = 0; i < num_l; i++) {
        for (int j = 0; j < num_c; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }

    printf("\n");

    for (int i = 0; i < num_l; i++) {
        for (int j = 0; j < num_c; j++) {
            printf("%d ", matriz_copia[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < num_l; i++) {
        free(matriz[i]);
    }
    free(matriz);

    for (int i = 0; i < num_l; i++) {
        free(matriz_copia[i]);
    }
    free(matriz_copia);

    return 0;
}
