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


void IntercalacaoBalanceada(FILE* arquivo, int quantidade, int situacao, Dados* dados) {
    //calcular o número de registros e quantas passadas serão necessárias para se ordenar

    double passadasNasFitas = calculaP(quantidade);

    //Cria a página de fitas
    ChamarCriadorFitas();

    //Primeira leitura e armazenamento em memória interna---------------------------------------
    Registro registros[m];
    
    char nomeArquivo[100];
    int menor;

    //curinga indica o fim de um bloco de registro
    Registro curinga;
    curinga.numero = -1;
    curinga.nota = -1;


    Registro copia;
    if(!leRegistro(arquivo, &copia)){
        printf("Erro ao ler os registros do arquivo.\n");
        return;
    }

    // a variável tamMemoria representa a posiçao no vetor em memória principal
    int tamMemoria = 0;

    //a variável w representa em qual fita de entrada está sendo escrita
    int w = 1, contador = 0;

    do{

        registros[tamMemoria] = copia;

        //se a memoria interna estiver cheia, ordena e escreve um bloco na fita
        if(tamMemoria == 19){

            //se fita ultrapassa o limite de 20, volta pra primeira fita
            if(w == 21){
                w = 1;
            }

            HeapSort(registros, 20);

            sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);

            FILE *fita = fopen(nomeArquivo, "ab");
            //escreve os 20 registros ordenados e o curinga no final do bloco
            fwrite(registros, sizeof(Registro), 20, fita);
            fwrite(&curinga, sizeof(Registro), 1, fita);
            fclose(fita);

            //atualiza a fita a ser escrita e reinicia o indice da memoria interna
            w++;
            tamMemoria = 0;
        }else{
            tamMemoria++;

        }
        contador++;
    //realiza a leitura enquanto houver registros no arquivo
    }while(leRegistro(arquivo, &copia) && contador <= quantidade);
    
    //se i nao termina em 0, numRegistros não é divisível por 20
    //ordena os registros restantes e escreve na devida fita
    if(tamMemoria != 0){
        HeapSort(registros, tamMemoria);

        sprintf(nomeArquivo, "fitas/entrada_%02d.bin", w);


        FILE *fita = fopen(nomeArquivo, "ab");
        fwrite(registros, sizeof(Registro), tamMemoria, fita);
        fwrite(&curinga, sizeof(Registro), 1, fita);

        fclose(fita);

    }

    //----------------------------------------------------------------------

    fclose(arquivo);

    //Primeira passada sobre o arquivo na fase de intercalação----------------------------------------------
    //Blocos iniciais foram gerados

    FILE* ponteirosDeFile[FITAS_ENTRADA];

    abrirFitasEntrada(ponteirosDeFile, 0);
    
    //
    bool ativos[m];
    int ativosNum = m;
    for(int i = 0; i < m; i++){
        //todas as fitas começam ativas
        ativos[i] = true;
    }


    int fitaSaida = 1;

    for(int i = 0; i < 20; i++){

        //VERIFICAR SE LEU O CORINGA

        if(fread(&registros[i], sizeof(Registro), 1, ponteirosDeFile[i]) != 1){

            ativos[i] = false;
            ativosNum--;
        }else if(registros[i].nota == -1){
            ativos[i] = false;
            ativosNum--;
            
        }
    }


    while(ativosNum > 0){
        
        //Pode ser que eu acaba lendo menos de 20s
        

        //menor indica o índice da fita com o menor valor retirado 
        menor = indiceMenorRegistro(registros, 20, ativos);
        //Caso nenhuma fita está ativa mais
        if(menor == -1){

            sprintf(nomeArquivo, "fitas/saida_%02d.bin", fitaSaida);

            //finaliza um bloco na fita de saída (terminou de intercalar blocos das fitas de entrada)
            FILE *fita = fopen(nomeArquivo, "ab");
            fwrite(&curinga, sizeof(Registro), 1, fita);
            fclose(fita);

            //Leio todos mais uma vez
            ativosNum = m;
            for(int i = 0; i < 20; i++){
                if(fread(&registros[i], sizeof(Registro), 1, ponteirosDeFile[i]) != 1){

                    ativos[i] = false;
                    ativosNum--;
                }else{
                    ativos[i] = true;
                }

            }
            
            fitaSaida++;

        }else {

            sprintf(nomeArquivo, "fitas/saida_%02d.bin", fitaSaida);

            FILE *fita = fopen(nomeArquivo, "ab");
            fwrite(&registros[menor], sizeof(Registro), 1, fita);
            fclose(fita);

            if(fread(&registros[menor], sizeof(Registro), 1, ponteirosDeFile[menor]) != 1){

                //Verificação do curinga

                ativos[menor] = false;
                ativosNum--;

            }else if(registros[menor].nota == -1){
                ativos[menor] = false;
                ativosNum--;
                
            }
            //SE FALHAR EU DEVO REMOVER O MENOR DA LEITURA
        }
    }

    fecharFitasIntercalacao(ponteirosDeFile);
    //Sequencia de passadas sobre, com base na regra de P(n)------------------------------------------------------------------


    for(int i = 0; i < passadasNasFitas -1; i++){

        if(i % 2 == 1){
            abrirFitasSaida(ponteirosDeFile, 1);
        }

        else{
            abrirFitasEntrada(ponteirosDeFile, 1);
        }
        
        ativosNum = 20;
        for(int j = 0; j < 20; j++)
            ativos[j] = true;

        int fitasAtual = 1;

        for(int j = 0; j < 20; j++){

            if(fread(&registros[j], sizeof(Registro), 1, ponteirosDeFile[j]) != 1){

                ativos[j] = false;
                ativosNum--;

            }else if(registros[j].nota == -1){
                ativos[j] = false;
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

                ativosNum = 20;
                //Leio todos mais uma vez
                for(int j = 0; j < 20; j++){
                    if(fread(&registros[j], sizeof(Registro), 1, ponteirosDeFile[j]) != 1){

                        ativos[j] = false;
                        ativosNum--;
                    }else{
                        ativos[j] = true;
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
                }else if(registros[i].nota == -1){
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

//RESOLVER ESSA FUNÇÃO
//Podemos utilizar heap
int indiceMenorRegistro(Registro v[], int n, bool ativos[20]) {
    int menor = -1;
    for (int i = 0; i < n; i++) {
        if (ativos[i] && v[i].nota != -1) {
            if (menor == -1 || RegistroCompara(v[i], v[menor]) == MENOR) {
                menor = i;
            }
        }
    }
    return menor;
    
}


double calculaP(int n) {
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

        //assassino "limpa" o arquivo
        if(n == 1){
            FILE* assassino = fopen(nomeArquivo, "wb");
            fclose(assassino);
        }

        //cria uma fita de entrada
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
            FILE* assassino = fopen(nomeArquivo, "wb");
            fclose(assassino);
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
    //calcula numero de registros na memoria
    int n = ObterNumCelOcupadas(Area);
    
    if (n == 0) return;

    *R = Area[0]; //menor elemento está no começo

    //move os elementos restantes para a esquerda
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