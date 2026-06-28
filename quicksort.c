#include "quicksort.h"

void QuickSortExterno(FILE **Li, FILE **Ei, FILE **LEs, int Esq, int Dir, Dados *dados){
    int i, j;
    Registro Area[TAM_MEMORIA];

    if(Dir - Esq < 1)
        return;

    FAVazia(Area); // O vetor já é um pointeiro
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
    for(int i = 0; i < TAM_MEMORIA; i++){
        Area[i].numero = 0;
        Area[i].nota = 0.0;
        strcpy(Area[i].estado, "");
        strcpy(Area[i].cidade, "");
        strcpy(Area[i].curso, "");
    }
}

void LeSup(FILE** ArqLEs, Registro* UltLido, int *Ls, bool *OndeLer){
    fseek(*ArqLEs, (*Ls - 1) * sizeof(Registro), SEEK_SET);
    fread(UltLido, sizeof(Registro), 1, *ArqLEs);
    (*Ls)--;
    *OndeLer = false;
}

void LeInf(FILE** ArqLi, Registro* UltLido, int* Li, bool *OndeLer){
    fread(UltLido, sizeof(Registro), 1, *ArqLi);
    (*Li)++;
    *OndeLer = true;
}

void InserirArea(Registro* Area, Registro* UltLido, int *NRArea){
    InsereItem(*UltLido, Area);
    *NRArea = ObterNumCelOcupadas(Area);
}

void EscreveMax(FILE **ArqLEs, Registro R, int *Es){
    fseek(*ArqLEs, (*Es - 1) * sizeof(Registro), SEEK_SET);
    fwrite(&R, sizeof(Registro), 1, *ArqLEs);
    (*Es)--;
}

void EscreveMin(FILE **ArqEi, Registro R, int *Ei){
    fwrite(&R, sizeof(Registro), 1, *ArqEi);
    (*Ei)++;
}

void RetiraMax(Registro* Area, Registro* R, int *NRArea){
    RetiraUltimo(Area, R);
    *NRArea = ObterNumCelOcupadas(Area);
}

void RetiraMin(Registro* Area, Registro* R, int *NRArea){
    RetiraPrimeiro(Area, R);
    *NRArea = ObterNumCelOcupadas(Area);
}

void Particao(FILE **ArqLi, FILE **ArqEi, FILE **ArqLEs, Registro* Area, int Esq, int Dir, int *i, int *j, Dados *dados){
    int NRArea = 0;
    int Li = Esq;
    int Ls = Dir;
    int Ei = Esq;
    int Es = Dir;
    int Linf = INT_MIN;
    int Lsup = INT_MAX;

    bool OndeLer = true;
    Registro UltLido, R;

    fseek(*ArqLi, (Li - 1) * sizeof(Registro), SEEK_SET);
    fseek(*ArqEi, (Ei - 1) * sizeof(Registro), SEEK_SET);
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
