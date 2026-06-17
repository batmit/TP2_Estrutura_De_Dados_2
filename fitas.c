
#include<stdio.h>
#include<stdlib.h>
#define FITAS_ENTRADA 20
#define FITAS_SAIDA 20
#define TOTAL_FITAS 40
#include "intercalacao.h"

bool criarFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]) {
    char nomeArquivo[100];

    criarPasta("fitas");

    for (int i = 0; i < FITAS_ENTRADA; i++) {
        sprintf(nomeArquivo, "fitas/entrada_%02d.bin", i + 1);

        entrada[i] = fopen(nomeArquivo, "w+b");

        if (entrada[i] == NULL) {
            printf("Erro ao criar a fita de entrada %s\n", nomeArquivo);

            for (int j = 0; j < i; j++) {
                fclose(entrada[j]);
            }

            return false;
        }
    }

    for (int i = 0; i < FITAS_SAIDA; i++) {
        sprintf(nomeArquivo, "fitas/saida_%02d.bin", i + 1);

        saida[i] = fopen(nomeArquivo, "w+b");

        if (saida[i] == NULL) {
            printf("Erro ao criar a fita de saida %s\n", nomeArquivo);

            for (int j = 0; j < FITAS_ENTRADA; j++) {
                fclose(entrada[j]);
            }

            for (int j = 0; j < i; j++) {
                fclose(saida[j]);
            }

            return false;
        }
    }

    return true;
}

void fecharFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]) {
    for (int i = 0; i < FITAS_ENTRADA; i++) {
        if (entrada[i] != NULL) {
            fclose(entrada[i]);
            entrada[i] = NULL;
        }
    }

    for (int i = 0; i < FITAS_SAIDA; i++) {
        if (saida[i] != NULL) {
            fclose(saida[i]);
            saida[i] = NULL;
        }
    }
}

void ChamarCriadorFitas() {
    FILE* entrada[FITAS_ENTRADA];
    FILE* saida[FITAS_SAIDA];

    if (!criarFitas(entrada, saida)) {
        printf("Erro ao criar as fitas.\n");
        return 1;
    }

    printf("Fitas criadas com sucesso dentro da pasta fitas.\n");

    fecharFitas(entrada, saida);
}