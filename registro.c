#include "registro.h"

COMP RegistroCompara(Registro r1, Registro r2) {
    
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
        char nota[6];//nora tem 5 caracteres + \0 (ex: "06.70 + \0")

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

void imprimirRegistros(FILE* arquivo){
    Registro reg;
    while(leRegistro(arquivo, &reg))
        RegistroPrint(reg);
}

//transforma o arquivo texto em binario para facilitar a leitura e escrita durante os processos de ordenação
FILE* criaArquivoBinario(FILE* arquivoTexto, const char* nomeArquivoBinario) {
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "wb");
    if (!arquivoBinario) {
        printf("Erro ao criar o arquivo binário.\n");
        return NULL;
    }

    Registro reg;
    while (leRegistro(arquivoTexto, &reg)) {
        fwrite(&reg, sizeof(Registro), 1, arquivoBinario);
    }

    fclose(arquivoBinario);
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
    while (fread(&reg, sizeof(Registro), 1, arquivoBinario)) {
        fprintf(arquivoSaida, "%08ld %05.2f %s %s %s\n", reg.numero, reg.nota, reg.estado, reg.cidade, reg.curso);
    }

    fclose(arquivoSaida);
    return fopen(nomeArquivoSaida, "rb");
}