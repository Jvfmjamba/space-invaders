#include <stdio.h>

int main() {
    int N;

    while (scanf("%d", &N) != EOF) {
        if (N < 3 || N >= 70) {
            printf("N fora do intervalo válido.\n");
            continue;
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (i == 0) { // Primeira linha
                    if (j == 0) {
                        printf("1");
                    } else if (j == N - 1) {
                        printf("2");
                    } else {
                        printf("3");
                    }
                } else if (i == N - 1) { // Última linha
                    if (j == 0) {
                        printf("2");
                    } else if (j == N - 1) {
                        printf("1");
                    } else {
                        printf("3");
                    }
                } else if (i + j == N - 1) { // Diagonal secundária
                    printf("2");
                } else if (i == j) { // Diagonal principal
                    printf("1");
                } else { // Restante
                    printf("3");
                }
            }
            printf("\n");
        }
    }

    return 0;
}