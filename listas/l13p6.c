#include <stdio.h>
#include <stdlib.h>

#define NENHUM 0
#define ESTADUAL 1
#define FEDERAL 2

typedef struct {
    int tipo;
    float distancia;
} Rodovia;

// a) Criação da matriz de adjacência
Rodovia** criaMatriz(int n) {
    Rodovia **M = (Rodovia **)malloc(n * sizeof(Rodovia *));
    if (M == NULL) {
        perror("Erro ao alocar memória para as linhas da matriz");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        M[i] = (Rodovia *)malloc(n * sizeof(Rodovia));
        if (M[i] == NULL) {
            perror("Erro ao alocar memória para as colunas da matriz");
            for (int j = 0; j < i; j++) {
                free(M[j]);
            }
            free(M);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < n; j++) {
            M[i][j].tipo = NENHUM;
            M[i][j].distancia = 0.0;
        }
    }
    return M;
}

// b) Cálculo da quilometragem total por tipo de rodovia
float quilometragemTipoTotal(Rodovia **M, int n, int tipo) {
    float total = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) { // Considera apenas metade da matriz (simétrica)
            if (M[i][j].tipo == tipo) {
                total += M[i][j].distancia;
            }
        }
    }
    return total;
}

// c) Desalocação da matriz
void desalocaMatriz(Rodovia **M, int n) {
    for (int i = 0; i < n; i++) {
        free(M[i]);
    }
    free(M);
}

int main() {
    int n, tipo, linha, coluna;
    float distancia;

    // Leitura do número de cidades
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Erro ao ler o número de cidades.\n");
        return EXIT_FAILURE;
    }

    Rodovia **M = criaMatriz(n);

    // Leitura das conexões entre as cidades
    while (scanf("%d,%f,%d,%d", &tipo, &distancia, &linha, &coluna) == 4) {
        M[linha][coluna].tipo = tipo;
        M[linha][coluna].distancia = distancia;
        M[coluna][linha] = M[linha][coluna]; // Garantir simetria
    }

    // Cálculo da quilometragem total para rodovias federais
    float total_federal = quilometragemTipoTotal(M, n, FEDERAL);
    printf("Quilometragem total de rodovias FEDERAL: %.1f km\n", total_federal);

    // Desalocação da matriz
    desalocaMatriz(M, n);

    return 0;
}
