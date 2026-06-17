#ifndef INTERCALACAO_H
#define INTERCALACAO_H
#define FITAS_ENTRADA 20
#include "registro.h"
#include <math.h>

#ifdef _WIN32
    #include <direct.h>
    #define criarPasta(nome) _mkdir(nome)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define criarPasta(nome) mkdir(nome, 0777)
#endif
void HeapSort(Registro *v, int n);
void HeapConstroi(Registro *v, int n);
void HeapRefaz(Registro *v, int l, int r);
int ChamarCriadorFitas();
void fecharFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]);
bool criarFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]);
void abrirFitasSaida(FILE* fitasSaida[FITAS_ENTRADA], int n);
void abrirFitasEntrada(FILE* fitasEntrada[FITAS_ENTRADA], int n);
int indiceMenorRegistro(Registro v[], int n, bool ativos[20]);
int contarRegistrosTxt(const char* nomeArquivo);

void fecharFitasIntercalacao(FILE* fitas[FITAS_ENTRADA]);

//int lerRegistro(FILE *arquivo, Registro *r);
void IntercalacaoBalanceada();

void trimFim(char* str);


#endif