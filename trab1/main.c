#include <stdio.h>
#include <stdlib.h>
#include "codifica.h"

int main() {
    FILE *arqTexto = fopen("teste1.txt", "r");
    FILE *arqBin = fopen("teste1.bin", "wb");

    struct compactadora tabela[32] = {
        {'K', 1, 12},
        {'W', 2, 12},
        {'Y', 3, 12},
        {'Z', 2, 11},
        {'J', 3, 11},
        {'X', 1, 9},
        {'\n', 1, 8},
        {'H', 1, 7},
        {'F', 36, 7},
        {'G', 37, 7},
        {'B', 1, 6},
        {'Q', 16, 6},
        {',', 17, 6},
        {'V', 19, 6},
        {'.', 49, 6},
        {'P', 48, 6},
        {'D', 1, 5},
        {'C', 17, 5},
        {'O', 16, 5},
        {'M', 25, 5},
        {'R', 1, 4},
        {'L', 7, 4},
        {'N', 6, 4},
        {'A', 5, 4},
        {'T', 9, 4},
        {'S', 14, 4},
        {'U', 15, 4},
        {'I', 13, 4},
        {'E', 1, 3},
        {' ', 5, 3},
        {0, 0, 0},  // Supondo ser um controle ou espaço reservado
        {4, 0, 12}  // Mantendo a linha conforme você especificou
    };

    if (!arqTexto || !arqBin) {
        perror("Erro ao abrir os arquivos");
        return EXIT_FAILURE;
    }

    compacta(arqTexto, arqBin, tabela);

    fclose(arqTexto);
    fclose(arqBin);

    arqBin = fopen("teste1.bin", "rb");
    FILE *arqTextoOut = fopen("teste1_out.txt", "w");

    if (!arqBin || !arqTextoOut) {
        perror("Erro ao abrir os arquivos");
        return EXIT_FAILURE;
    }

    descompacta(arqBin, arqTextoOut, tabela);

    fclose(arqBin);
    fclose(arqTextoOut);

    FILE *arqBin2 = fopen("teste2.bin", "rb");
    FILE *arqTextoOut2 = fopen("teste2_out.txt","w");

    if (!arqBin2 || !arqTextoOut2) {
        perror("Erro ao abrir os arquivos");
        return EXIT_FAILURE;
    }

    descompacta(arqBin2, arqTextoOut2, tabela);

    fclose(arqBin2);
    fclose(arqTextoOut2);

    return EXIT_SUCCESS;
}