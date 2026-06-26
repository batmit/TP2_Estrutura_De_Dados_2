#include "heap.h"

void HeapSort(Registro *v, int n) {
    HeapConstroi(v, n);
    int r = n - 1;
    while (r > 0) {
        Registro aux = v[0];
        v[0] = v[r];
        v[r] = aux;
        r--;
        HeapRefaz(v, 0, r);
    }
}

void HeapConstroi(Registro *v, int n) {
    int left = n / 2 - 1;
    while (left >= 0) {
        HeapRefaz(v, left, n - 1);
        left--;
    }
}

void HeapRefaz(Registro *v, int l, int r) {
    Registro aux = v[l];
    int i = l;
    int j = i * 2 + 1;
    while (j <= r) {
        if (j < r && RegistroCompara(v[j], v[j + 1]) == MENOR)
            j++;
        COMP resp = RegistroCompara(aux, v[j]);
        if (resp == MAIOR || resp == IGUAL)
            break;
        v[i] = v[j];
        i = j;
        j = i * 2 + 1;
    }
    v[i] = aux;
}

/* Sobe o nó na posição 'i' enquanto for menor que o pai */
void heapSobe(NoHeap *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (RegistroCompara(h[i].reg, h[pai].reg) == MENOR) {
            NoHeap tmp = h[i]; h[i] = h[pai]; h[pai] = tmp;
            i = pai;
        } else break;
    }
}

/* Desce o nó na posição 'i' dentro de um heap de tamanho 'n' */
void heapDesce(NoHeap *h, int i, int n) {
    while (1) {
        int menor = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        if (esq < n && RegistroCompara(h[esq].reg, h[menor].reg) == MENOR) 
            menor = esq;
        if (dir < n && RegistroCompara(h[dir].reg, h[menor].reg) == MENOR) 
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
int heapInsere(NoHeap *h, int n, NoHeap no) {
    h[n] = no;
    heapSobe(h, n);
    return n + 1;
}

/* Remove a raiz (mínimo) e retorna o nó extraído (retorna novo tamanho via *n) */
NoHeap heapRemove(NoHeap *h, int *n) {
    NoHeap raiz = h[0];
    (*n)--;
    if (*n > 0) {
        h[0] = h[*n];
        heapDesce(h, 0, *n);
    }
    return raiz;
}