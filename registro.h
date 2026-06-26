#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

#define TAM_MEMORIA 20 //quantidade de registros que cabem na memória principal

typedef struct registro{
    long numero;
    float nota;
    char estado[3];
    char cidade[51];
    char curso [31];
} Registro;

typedef enum {
    MENOR=0,
    IGUAL,
    MAIOR
} COMP;

typedef struct transferencia{
    int leituras;
    int escritas;
} Transferencia;

typedef struct dados{
    Transferencia transferencias;
    int comparacoes;
} Dados;


void imprimirRegistros(FILE* arquivo);
void RegistroPrint(Registro item);

bool leRegistro(FILE* arquivo, Registro* reg);

COMP RegistroCompara(Registro i1, Registro i2);

FILE* criaArquivoBinario(FILE* arquivoTexto, const char* nomeArquivoBinario, Dados* dados);
FILE* criaArquivoSaida(FILE* arquivoBinario, const char* nomeArquivoSaida);

#endif
