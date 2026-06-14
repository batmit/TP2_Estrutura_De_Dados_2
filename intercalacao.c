#include "intercalacao.h"

void HeapConstroi(Registro *v, int n) {
    int left = n / 2 - 1;//inicia a construção da heap a partir do último nó pai
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
        i = j;//desce o elemento aux para a posição correta na heap
        j = i * 2 + 1;//novo filho esquerdo
    }
    v[i] = aux;//
}