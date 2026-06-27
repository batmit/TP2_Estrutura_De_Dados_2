#ifndef INTERCALACAO_H
#define INTERCALACAO_H
#include "registro.h"
#include "fitas.h"
#include "heap.h"
#include "intercalacaoSub.h"

#include <math.h>
#include<stdbool.h>
#ifdef _WIN32
    #include <direct.h>
    #define criarPasta(nome) _mkdir(nome)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define criarPasta(nome) mkdir(nome, 0777)
#endif

// int ChamarCriadorFitas();
// void fecharFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]);
// bool criarFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]);
void intercalaFitas(FILE *entrada[], int quantFitas, char *prefixoSaida, int numSaida, Dados *dados);
void abrirFitasSaida(FILE* fitasSaida[FITAS_ENTRADA], int n);
void abrirFitasEntrada(FILE* fitasEntrada[FITAS_ENTRADA], int n);
int indiceMenorRegistro(Registro v[], int n, bool ativos[20]);
int contarRegistrosTxt(const char* nomeArquivo);

void fecharFitasIntercalacao(FILE* fitas[FITAS_ENTRADA]);

int geraBlocosHeap(FILE* arquivo, int quantidade, Dados *dados);

//int lerRegistro(FILE *arquivo, Registro *r);
void IntercalacaoBalanceada(FILE* arquivo, int quantidade, int situacao, int metodo, Dados *dados);

void trimFim(char* str);

double calculaP(int n);

#endif
