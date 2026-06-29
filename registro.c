#include "registro.h"
#include "intercalacao.h"
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

    if (reg != NULL) {
        memset(reg, 0, sizeof(*reg));
    }

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
void criaArquivoSaida(FILE* arquivoBinario, const char* nomeArquivoSaida) {
    FILE* arquivoSaida = fopen(nomeArquivoSaida, "wb");
    if (!arquivoSaida) {
        printf("Erro ao criar o arquivo de saída.\n");
        return;
    }

    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, arquivoBinario) && reg.nota != -1) {
        fprintf(arquivoSaida, "%08ld %05.1f %s %s %s\n", reg.numero, reg.nota, reg.estado, reg.cidade, reg.curso);
    }

    fclose(arquivoSaida);
}

// Função de preparação usando intercalação externa com seleção por substituição
void prepararTexto(const char* arquivo, const char* arquivoOrdenado, int quantidade, int situacao) {
    FILE *arq = fopen(arquivo, "r");
    if (!arq) {
        printf("Erro ao abrir %s para preparação.\n", arquivo);
        return;
    }

    FILE *arqOrd = fopen(arquivoOrdenado, "w");
    if (!arqOrd) {
        printf("Erro ao abrir %s para preparação.\n", arquivoOrdenado);
        fclose(arq);
        return;
    }

    char nomeTemporario[256];
    snprintf(nomeTemporario, sizeof(nomeTemporario), "%s.tmp.bin", arquivoOrdenado);

    FILE *arqTmp = fopen(nomeTemporario, "wb");
    if (!arqTmp) {
        printf("Erro ao criar o arquivo temporário para preparação.\n");
        fclose(arq);
        fclose(arqOrd);
        return;
    }

    int lidos = 0;
    Registro reg;
    while (lidos < quantidade && leRegistro(arq, &reg)) {
        fwrite(&reg, sizeof(Registro), 1, arqTmp);
        lidos++;
    }

    fclose(arq);
    fclose(arqTmp);

    if (lidos > 0 && (situacao == 1 || situacao == 2)) {
        Dados dados = {0};
        FILE *arquivoBinario = fopen(nomeTemporario, "rb");
        if (!arquivoBinario) {
            printf("Erro ao abrir o arquivo temporário para ordenação externa.\n");
            fclose(arqOrd);
            remove(nomeTemporario);
            return;
        }

        remove("Resultado.txt");
        IntercalacaoBalanceada(arquivoBinario, lidos, 2, &dados);
        fclose(arquivoBinario);

        FILE *resultado = fopen("Resultado.txt", "r");
        if (!resultado) {
            printf("Erro ao abrir o arquivo de resultado da intercalação.\n");
            fclose(arqOrd);
            remove(nomeTemporario);
            return;
        }

        char **linhas = NULL;
        int qtdLinhas = 0;
        char buffer[256];

        while (fgets(buffer, sizeof(buffer), resultado) != NULL) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[--len] = '\0';
            }

            char *linha = (char *)malloc(len + 1);
            if (!linha) {
                for (int i = 0; i < qtdLinhas; i++) {
                    free(linhas[i]);
                }
                free(linhas);
                fclose(resultado);
                fclose(arqOrd);
                remove(nomeTemporario);
                return;
            }

            strcpy(linha, buffer);
            char **novo = (char **)realloc(linhas, (qtdLinhas + 1) * sizeof(char *));
            if (!novo) {
                free(linha);
                for (int i = 0; i < qtdLinhas; i++) {
                    free(linhas[i]);
                }
                free(linhas);
                fclose(resultado);
                fclose(arqOrd);
                remove(nomeTemporario);
                return;
            }

            linhas = novo;
            linhas[qtdLinhas++] = linha;
        }

        fclose(resultado);

        if (situacao == 2) {
            for (int i = qtdLinhas - 1; i >= 0; i--) {
                fputs(linhas[i], arqOrd);
                fputc('\n', arqOrd);
                free(linhas[i]);
            }
        } else {
            for (int i = 0; i < qtdLinhas; i++) {
                fputs(linhas[i], arqOrd);
                fputc('\n', arqOrd);
                free(linhas[i]);
            }
        }

        free(linhas);
    } else {
        FILE *arqTexto = fopen(arquivo, "r");
        if (!arqTexto) {
            printf("Erro ao abrir %s para cópia.\n", arquivo);
            fclose(arqOrd);
            remove(nomeTemporario);
            return;
        }

        int escritos = 0;
        while (escritos < quantidade && leRegistro(arqTexto, &reg)) {
            fprintf(arqOrd, "%08ld %05.1f %s %s %s\n",
                    reg.numero,
                    reg.nota,
                    reg.estado,
                    reg.cidade,
                    reg.curso);
            escritos++;
        }

        fclose(arqTexto);
    }

    fclose(arqOrd);
    remove(nomeTemporario);
}
