#ifndef __CODIFICA__
#define __CODIFICA__

#include <stdio.h>

struct compactadora { 
    char simbolo; 
    unsigned int codigo; 
    int tamanho; };

void compacta(FILE *arqTexto, FILE *arqBin, struct compactadora *v);
void descompacta(FILE *arqBin, FILE *arqTexto, struct compactadora *v);

#endif