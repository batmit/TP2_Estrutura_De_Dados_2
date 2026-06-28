#include "heap.h"

void HeapSort(Registro *v, int n, Dados *dados) {
    HeapConstroi(v, n, dados);
    int r = n - 1;
    while (r > 0) {
        Registro aux = v[0];
        v[0] = v[r];
        v[r] = aux;
        r--;
        HeapRefaz(v, 0, r, dados);
    }
}

void HeapConstroi(Registro *v, int n, Dados *dados) {
    int left = n / 2 - 1;
    while (left >= 0) {
        HeapRefaz(v, left, n - 1, dados);
        left--;
    }
}

void HeapRefaz(Registro *v, int l, int r, Dados *dados) {
    Registro aux = v[l];
    int i = l;
    int j = i * 2 + 1;
    while (j <= r) {
        if (j < r && RegistroCompara(v[j], v[j + 1], dados) == MENOR)
            j++;
        COMP resp = RegistroCompara(aux, v[j], dados);
        if (resp == MAIOR || resp == IGUAL)
            break;
        v[i] = v[j];
        i = j;
        j = i * 2 + 1;
    }
    v[i] = aux;
}

/* Sobe o nó na posição 'i' enquanto for menor que o pai */
void heapSobe(NoHeap *h, int i, Dados *dados) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (RegistroCompara(h[i].reg, h[pai].reg, dados) == MENOR) {
            NoHeap tmp = h[i]; h[i] = h[pai]; h[pai] = tmp;
            i = pai;
        } else break;
    }
}

/* Desce o nó na posição 'i' dentro de um heap de tamanho 'n' */
void heapDesce(NoHeap *h, int i, int n, Dados *dados) {
    while (1) {
        int menor = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        if (esq < n && RegistroCompara(h[esq].reg, h[menor].reg, dados) == MENOR)
            menor = esq;
        if (dir < n && RegistroCompara(h[dir].reg, h[menor].reg, dados) == MENOR)
            menor = dir;
        if (menor == i)
            break;
        NoHeap tmp = h[i];
        h[i] = h[menor];
        h[menor] = tmp;
        i = menor;
    }
}

/* Insere um nó na heap (retorna novo tamanho) */
int heapInsere(NoHeap *h, int n, NoHeap no, Dados *dados) {
    h[n] = no;
    heapSobe(h, n, dados);
    return n + 1;
}

/* Remove a raiz (mínimo) e retorna o nó extraído (retorna novo tamanho via *n) */
NoHeap heapRemove(NoHeap *h, int *n, Dados *dados) {
    NoHeap raiz = h[0];
    (*n)--;
    if (*n > 0) {
        h[0] = h[*n];
        heapDesce(h, 0, *n, dados);
    }
    return raiz;
}