#include "registro.h"
#include "heap.h"
#include <stdio.h>

COMP RegistroCompara(Registro r1, Registro r2, Dados *dados) {
    if(dados)
        dados->comparacoes++;

    if (r1.nota < r2.nota)
        return MENOR;
    else if (r1.nota > r2.nota)
        return MAIOR;
    else
        return IGUAL;
}

void RegistroPrint(Registro item) {
    printf("Número: %ld\n", item.numero);
    printf("Nota: %.2f\n", item.nota);
    printf("Estado: %s\n", item.estado);
    printf("Cidade: %s\n", item.cidade);
    printf("Curso: %s\n", item.curso);
}

//realiza a leitura de uma linha inteira, particiona os campos e preenche a struct Registro
bool leRegistro(FILE* arquivo, Registro* reg) {
    char linha[110];//guarda uma linha inteira

    //le uma linha inteira
    if (fgets(linha, sizeof(linha), arquivo) != NULL) {

        char numero[9];//numero tem 8 caracteres + \0 (ex: "00670838 + \0")
        char nota[6];//nota tem 5 caracteres + \0 (ex: "06.70 + \0")

        // strncpy(destino, origem[posicao_inicial], qtd_caracteres)

        //numero (colunas 1 a 8 -> posição 0, tamanho 8)
        strncpy(numero, &linha[0], 8);
        numero[8] = '\0';//insere o \0 no fim da string
        reg->numero = atol(numero);//converte para long

        //nota (colunas 10 a 14 -> posição 9, tamanho 5)
        strncpy(nota, &linha[9], 5);
        nota[5] = '\0';//insere o \0 no fim da string
        reg->nota = atof(nota);//converte para float

        //estado (colunas 16 e 17 -> posição 15, tamanho 2)
        strncpy(reg->estado, &linha[15], 2);
        reg->estado[2] = '\0';

        // Cidade (Colunas 19 a 68 -> posição 18, tamanho 50)
        strncpy(reg->cidade, &linha[18], 50);
        reg->cidade[50] = '\0';

        // Curso (Colunas 70 a 99 -> posição 69, tamanho 30)
        strncpy(reg->curso, &linha[69], 30);
        reg->curso[30] = '\0';

        return true;
    }
    return false;
}

void imprimirRegistros(FILE* arquivo, int quantidade){
    Registro reg;
    int i=0;
    while(leRegistro(arquivo,&reg) && i<quantidade){
        RegistroPrint(reg);
        i++;
    }
}

//transforma o arquivo texto em binario para facilitar a leitura e escrita durante os processos de ordenação
FILE* criaArquivoBinario(FILE* arquivoTexto, const char* nomeArquivoBinario, Dados* dados) {
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "wb");
    if (!arquivoBinario) {
        printf("Erro ao criar o arquivo binário.\n");
        return NULL;
    }

    Registro reg;
    while (leRegistro(arquivoTexto, &reg)) {
        dados->transferencias.leituras++;
        fwrite(&reg, sizeof(Registro), 1, arquivoBinario);
        dados->transferencias.escritas++;
    }

    fclose(arquivoBinario);
    fclose(arquivoTexto);
    return fopen(nomeArquivoBinario, "rb");
}

//retorna os registros do arquivo binário para um arquivo texto de saída, mantendo a formatação original do arquivo PROVAO.txt
FILE* criaArquivoSaida(FILE* arquivoBinario, const char* nomeArquivoSaida) {
    FILE* arquivoSaida = fopen(nomeArquivoSaida, "wb");
    if (!arquivoSaida) {
        printf("Erro ao criar o arquivo de saída.\n");
        return NULL;
    }

    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, arquivoBinario) && reg.nota != -1) {
        fprintf(arquivoSaida, "%08ld %05.2f %s %s %s\n", reg.numero, reg.nota, reg.estado, reg.cidade, reg.curso);
    }

    fclose(arquivoSaida);
    return fopen(nomeArquivoSaida, "rb");
}

// Função de preparação
void prepararTexto(const char* arquivo, const char* arquivoOrdenado, int quantidade, int situacao) {
    FILE *arq = fopen(arquivo, "r");
    if (!arq) {
        printf("Erro ao abrir %s para preparação.\n", arquivo);
        return;
    }

    FILE *arqOrd = fopen(arquivoOrdenado, "w");
    if (!arq) {
        printf("Erro ao abrir %s para preparação.\n", arquivoOrdenado);
        return;
    }

    Registro *vetor = (Registro*) malloc(quantidade * sizeof(Registro));
    if (!vetor) {
        printf("Erro de alocação de memória na preparação.\n");
        fclose(arq);
        fclose(arqOrd);
        return;
    }

    // Lê a quantidade exata de registros do ficheiro base
    int lidos = 0;
    while (lidos < quantidade && !feof(arq)) {
        if (leRegistro(arq, &vetor[lidos])) lidos++;
        else break;
    }

    // Se a situação for 1 (Crescente) ou 2 (Decrescente), ordena o vetor na memória
    if (situacao == 1 || situacao == 2) {
        // Insertion Sort
        for (int i = 1; i < lidos; i++) {
            Registro pivo = vetor[i];
            int j = i - 1;

            if (situacao == 1) {
                // Ordenação Crescente: move os elementos maiores para a direita
                while (j >= 0 && vetor[j].nota > pivo.nota) {
                    vetor[j + 1] = vetor[j];
                    j--;
                }
            } else {
                // Ordenação Decrescente: move os elementos menores para a direita
                while (j >= 0 && vetor[j].nota < pivo.nota) {
                    vetor[j + 1] = vetor[j];
                    j--;
                }
            }
            vetor[j + 1] = pivo;
        }
    }

    for (int i = 0; i < lidos; i++) {
        fprintf(arqOrd, "%08ld %05.2f %s %s %s\n",
                vetor[i].numero,
                vetor[i].nota,
                vetor[i].estado,
                vetor[i].cidade,
                vetor[i].curso);
    }

    free(vetor);
    fclose(arq);
    fclose(arqOrd);
}
