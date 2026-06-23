#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "registro.h"

void QuickSortExterno(FILE **Li, FILE **Ei, FILE **LEs, int Esq, int Dir, Dados *dados);
void FAVazia(Registro* Area);
void LeSup(FILE** ArqLEs, Registro* UltLido, int *Ls, bool *OndeLer);
void LeInf(FILE** ArqLi, Registro* UltLido, int* Li, bool *OndeLer);
void InserirArea(Registro* Area, Registro* UltLido, int *NRArea);
void EscreveMax(FILE **ArqLEs, Registro R, int *Es);
void EscreveMin(FILE **ArqEi, Registro R, int *Ei);
void RetiraMax(Registro* Area, Registro* R, int *NRArea);
void RetiraMin(Registro* Area, Registro* R, int *NRArea);
void Particao(FILE **ArqLi, FILE **ArqEi, FILE **ArqLEs, Registro* Area, int Esq, int Dir, int *i, int *j, Dados *dados);
// Funções que faltam
int ObterNumCelOcupadas(Registro* Area);
void InsereItem(Registro item, Registro* Area);
void RetiraPrimeiro(Registro* Area, Registro* R);
void RetiraUltimo(Registro* Area, Registro* R);

#endif
