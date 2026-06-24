#include "intercalacao.h"

//============================================================
//FUNÇÕES DO HEAP
//============================================================ 

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

/* ============================================================
 *  SEÇÃO 2 – Min-Heap para intercalação de blocos
 *
 *  Cada nó da heap carrega o registro E o índice da fita de
 *  onde ele veio.  Isso permite saber qual fita alimentar após
 *  remover o mínimo (operação O(log f) vs. O(f) da busca linear).
 * ============================================================ */

typedef struct {
    Registro reg;
    int indiceFita;   /* índice 0-based da fita de origem */
} NoHeap;

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
        int esq   = 2 * i + 1;
        int dir   = 2 * i + 2;
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

//============================================================
//Auxiliar: intercala um conjunto de fitas abertas para um conjunto de fitas de saída
//============================================================

void intercalaFitas(FILE *entrada[], int quantFitas, char *prefixoSaida, int numSaida) {
    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;

    char nomeArq[100];
    int  fitaSaida = 1; //qual arquivo (fita) de saída está sendo preenchido

    //booleano por fita: ela ainda tem dados para esta passada?
    bool fitaEsgotada[FITAS_ENTRADA];
    for (int i = 0; i < quantFitas; i++)
        //nenhuma fita esgotada inicialmente
        fitaEsgotada[i] = false;

    //cria a memória principal (heap de nós)
    NoHeap heap[FITAS_ENTRADA];
    int heapN = 0;

    //carga inicial (pega o primeiro registro de cada fita)
    for (int i = 0; i < quantFitas; i++) {
        Registro r;
        if (fread(&r, sizeof(Registro), 1, entrada[i]) != 1) {
            fitaEsgotada[i] = true;
            continue;
        }
        if (r.nota == -1) {
            //Curinga logo no início: fita tem apenas blocos já consumidos antes
            fitaEsgotada[i] = true;
            continue;
        }
        //insere na memória (heap de nós) o registro lido e o índice que indica a fita em que foi lido
        NoHeap no = { r, i };
        heapN = heapInsere(heap, heapN, no);//insere nó no heap e atualiza o tamanho do heap (heapN)
    }

    //loop principal de intercalação
    //ativosNoBloco indica quantas fitas ainda contribuem para o bloco atual
    bool ativosNoBloco[FITAS_ENTRADA];
    int  ativosNum = 0;
    for (int i = 0; i < quantFitas; i++) {
        ativosNoBloco[i] = !fitaEsgotada[i];
        if (ativosNoBloco[i])
            ativosNum++;
    }

    while (heapN > 0 || ativosNum > 0) {

        if (heapN == 0) {
            break;
        }

        //retira o menor elemento da memória
        NoHeap NoMinimo = heapRemove(heap, &heapN);

        //escreve na fita de saída atual
        sprintf(nomeArq, prefixoSaida, fitaSaida);
        FILE *fSaida = fopen(nomeArq, "ab");
        fwrite(&NoMinimo.reg, sizeof(Registro), 1, fSaida);
        fclose(fSaida);

        //le o próximo registro da mesma fita de onde veio o mínimo
        int indiceFita = NoMinimo.indiceFita;
        Registro prox;

        if (!fitaEsgotada[indiceFita] && fread(&prox, sizeof(Registro), 1, entrada[indiceFita]) == 1) {
            if (prox.nota == -1) {
            
                //leu curinga, esta fita terminou o bloco atual
                //Marca como inativa NO BLOCO, mas não esgotada, pois pode ter mais blocos em seguida
                ativosNoBloco[indiceFita] = false;
                ativosNum--;

                if (ativosNum == 0) {
                    //Todas as fitas terminaram este bloco
                     
                    sprintf(nomeArq, prefixoSaida, fitaSaida);
                    FILE *fS = fopen(nomeArq, "ab");
                    //Fecha o bloco na saída com um curinga
                    fwrite(&curinga, sizeof(Registro), 1, fS);
                    fclose(fS);

                    //avança para a próxima fita de saída e
                    fitaSaida++;
                    if (fitaSaida > numSaida) 
                        fitaSaida = 1;

                    //recarrega heap para o próximo bloco
                    heapN = 0;
                    ativosNum = 0;

                    //reinicia o bloco: relê os registros das fitas que ainda não estão esgotadas
                    for (int i = 0; i < quantFitas; i++) {
                        if (fitaEsgotada[i]) 
                            continue;
                        Registro r;
                        if (fread(&r, sizeof(Registro), 1, entrada[i]) != 1) {
                            fitaEsgotada[i] = true;
                            continue;
                        }
                        if (r.nota == -1) {
                            fitaEsgotada[i] = true;
                            continue;
                        }
                        ativosNoBloco[i] = true;
                        ativosNum++;
                        NoHeap no = { r, i };
                        heapN = heapInsere(heap, heapN, no);
                    }
                }
                //se ainda há fitas ativas, continuamos o bloco sem reinserir nada
            }
            else{
                //registro normal: reinsere na heap
                NoHeap no = { prox, indiceFita };
                heapN = heapInsere(heap, heapN, no);
            }
        }
        else{
            //fread falhou: fita totalmente esgotada 
            fitaEsgotada[indiceFita]  = true;
            ativosNoBloco[indiceFita] = false;
            if (ativosNum > 0) 
                ativosNum--;

            if (ativosNum == 0 && heapN == 0) {
                //Fecha o último bloco se ainda aberto 
                sprintf(nomeArq, prefixoSaida, fitaSaida);
                FILE *fS = fopen(nomeArq, "ab");
                fwrite(&curinga, sizeof(Registro), 1, fS);
                fclose(fS);
            }
        }
    }
}

//============================================================
//IntercalacaoBalanceada
//============================================================ 

void IntercalacaoBalanceada(FILE *arquivo, int quantidade, int situacao, Dados *dados) {

    double passadasNasFitas = calculaP(quantidade);

    ChamarCriadorFitas();

    //Geração dos blocos iniciais via HeapSort

    Registro registros[m];
    char nomeArquivo[100];

    //curinga indica o fim de um bloco de registro
    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;

    Registro copia;
    bool leu = leRegistro(arquivo, &copia);

    // a variável i representa a posiçao no vetor em memória principal
    int i = 0;

    //a variável w representa em qual fita de entrada está sendo escrita
    int w = 1;

    int contador = 1;

    do {
        registros[i] = copia;

        if (i == 19) {
            if (w == FITAS_ENTRADA + 1) 
                w = 1;

            HeapSort(registros, 20);

            sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);
            FILE *fita = fopen(nomeArquivo, "ab");
            fwrite(registros, sizeof(Registro), 20, fita);
            fwrite(&curinga,  sizeof(Registro), 1,  fita);
            fclose(fita);

            w++;
            i = 0;
        } 
        else{
            i++;
        }
        contador++;
    } while (leRegistro(arquivo, &copia) && contador < quantidade);



    // ultimo bloco(quantidade não múltipla de m)
    if (i != 0) {
        HeapSort(registros, i);
        sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);
        FILE *fita = fopen(nomeArquivo, "ab");
        fwrite(registros, sizeof(Registro), i,  fita);
        fwrite(&curinga,  sizeof(Registro), 1,  fita);
        fclose(fita);
    }

    fclose(arquivo);

    //Primeira intercalação (entrada -> saída)

    FILE *ponteiros[FITAS_ENTRADA];
    abrirFitasEntrada(ponteiros, 0);   /* 0 = não apaga, só abre para leitura */

    intercalaFitas(ponteiros, FITAS_ENTRADA, "fitas/saida_%02d.bin", FITAS_ENTRADA);

    fecharFitasIntercalacao(ponteiros);

    //Passadas alternando entrada e saída

    for (int p = 1; p <= (int)passadasNasFitas ; p++) {

        if (p % 2 == 1) {
            //Lê das saídas, escreve nas entradas
            abrirFitasSaida(ponteiros, 0);
            /* Limpa entradas antes de escrever */

            FILE *tmp[FITAS_ENTRADA];
            abrirFitasEntrada(tmp, 1);   /* 1 = apaga e reabre */
            fecharFitasIntercalacao(tmp);
            
            intercalaFitas(ponteiros, FITAS_ENTRADA, "fitas/entrada_%02d.bin", FITAS_ENTRADA);
        }
        else{
            //Lê das entradas, escreve nas saídas
            abrirFitasEntrada(ponteiros, 0);
        
            FILE *tmp[FITAS_ENTRADA];
            abrirFitasSaida(tmp, 1);
            fecharFitasIntercalacao(tmp);
        
            intercalaFitas(ponteiros, FITAS_ENTRADA, "fitas/saida_%02d.bin", FITAS_ENTRADA);
        }

        fecharFitasIntercalacao(ponteiros);
    }
}

//============================================================
//Funções auxiliares  
//============================================================

void fecharFitasIntercalacao(FILE *fitas[FITAS_ENTRADA]) {
    for (int i = 0; i < FITAS_ENTRADA; i++) {
        if (fitas[i] != NULL) {
            fclose(fitas[i]);
            fitas[i] = NULL;
        }
    }
}

/*
 * indiceMenorRegistro mantida por compatibilidade, mas não é mais
 * chamada pela lógica principal (substituída pela heap).
 */
int indiceMenorRegistro(Registro v[], int n, bool ativos[20]) {
    int menor = -1;
    for (int i = 0; i < n; i++) {
        if (ativos[i] && v[i].nota != -1) {
            if (menor == -1 || RegistroCompara(v[i], v[menor]) == MENOR)
                menor = i;
        }
    }
    return menor;
}

double calculaP(int n) {
    int f = 20;
    if (n <= 0)
        return 0.0;
    double blocos = (double)n / m;
    if (blocos <= 1.0) 
        return 0.0;
    return ceil(log(blocos) / log(f));
}

int contarRegistrosTxt(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir %s\n", nomeArquivo);
        return -1;
    }
    int quantidade = 0;
    char linha[150];
    while (fgets(linha, sizeof(linha), arquivo) != NULL)
        if (strlen(linha) > 1) 
            quantidade++;
    fclose(arquivo);
    return quantidade;
}

void abrirFitasEntrada(FILE *fitasEntrada[FITAS_ENTRADA], int n) {
    char nomeArquivo[100];
    for (int i = 0; i < FITAS_ENTRADA; i++) {
        sprintf(nomeArquivo, "fitas/entrada_%02d.bin", i + 1);
        if (n == 1) {
            FILE *k = fopen(nomeArquivo, "wb");
            fclose(k);
        }
        fitasEntrada[i] = fopen(nomeArquivo, "rb");
        if (fitasEntrada[i] == NULL) {
            printf("Erro ao abrir %s\n", nomeArquivo);
            for (int j = 0; j < i; j++) 
                fclose(fitasEntrada[j]);
            return;
        }
    }
}

void abrirFitasSaida(FILE *fitasSaida[FITAS_ENTRADA], int n) {
    char nomeArquivo[100];
    for (int i = 0; i < FITAS_ENTRADA; i++) {
        sprintf(nomeArquivo, "fitas/saida_%02d.bin", i + 1);
        if (n == 1) { 
            FILE *k = fopen(nomeArquivo, "wb");
            fclose(k);
        }
        fitasSaida[i] = fopen(nomeArquivo, "rb");
        if (fitasSaida[i] == NULL) {
            printf("Erro ao abrir %s\n", nomeArquivo);
            for (int j = 0; j < i; j++) 
                fclose(fitasSaida[j]);
            return;
        }
    }
}

void trimFim(char *str) {
    int i = strlen(str) - 1;
    while (i >= 0 && (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t'))
        str[i--] = '\0';
}

int ObterNumCelOcupadas(Registro *Area) {
    int count = 0;
    for (int i = 0; i < m; i++)
        if (Area[i].numero != 0) 
            count++;
    return count;
}

void InsereItem(Registro item, Registro *Area) {
    int n = ObterNumCelOcupadas(Area);
    int i = n - 1;
    while (i >= 0 && Area[i].nota > item.nota){ 
        Area[i + 1] = Area[i];
        i--;
    }
    Area[i + 1] = item;
}

void RetiraPrimeiro(Registro *Area, Registro *R) {
    int n = ObterNumCelOcupadas(Area);
    if (n == 0) return;
    *R = Area[0];
    for (int i = 0; i < n - 1; i++) 
        Area[i] = Area[i + 1];
    Area[n-1].numero = 0;
    Area[n-1].nota = 0.0;
    strcpy(Area[n-1].estado, "");
    strcpy(Area[n-1].cidade, "");
    strcpy(Area[n-1].curso, "");
}

void RetiraUltimo(Registro *Area, Registro *R) {
    int n = ObterNumCelOcupadas(Area);
    if (n == 0) return;
    *R = Area[n-1];
    Area[n-1].numero = 0;
    Area[n-1].nota = 0.0;
    strcpy(Area[n-1].estado, ""); 
    strcpy(Area[n-1].cidade, ""); 
    strcpy(Area[n-1].curso, "");
}

void criarTxt(const char* nomeArquivoSaida) {

    /* Conta quantos curingas (fim de bloco) existem em uma fita.
       Uma fita totalmente ordenada tem exatamente 1 bloco → 1 curinga. */
    int contarCuringas(const char* nomeFita) {
        FILE* f = fopen(nomeFita, "rb");
        if (!f) return -1; /* fita inexistente */

        Registro reg;
        int curingas = 0;
        while (fread(&reg, sizeof(Registro), 1, f) == 1) {
            if (reg.nota == -1 && reg.numero == -1)
                curingas++;
        }
        fclose(f);
        return curingas;
    }

    /* Descobre qual das duas fitas candidatas é a fita final ordenada */
    const char* candidatos[2] = {
        "fitas/saida_01.bin",
        "fitas/entrada_01.bin"
    };

    const char* fitaFinal = NULL;

    for (int i = 0; i < 2; i++) {
        int c = contarCuringas(candidatos[i]);
        /* A fita final tem exatamente 1 curinga (um único bloco ordenado) */
        if (c == 1) {
            fitaFinal = candidatos[i];
            break;
        }
    }

    if (!fitaFinal) {
        printf("criarTxt: nao foi possivel identificar a fita de saida final.\n");
        return;
    }

    printf("criarTxt: fita de saida final identificada como '%s'.\n", fitaFinal);

    /* Traduz a fita final para texto */
    FILE* fita = fopen(fitaFinal, "rb");
    if (!fita) {
        printf("criarTxt: erro ao abrir '%s'.\n", fitaFinal);
        return;
    }

    FILE* arquivoSaida = fopen(nomeArquivoSaida, "w");
    if (!arquivoSaida) {
        printf("criarTxt: erro ao criar '%s'.\n", nomeArquivoSaida);
        fclose(fita);
        return;
    }

    Registro reg;
    int totalEscritos = 0;

    while (fread(&reg, sizeof(Registro), 1, fita) == 1) {
        /* Ignora curingas de fim de bloco */
        if (reg.nota == -1 && reg.numero == -1)
            continue;

        /* Reproduz a formatação original do PROVAO.TXT */
        fprintf(arquivoSaida,
                "%-8ld %05.2f %s %-50s %-30s\n",
                reg.numero,
                reg.nota,
                reg.estado,
                reg.cidade,
                reg.curso);
        totalEscritos++;
    }

    fclose(fita);
    fclose(arquivoSaida);

    printf("criarTxt: %d registros escritos em '%s'.\n", totalEscritos, nomeArquivoSaida);
}