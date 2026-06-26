#ifndef FITAS_H
#define FITAS_H
#include <stdio.h>

#define FITAS_ENTRADA 20
#define FITAS_SAIDA 20
#define TOTAL_FITAS FITAS_ENTRADA + FITAS_SAIDA

int ChamarCriadorFitas();
void fecharFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]);
bool criarFitas(FILE* entrada[FITAS_ENTRADA], FILE* saida[FITAS_SAIDA]);

#endif
