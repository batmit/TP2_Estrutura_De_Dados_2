#include "intercalacao.h"

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


void IntercalacaoBalanceada(){
    //calcular o número de registros e quantas passadas serão necessárias para se ordenar
    long numRegistros = contarRegistrosTxt("PROVAO.TXT");

    double passadasNasFitas = calculaP(numRegistros);

    //Cria a página de fitas
    ChamarCriadorFitas();

    //Primeira leitura e armazenamento em memória interna---------------------------------------
    Registro registros[20];
    FILE* arquivo = fopen("PROVAO.TXT", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir PROVAO.TXT\n");
        return;
    }
    char nomeArquivo[100];
    int menor;
    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;


    Registro copia;
    int i = leRegistro(arquivo, &copia);
    // a variável i representa a posiçao no vetor em memória principal
    i = 0;

    //a variável w representa em qual fita de entrada está sendo escrita
    int w = 1;

    do{

        registros[i] = copia;

        if(i == 19){

            if(w == 21){
                w = 1;
            }

            HeapSort(registros, 20);

            sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);

            FILE *fita = fopen(nomeArquivo, "ab");
            fwrite(registros, sizeof(Registro), 20, fita);
            fwrite(&curinga, sizeof(Registro), 1, fita);
            fclose(fita);

            w++;
            i = 0;
        }else{
            i++;

        }


    }while(leRegistro(arquivo, &copia));
    //Caso não seja divisível por 20
    if(i != 0){
        HeapSort(registros, i);

        sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);


        FILE *fita = fopen(nomeArquivo, "ab");
        fwrite(registros, sizeof(Registro), i, fita);
        fwrite(&curinga, sizeof(Registro), 1, fita);

        fclose(fita);

    }

    //----------------------------------------------------------------------

    fclose(arquivo);

    //Primeira passada sobre o arquivo na fase de intercalação----------------------------------------------

    FILE* ponteirosDeFile[FITAS_ENTRADA];

    abrirFitasEntrada(ponteirosDeFile, 0);
    bool ativos[20];
    int ativosNum = 20;
    for(int i = 0; i < 20; i++){
        ativos[i] = true;
    }

    int fitaSaida = 1;

    for(int i = 0; i < 20; i++){

        if(fread(&registros[i], sizeof(Registro), 1, ponteirosDeFile[i]) != 1){

            ativos[i] = false;
            ativosNum--;
        }
    }


    while(ativosNum > 0){

        menor = indiceMenorRegistro(registros, 20, ativos);
        if(menor == -1){

            sprintf(nomeArquivo, "fitas/saida_%02d.bin", fitaSaida);

            FILE *fita = fopen(nomeArquivo, "ab");
            fwrite(&curinga, sizeof(Registro), 1, fita);
            fclose(fita);

            //Leio todos mais uma vez
            for(int i = 0; i < 20; i++){
                if(fread(&registros[i], sizeof(Registro), 1, ponteirosDeFile[i]) != 1){

                    ativos[i] = false;
                    ativosNum--;
                }

            }
            fitaSaida++;

        }else {

            sprintf(nomeArquivo, "fitas/saida_%02d.bin", fitaSaida);

            FILE *fita = fopen(nomeArquivo, "ab");
            fwrite(&registros[menor], sizeof(Registro), 1, fita);
            fclose(fita);

            if(fread(&registros[menor], sizeof(Registro), 1, ponteirosDeFile[menor]) != 1){

                ativos[menor] = false;
                ativosNum--;
            }


        }



    }

    fecharFitasIntercalacao(ponteirosDeFile);
    //Sequencia de passadas sobre, com base na regra de P(n)------------------------------------------------------------------


    for(int i = 1; i < passadasNasFitas; i++){

        if(i % 2 == 1){

            abrirFitasSaida(ponteirosDeFile, 1);

        }else{

            abrirFitasEntrada(ponteirosDeFile, 1);

        }
        ativosNum = 20;
        for(int i = 0; i < 20; i++){
            ativos[i] = true;
        }

        int fitasAtual = 1;

        for(int i = 0; i < 20; i++){

            if(fread(&registros[i], sizeof(Registro), 1, ponteirosDeFile[i]) != 1){

                ativos[i] = false;
                ativosNum--;

            }
        }


        while(ativosNum > 0){

            menor = indiceMenorRegistro(registros, 20, ativos);
            if(menor == -1){

                if(i % 2 == 1){
                    sprintf(nomeArquivo, "fitas/saida_%02d.bin", fitasAtual);


                }else{
                    sprintf(nomeArquivo, "fitas/entrada_%02d.bin", fitasAtual);


                }

                FILE *fita = fopen(nomeArquivo, "ab");
                fwrite(&curinga, sizeof(Registro), 1, fita);
                fclose(fita);


                //Leio todos mais uma vez
                for(int i = 0; i < 20; i++){
                    if(fread(&registros[i], sizeof(Registro), 1, ponteirosDeFile[i]) != 1){

                        ativos[i] = false;
                        ativosNum--;
                    }

                }
                fitasAtual++;

            }else{
                if(i % 2 == 1){
                    sprintf(nomeArquivo, "fitas/saida_%02d.bin", fitasAtual);


                }else{
                    sprintf(nomeArquivo, "fitas/entrada_%02d.bin", fitasAtual);


                }
                FILE *fita = fopen(nomeArquivo, "ab");
                fwrite(&registros[menor], sizeof(Registro), 1, fita);
                fclose(fita);

                if(fread(&registros[menor], sizeof(Registro), 1, ponteirosDeFile[menor]) != 1){

                    ativos[menor] = false;
                    ativosNum--;
                }



            }



        }
        fecharFitasIntercalacao(ponteirosDeFile);


    }


}


void fecharFitasIntercalacao(FILE* fitas[FITAS_ENTRADA]) {
    for (int i = 0; i < FITAS_ENTRADA; i++) {
        if (fitas[i] != NULL) {
            fclose(fitas[i]);
            fitas[i] = NULL;
        }
    }
}


int indiceMenorRegistro(Registro v[], int n, bool ativos[20]) {
    int menor = -1;

    for (int i = 0; i < n; i++) {
        if (ativos[i] && v[i].nota != -1) {
            if (menor == -1) {
                menor = i;
            } else if (RegistroCompara(v[i], v[menor]) == MENOR) {
                menor = i;
            }
        }
    }

    return menor;
}


double calculaP(long n) {
    // int m = 20; Já é uma constante definida em intercalacao.h
    int f = 20;

    if (n <= 0) {
        return 0.0;
    }

    double blocosIniciais = (double)n / m;

    if (blocosIniciais <= 1.0) {
        return 0.0;
    }

    return ceil(log(blocosIniciais) / log(f));
}


int contarRegistrosTxt(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return -1;
    }

    int quantidade = 0;
    char linha[150];

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        /*
            Cada linha do PROVAO.TXT representa um registro.
            Se quiser ignorar linhas vazias, verificamos se a linha
            tem pelo menos alguns caracteres úteis.
        */
        if (strlen(linha) > 1) {
            quantidade++;
        }
    }

    fclose(arquivo);

    return quantidade;
}


//Define um ponteiro de files, cada um apontando para um arquivo
void abrirFitasEntrada(FILE* fitasEntrada[FITAS_ENTRADA], int n) {
    char nomeArquivo[100];

    for (int i = 0; i < FITAS_ENTRADA; i++) {
        sprintf(nomeArquivo, "fitas/entrada_%02d.bin", i + 1);

        if(n == 1){

            FILE* assasino = fopen(nomeArquivo, "wb");
            fclose(assasino);

        }

        fitasEntrada[i] = fopen(nomeArquivo, "rb");

        if (fitasEntrada[i] == NULL) {
            printf("Erro ao abrir a fita %s\n", nomeArquivo);

            for (int j = 0; j < i; j++) {
                fclose(fitasEntrada[j]);
            }

            return;
        }
    }
}

void abrirFitasSaida(FILE* fitasSaida[FITAS_ENTRADA], int n) {
    char nomeArquivo[100];

    for (int i = 0; i < FITAS_ENTRADA; i++) {
        sprintf(nomeArquivo, "fitas/saida_%02d.bin", i + 1);
        if(n == 1){

            FILE* assasino = fopen(nomeArquivo, "wb");
            fclose(assasino);

        }
        fitasSaida[i] = fopen(nomeArquivo, "rb");

        if (fitasSaida[i] == NULL) {
            printf("Erro ao abrir a fita %s\n", nomeArquivo);

            for (int j = 0; j < i; j++) {
                fclose(fitasSaida[j]);
            }

            return;
        }
    }
}

/*
//Lê apenas 1 registro
int leRegistro(FILE* arquivo, Registro* reg) {
    char linha[150];

    if (arquivo == NULL || reg == NULL) {
        return false;
    }

    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        return false;
    }

    char campoNumero[9];
    char campoNota[6];

    strncpy(campoNumero, linha, 8);
    campoNumero[8] = '\0';

    strncpy(campoNota, linha + 9, 5);
    campoNota[5] = '\0';

    strncpy(reg->estado, linha + 15, 2);
    reg->estado[2] = '\0';

    strncpy(reg->cidade, linha + 18, 50);
    reg->cidade[50] = '\0';

    strncpy(reg->curso, linha + 69, 30);
    reg->curso[30] = '\0';

    reg->numero = atol(campoNumero);
    reg->nota = (float) atof(campoNota);

    trimFim(reg->cidade);
    trimFim(reg->curso);

    return 1;
}
*/
void trimFim(char* str) {
    int i = strlen(str) - 1;

    while (i >= 0 && (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t')) {
        str[i] = '\0';
        i--;
    }
}

// Funções que faltam
// Conta quantos registros na área estão ocupados
// Usamos um número diferente de zero como indicador de lugar ocupado.
int ObterNumCelOcupadas(Registro* Area) {
    int count = 0;
    for (int i = 0; i < m; i++)
        if (Area[i].numero != 0)
            count++;
    return count;
}

// Insere um novo item na area
void InsereItem(Registro item, Registro* Area) {
    int n = ObterNumCelOcupadas(Area);
    int i = n - 1;

    // Encontra a posição correta para inserir e manter a ordem
    while (i >= 0 && Area[i].nota > item.nota) {
        Area[i + 1] = Area[i];
        i--;
    }

    // Insere o item
    Area[i + 1] = item;
}

// Remove o registro com a menor nota
void RetiraPrimeiro(Registro* Area, Registro* R) {
    int n = ObterNumCelOcupadas(Area);
    if (n == 0) return;

    *R = Area[0]; // Elemento mínimo está no começo

    // Move os elementos restantes para a esquerda
    for (int i = 0; i < n - 1; i++)
        Area[i] = Area[i + 1];

    // Limpa o último elemento movido
    Area[n - 1].numero = 0;
    Area[n - 1].nota = 0.0;
    strcpy(Area[n - 1].estado, "");
    strcpy(Area[n - 1].cidade, "");
    strcpy(Area[n - 1].curso, "");
}

// Remove o registro com a maior nota
void RetiraUltimo(Registro* Area, Registro* R) {
    int n = ObterNumCelOcupadas(Area);
    if (n == 0) return;

    *R = Area[n - 1]; // Elemento máximo está no fim

    // Limpa o elemento extraído
    Area[n - 1].numero = 0;
    Area[n - 1].nota = 0.0;
    strcpy(Area[n - 1].estado, "");
    strcpy(Area[n - 1].cidade, "");
    strcpy(Area[n - 1].curso, "");
}
