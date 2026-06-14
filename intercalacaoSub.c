#include "intercalacaoSub.h"

COMP ItemCompara(Item r1, Item r2) {
    if (r1.marcado && !r2.marcado)
        return MAIOR;
    else if (!r1.marcado && r2.marcado)
        return MENOR;
    else
        return RegistroCompara(r1.reg, r2.reg);
}

void HeapConstroi(Item *v, int n) {
    int left = n / 2 - 1;//inicia a construção da heap a partir do último nó pai
    while (left >= 0) {
        HeapRefaz(v, left, n - 1);
        left--;
    }
}

void HeapRefaz(Item *v, int l, int r) {
    Item aux = v[l];
    int i = l;
    int j = i * 2 + 1;
    while (j <= r) {
        if (j < r && ItemCompara(v[j], v[j + 1]) == MENOR)
            j++;
        COMP resp = ItemCompara(aux, v[j]);
        if (resp == MAIOR || resp == IGUAL)
            break;
        v[i] = v[j];
        i = j;//desce o elemento aux para a posição correta na heap
        j = i * 2 + 1;//novo filho esquerdo
    }
    v[i] = aux;//
}