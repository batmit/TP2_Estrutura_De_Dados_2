#include "quicksort.h"
#include <string.h>
void QuickSortExterno(FILE **Li, FILE **Ei, FILE **LEs, double Esq, double Dir, Dados *dados){
    double i, j;
    Registro Area[TAM_MEMORIA];

    if(Dir - Esq < 1)
        return;

    FAVazia(Area); // O vetor já é um ponteiro
    Particao(Li, Ei, LEs, Area, Esq, Dir, &i, &j, dados);

    if (i - Esq < Dir - j) {
        QuickSortExterno(Li, Ei, LEs, Esq, i, dados);
        QuickSortExterno(Li, Ei, LEs, j, Dir, dados);
    } else {
        QuickSortExterno(Li, Ei, LEs, j, Dir, dados);
        QuickSortExterno(Li, Ei, LEs, Esq, i, dados);
    }
}

void FAVazia(Registro* Area){
    memset(Area, 0, sizeof(Registro) * TAM_MEMORIA);
}

void LeSup(FILE** ArqLEs, Registro* UltLido, double *Ls, bool *OndeLer){
    fseek(*ArqLEs, (long)((*Ls - 1) * sizeof(Registro)), SEEK_SET);
    fread(UltLido, sizeof(Registro), 1, *ArqLEs);
    (*Ls)--;
    *OndeLer = false;
}

void LeInf(FILE** ArqLi, Registro* UltLido, double* Li, bool *OndeLer){
    fread(UltLido, sizeof(Registro), 1, *ArqLi);
    (*Li)++;
    *OndeLer = true;
}

void InserirArea(Registro* Area, Registro* UltLido, int *NRArea){
    int i = *NRArea - 1;
    
    // Busca a posição correta para manter a ordenação interna
    while (i >= 0 && Area[i].nota > UltLido->nota) {
        i--;
    }
    
    // Desloca TODOS os elementos maiores de uma só vez usando o memmove
    if (i + 1 < *NRArea) {
        memmove(&Area[i + 2], &Area[i + 1], (*NRArea - (i + 1)) * sizeof(Registro));
    }
    
    Area[i + 1] = *UltLido;
    (*NRArea)++; // Apenas incrementa a variável já rastreada (elimina o O(N) de contagem)
}

void EscreveMax(FILE **ArqLEs, Registro R, double *Es){
    fseek(*ArqLEs, (long)((*Es - 1) * sizeof(Registro)), SEEK_SET);
    fwrite(&R, sizeof(Registro), 1, *ArqLEs);
    (*Es)--;
}

void EscreveMin(FILE **ArqEi, Registro R, double *Ei){
    fwrite(&R, sizeof(Registro), 1, *ArqEi);
    (*Ei)++;
}

void RetiraMax(Registro* Area, Registro* R, int *NRArea){
    if (*NRArea == 0)
        return;
    
    *R = Area[*NRArea - 1]; // O máximo é sempre o último elemento atual
    
    (*NRArea)--; // Apenas decrementa a variável
}

void RetiraMin(Registro* Area, Registro* R, int *NRArea){
    if (*NRArea == 0)
        return;
    
    *R = Area[0]; // O mínimo é sempre o primeiro elemento
    (*NRArea)--; 
    
    // Puxa TODOS os elementos restantes para a esquerda de uma só vez
    if (*NRArea > 0) 
        memmove(&Area[0], &Area[1], (*NRArea) * sizeof(Registro));
    
    // Limpa a última célula restante
    memset(&Area[*NRArea], 0, sizeof(Registro));
}

void Particao(FILE **ArqLi, FILE **ArqEi, FILE **ArqLEs, Registro* Area, double Esq, double Dir, double *i, double *j, Dados *dados){
    int NRArea = 0;
    double Li = Esq;
    double Ls = Dir;
    double Ei = Esq;
    double Es = Dir;
    int Linf = INT_MIN;
    int Lsup = INT_MAX;

    bool OndeLer = true;
    Registro UltLido, R;

    fseek(*ArqLi, (long)((Li - 1) * sizeof(Registro)), SEEK_SET);
    fseek(*ArqEi, (long)((Ei - 1) * sizeof(Registro)), SEEK_SET);
    *i = Esq - 1;
    *j = Dir + 1;

    while(Ls >= Li){
        dados->comparacoes++;
        if(NRArea < TAM_MEMORIA - 1){
            dados->transferencias.leituras++;
            if(OndeLer)
                LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
            else
                LeInf(ArqLi, &UltLido, &Li, &OndeLer);
            InserirArea(Area, &UltLido, &NRArea);
            continue;
        }
        dados->transferencias.leituras++;
        dados->comparacoes++;
        if(Ls == Es)
            LeSup (ArqLEs, &UltLido, &Ls, &OndeLer);
        else if(Li == Ei)
            LeInf (ArqLi, &UltLido, &Li, &OndeLer);
        else if(OndeLer)
            LeSup (ArqLEs, &UltLido, &Ls, &OndeLer);
        else
            LeInf (ArqLi, &UltLido, &Li, &OndeLer);


        dados->comparacoes++;
        dados->transferencias.escritas++;
        if(UltLido.nota > Lsup){
            *j = Es;
            EscreveMax(ArqLEs, UltLido, &Es);
            continue;
        }
        if(UltLido.nota < Linf){
            *i = Ei;
            EscreveMin(ArqEi, UltLido, &Ei);
            continue;
        }
        InserirArea(Area, &UltLido, &NRArea);

        if(Ei - Esq < Dir - Es){
            RetiraMin(Area, &R, &NRArea);
            EscreveMin(ArqEi, R, &Ei);
            Linf = R.nota;
        } else {
            RetiraMax(Area, &R, &NRArea);
            EscreveMax(ArqLEs, R, &Es);
            Lsup = R.nota;
        }
    }

    while(Ei <= Es){
        dados->transferencias.escritas++;
        RetiraMin(Area, &R, &NRArea);
        EscreveMin(ArqEi, R, &Ei);
    }
}
