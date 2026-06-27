#ifndef HEAP_H
#define HEAP_H
#include "registro.h"

void HeapSort(Registro *v, int n, Dados *dados);
void HeapConstroi(Registro *v, int n, Dados *dados);
void HeapRefaz(Registro *v, int l, int r, Dados *dados);

typedef struct noHeap{
    Registro reg;
    int indiceFita;
} NoHeap;


/* Sobe o nó na posição 'i' enquanto for menor que o pai */
void heapSobe(NoHeap *h, int i, Dados *dados);

/* Desce o nó na posição 'i' dentro de um heap de tamanho 'n' */
void heapDesce(NoHeap *h, int i, int n, Dados *dados);

/* Insere um nó na heap (retorna novo tamanho) */
int heapInsere(NoHeap *h, int n, NoHeap no, Dados *dados);

/* Remove a raiz (mínimo) e retorna o nó extraído (retorna novo tamanho via *n) */
NoHeap heapRemove(NoHeap *h, int *n, Dados *dados);

#endif
