#include "intercalacaoSub.h"
#include "intercalacao.h"

COMP ItemCompara(Item r1, Item r2) {
    if (r1.marcado && !r2.marcado)
        return MAIOR;
    else if (!r1.marcado && r2.marcado)
        return MENOR;
    else
        return RegistroCompara(r1.reg, r2.reg);
}

bool novoBloco(Item *v, int  n){
    
    bool novo = true;

    for(int i = 0; i < n; i++){
        novo = novo && v[i].marcado;
    }

    if(novo)
        desmarcaElementos(v, n);
    
    return novo;
}

void desmarcaElementos(Item* v, int n){
    for(int i = 0; i < n; i++)
        v[i].marcado = false;
}

void transformaEmItem(Registro r, Item* i){
    i->reg = r;
    i->marcado = false;
}

void transformaEmRegistro(Item i, Registro* r){
    *r = i.reg;
}

bool leItem(FILE* arquivo, Item* reg){
    Registro r;
    if (leRegistro(arquivo, &r)) {
        transformaEmItem(r, reg);
        return true;
    }
    return false;
}

Item heapRemoveSub(Item *h, int *n){
    Item raiz = h[0];
    (*n)--;
    if (*n > 0) {
        h[0] = h[*n];
        HeapRefazSub(h, 0, *n - 1);
    }
    return raiz;
}

void HeapConstroiSub(Item *v, int n) {
    int left = n / 2 - 1;//inicia a construção da heap a partir do último nó pai
    while (left >= 0) {
        HeapRefazSub(v, left, n - 1);
        left--;
    }
}

void HeapRefazSub(Item *v, int l, int r) {
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
    v[i] = aux;
}

int geraBlocosSubstituicao(FILE* arquivo, int quantidade, Dados *dados) {
    int blocosGerados = 0;
    
    ChamarCriadorFitas();

    //Geração dos blocos iniciais via HeapSort

    Item itens[TAM_MEMORIA];
    int tamHeap = 0;
    char nomeArquivo[100];

    Item menorItem;

    //curinga indica o fim de um bloco de registro
    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;
    curinga.cidade[0] = '\0';
    curinga.curso[0] = '\0';
    curinga.estado[0] = '\0';

    Item novoItem;
    leItem(arquivo, &novoItem);

    // a variável i representa a posiçao no vetor em memória principal
    int i = 0;

    //a variável w representa em qual fita de entrada está sendo escrita
    int w = 1;

    int contador = 1;

    do {
        itens[i] = novoItem;

        if (i == TAM_MEMORIA - 1) {

            sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);
            FILE *fita = fopen(nomeArquivo, "ab");

            if(novoBloco(itens, TAM_MEMORIA)){
                fwrite(&curinga, sizeof(Item), 1, fita);
                blocosGerados++;
                w++;
            }
            
            if (w == FITAS_ENTRADA + 1)
                w = 1;

            HeapRefazSub(itens, 0, TAM_MEMORIA);

            menorItem = heapRemoveSub(itens, &tamHeap);

            Registro r;
            transformaEmRegistro(menorItem, &r);
            fwrite(&r, sizeof(Registro), 1, fita);

            i--;
            fclose(fita);
        }
        else{
            i++;
        }

        contador++;
        leItem(arquivo, &novoItem);
        
        if(ItemCompara(novoItem, menorItem) == MENOR)
            novoItem.marcado = true;
        
    } while (contador < quantidade);


    return blocosGerados;
}
