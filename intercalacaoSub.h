#ifndef INTERCALACAOSUB_H
#define INTERCALACAOSUB_H

#include "registro.h"

typedef struct {
    Registro reg;
    bool marcado;//valor marcado é sempre maior que o valor não marcado, valor marcado entra no proximo bloco
} Item;

void HeapConstroi(Item *v, int n);
void HeapRefaz(Item *v, int l, int r);

COMP ItemCompara(Item r1, Item r2);

#endif