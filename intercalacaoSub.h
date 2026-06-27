#ifndef INTERCALACAOSUB_H
#define INTERCALACAOSUB_H

#include "registro.h"
#include "heap.h"

typedef struct {
    Registro reg;
    bool marcado;//valor marcado é sempre maior que o valor não marcado, valor marcado entra no proximo bloco
} Item;

void HeapConstroiSub(Item *v, int n, Dados *dados);
void HeapRefazSub(Item *v, int l, int r, Dados *dados);
Item HeapRemoveSub(Item *h, int *n, Dados *dados);

void HeapInsereSub(Item *h, Item novo);
void HeapSobeSub(Item *h, int i);

COMP ItemCompara(Item r1, Item r2, Dados *dados);

bool novoBloco(Item *v, int  n);
void desmarcaElementos(Item* v, int n);
void transformaEmItem(Registro r, Item* i);
void transformaEmRegistro(Item i, Registro* r);

int geraBlocosSubstituicao(FILE* arquivo, int quantidade, Dados *dados);

bool leItem(FILE* arquivo, Item* reg, Dados *dados);

#endif
