#include <stdio.h>
#include <stdlib.h>

#include "registro.h"
#include "intercalacao.h"
#include "quicksort.h"

int main(int argc, char *argv[]){//pesquisa <método> <quantidade> <situação> [-P]

    if(argc != 4 && argc != 5){
        printf("Sintaxe: %s <método> <quantidade> <situação> [-P]\n", argv[0]);
        return 1;
    }

    Dados dados;
    dados.comparacoes = 0;
    dados.transferencias.leituras = 0;
    dados.transferencias.escritas = 0;

    clock_t inicio, fim;

    FILE* base = fopen("PROVAO.txt", "r");
    if (!base) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    FILE* arquivoBinario = criaArquivoBinario(base, "PROVAO.bin");
    if (!arquivoBinario) {
        printf("Erro ao criar o arquivo binário.\n");
        return 1;
    }

    if(atoi(argv[1]) == 1){//Intercalação Balanceada de 2f caminhos
        inicio = clock();
        IntercalacaoBalanceada();
        fim = clock();
    }
    else if(atoi(argv[1]) == 2){//Intercalação Balanceada de 2f caminhos com substituição por seleção
        inicio = clock();
        //IntercalacaoBalanceadaSubstituicao();
        fim = clock();
    }
    else if(atoi(argv[1]) == 3){//QuickSort Externo
        inicio = clock();
        //inicia os apontadores para o arquivo
        FILE *Li = fopen("PROVAO.bin", "r+b");
        if(!Li) {
            printf("Erro ao abrir o arquivo binário para leitura e escrita.\n");
            return 1;
        }
        FILE *Ei = fopen("PROVAO.bin", "r+b");
        if(!Ei) {
            printf("Erro ao criar o arquivo Ei.bin.\n");
            return 1;
        }
        FILE* LEs = fopen("PROVAO.bin", "r+b");
        if(!LEs) {
            printf("Erro ao criar o arquivo LEs.bin.\n");
            return 1;
        }
        QuickSortExterno(&Li, &Ei, &LEs, 1, atoi(argv[2]), &dados);
        fim = clock();
    }
    else{
        printf("Metodo de busca desconhecido: %s\n", argv[1]);
        return 1;
    }

    if(argc == 5 && strcmp(argv[4], "-P") == 0) {//
        imprimirRegistros(arquivoBinario);
    }

    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
     //calcula o tempo gasto na busca
    printf("Comparacoes na ordenação: %d\n", dados.comparacoes);
    printf("Tempo: %lf s\n", tempo);
    printf("Transferencias na leitura: %d\n", dados.transferencias.leituras);
    printf("Transferencias na escrita: %d\n", dados.transferencias.escritas);

    printf("----------------------------\n");


    return 0;
}
