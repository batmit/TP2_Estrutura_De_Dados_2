#include "intercalacaoSub.h"
#include "intercalacao.h"

COMP ItemCompara(Item r1, Item r2, Dados *dados) {
    if (r1.marcado && !r2.marcado)
        return MAIOR;
    else if (!r1.marcado && r2.marcado)
        return MENOR;
    else
        return RegistroCompara(r1.reg, r2.reg, dados);
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

bool leItem(FILE* arquivo, Item* reg, Dados *dados){
    Registro r;
    // if (leRegistro(arquivo, &r)) {  -> Não há necessidade de reler o texto já que o binário foi gerado
    if(fread(&r, sizeof(Registro), 1, arquivo)){
        dados->transferencias.leituras++;
        transformaEmItem(r, reg);
        return true;
    }
    return false;
}

Item heapRemoveSub(Item *h, int *n, Dados *dados){
    Item raiz = h[0];
    (*n)--;
    if (*n > 0) {
        h[0] = h[*n];
        HeapRefazSub(h, 0, *n - 1, dados);
    }
    return raiz;
}

void HeapConstroiSub(Item *v, int n, Dados *dados) {
    int left = n / 2 - 1;
    while (left >= 0) {
        HeapRefazSub(v, left, n - 1, dados);
        left--;
    }
}

/* CORREÇÃO 1: Construção de MIN-HEAP */
void HeapRefazSub(Item *v, int l, int r, Dados *dados) {
    Item aux = v[l];
    int i = l;
    int j = i * 2 + 1;

    while (j <= r) {
        // Encontra o MENOR filho
        if (j < r && ItemCompara(v[j], v[j + 1], dados) == MAIOR)
            j++;

        COMP resp = ItemCompara(aux, v[j], dados);

        // Se o pai já é menor ou igual ao menor filho, a propriedade está mantida
        if (resp == MENOR || resp == IGUAL)
            break;

        v[i] = v[j];
        i = j; // Desce o elemento aux para a posição correta
        j = i * 2 + 1;
    }
    v[i] = aux;
}

/* CORREÇÃO 2: Lógica otimizada de Seleção por Substituição */
int geraBlocosSubstituicao(FILE* arquivo, int quantidade, Dados *dados) {
    int blocosGerados = 0;

    ChamarCriadorFitas();

    Item itens[TAM_MEMORIA];
    int tamHeap = 0;
    char nomeArquivo[100];

    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;
    curinga.cidade[0] = '\0';
    curinga.curso[0] = '\0';
    curinga.estado[0] = '\0';

    int contador = 0;
    Item novoItem;

    //Preenche a memoria totalmente
    while (tamHeap < TAM_MEMORIA && contador < quantidade && leItem(arquivo, &novoItem, dados)) {
        itens[tamHeap] = novoItem;
        tamHeap++;
        contador++;
    }

    if (tamHeap == 0)
        return 0;

    HeapConstroiSub(itens, tamHeap, dados);

    int w = 1;
    sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);
    FILE *fita = fopen(nomeArquivo, "ab");

    while (tamHeap > 0) {
        // Se a raiz está marcada, significa que todos os elementos estão marcados
        if (itens[0].marcado) {

            //escreve o curinga para simbolizar o fum do bloco
            fwrite(&curinga, sizeof(Registro), 1, fita);
            fclose(fita);
            dados->transferencias.escritas++;
            blocosGerados++;

            //atualiza para a proxima fita
            w++;
            if (w == FITAS_ENTRADA + 1)
                w = 1;

            sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);
            fita = fopen(nomeArquivo, "ab");

            //desmarca os elementos pra proxima fita
            desmarcaElementos(itens, tamHeap);
            //reconstroi o heap pra manter a ordenação correta
            HeapConstroiSub(itens, tamHeap, dados);
        }

        //remove o menor item e insere na fita de saída atual
        Item menorItem = itens[0];
        Registro r;
        transformaEmRegistro(menorItem, &r);
        fwrite(&r, sizeof(Registro), 1, fita);
        dados->transferencias.escritas++;

        //tenta ler um novo registro para colocar no lugar da raiz
        if (contador < quantidade && leItem(arquivo, &novoItem, dados)) {
            contador++;

            //se o novo item for menor que o último, não entra neste bloco e é marcado como maior que todos
            if (RegistroCompara(novoItem.reg, menorItem.reg, dados) == MENOR) {
                novoItem.marcado = true;
            }

            //coloca o novo item diretamente na raiz e refaz o heap
            itens[0] = novoItem;
            HeapRefazSub(itens, 0, tamHeap - 1, dados);

        }
        else {
            //se o arquivo de leitura acabou, retiramos valores do heap
            itens[0] = itens[tamHeap - 1];
            tamHeap--;
            if (tamHeap > 0) {
                HeapRefazSub(itens, 0, tamHeap - 1, dados);
            }
        }
    }

    //fecha o último bloco aberto
    fwrite(&curinga, sizeof(Registro), 1, fita);
    blocosGerados++;
    fclose(fita);

    return blocosGerados;
}
