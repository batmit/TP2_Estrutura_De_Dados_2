#include "intercalacao.h"

//============================================================
//Auxiliar: intercala um conjunto de fitas abertas para um conjunto de fitas de saída
//============================================================

void intercalaFitas(FILE *entrada[], int quantFitas, char *prefixoSaida, int numSaida, Dados *dados) {
    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;
    curinga.cidade[0] = '\0';
    curinga.curso[0] = '\0';
    curinga.estado[0] = '\0';

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
        dados->transferencias.leituras++;
        if (r.nota == -1) {
            //Curinga logo no início: fita tem apenas blocos já consumidos antes
            fitaEsgotada[i] = true;
            continue;
        }
        //insere na memória (heap de nós) o registro lido e o índice que indica a fita em que foi lido
        NoHeap no = { r, i };
        heapN = heapInsere(heap, heapN, no, dados);//insere nó no heap e atualiza o tamanho do heap (heapN)
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
        NoHeap NoMinimo = heapRemove(heap, &heapN, dados);

        //escreve na fita de saída atual
        sprintf(nomeArq, prefixoSaida, fitaSaida);
        FILE *fSaida = fopen(nomeArq, "ab");
        fwrite(&NoMinimo.reg, sizeof(Registro), 1, fSaida);
        dados->transferencias.escritas++;
        fclose(fSaida);

        //le o próximo registro da mesma fita de onde veio o mínimo
        int indiceFita = NoMinimo.indiceFita;
        Registro prox;

        if (!fitaEsgotada[indiceFita] && fread(&prox, sizeof(Registro), 1, entrada[indiceFita]) == 1) {
            dados->transferencias.leituras++;
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
                    dados->transferencias.escritas++;
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
                        dados->transferencias.leituras++;
                        if (r.nota == -1) {
                            fitaEsgotada[i] = true;
                            continue;
                        }
                        ativosNoBloco[i] = true;
                        ativosNum++;
                        NoHeap no = { r, i };
                        heapN = heapInsere(heap, heapN, no, dados);
                    }
                }
                //se ainda há fitas ativas, continuamos o bloco sem reinserir nada
            }
            else{
                //registro normal: reinsere na heap
                NoHeap no = { prox, indiceFita };
                heapN = heapInsere(heap, heapN, no, dados);
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
                dados->transferencias.escritas++;
                fclose(fS);
            }
        }
    }
}

//============================================================
//IntercalacaoBalanceada
//============================================================

void IntercalacaoBalanceada(FILE *arquivo, int quantidade, int metodo, Dados *dados) {

    int quantidadeBlocos;
    if(metodo == 1){
        //função de geração de blocos;
        quantidadeBlocos = geraBlocosHeap(arquivo, quantidade, dados);
    }
    else if (metodo == 2){
        //função de geração de blocos;
        quantidadeBlocos = geraBlocosSubstituicao(arquivo, quantidade, dados);
    }
    else
        return;

    int passadasNasFitas = calculaP(quantidadeBlocos);

    //Primeira intercalação (entrada -> saída)

    FILE *ponteiros[FITAS_ENTRADA];
    // abrirFitasEntrada(ponteiros, 0);   /* 0 = não apaga, só abre para leitura */

    // intercalaFitas(ponteiros, FITAS_ENTRADA, "fitas/entrada_%02d.bin", FITAS_ENTRADA);

    // fecharFitasIntercalacao(ponteiros);

    //Passadas alternando entrada e saída
    for (int p = 0; p < (int)passadasNasFitas; p++) {

        if (p % 2 == 1) {
            //Lê das saídas, escreve nas entradas
            abrirFitasSaida(ponteiros, 0);
            /* Limpa entradas antes de escrever */

            FILE *tmp[FITAS_ENTRADA];
            abrirFitasEntrada(tmp, 1);   /* 1 = apaga e reabre */
            fecharFitasIntercalacao(tmp);

            intercalaFitas(ponteiros, FITAS_ENTRADA, "fitas/entrada_%02d.bin", FITAS_ENTRADA, dados);
        }
        else{
            //Lê das entradas, escreve nas saídas
            abrirFitasEntrada(ponteiros, 0);

            FILE *tmp[FITAS_ENTRADA];
            abrirFitasSaida(tmp, 1);
            fecharFitasIntercalacao(tmp);

            intercalaFitas(ponteiros, FITAS_ENTRADA, "fitas/saida_%02d.bin", FITAS_ENTRADA, dados);
        }

        fecharFitasIntercalacao(ponteiros);
    }

    // Se a quantidade de passadas for par, o arquivo final estará na primeira fita de entrada; caso contrário, estará na primeira fita de saída.
    if(passadasNasFitas % 2){
        FILE *final = fopen("fitas/saida_01.bin", "rb");
        criaArquivoSaida(final, "Resultado.txt");
        fclose(final);
    } else{
        FILE *final = fopen("fitas/entrada_01.bin", "rb");
        criaArquivoSaida(final, "Resultado.txt");
        fclose(final);
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

double calculaP(int blocos) {
    if (blocos <= 1.0)
        return 0.0;
    return ceil(log(blocos) / log(FITAS_ENTRADA));
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
    for (int i = 0; i < TAM_MEMORIA; i++)
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

int geraBlocosHeap(FILE *arquivo, int quantidade, Dados *dados) {
    int blocosGerados = 0;

    ChamarCriadorFitas();

    //Geração dos blocos iniciais via HeapSort

    Registro registros[TAM_MEMORIA];
    char nomeArquivo[100];

    //curinga indica o fim de um bloco de registro
    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;
    curinga.cidade[0] = '\0';
    curinga.curso[0] = '\0';
    curinga.estado[0] = '\0';

    Registro copia;
    // leRegistro(arquivo, &copia); -> Não há necessidade de reler o texto já que o binário foi gerado
    fread(&copia, sizeof(Registro), 1, arquivo);
    dados->transferencias.leituras++;

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

            HeapSort(registros, 20, dados);

            sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);
            FILE *fita = fopen(nomeArquivo, "ab");
            fwrite(registros, sizeof(Registro), 20, fita);
            dados->transferencias.escritas++;
            fwrite(&curinga,  sizeof(Registro), 1,  fita);
            dados->transferencias.escritas++;
            fclose(fita);

            w++;
            i = 0;
            blocosGerados++;
        }
        else{
            i++;
        }
        contador++;
    } while (fread(&copia, sizeof(Registro), 1, arquivo) && dados->transferencias.leituras++ && contador < quantidade); // leRegistro(arquivo, &copia); -> Não há necessidade de reler o texto já que o binário foi gerado

    // ultimo bloco(quantidade não múltipla de m)
    if (i != 0) {
        HeapSort(registros, i, dados);
        sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);
        FILE *fita = fopen(nomeArquivo, "ab");
        fwrite(registros, sizeof(Registro), i,  fita);
        dados->transferencias.escritas++; 
        fwrite(&curinga,  sizeof(Registro), 1,  fita);
        dados->transferencias.escritas++;
        fclose(fita);
        blocosGerados++;
    }

    fclose(arquivo);
    return blocosGerados;
}
