#include <stdio.h>
#include <stdlib.h>
#include "codifica.h"

void compacta(FILE *arqTexto, FILE *arqBin, struct compactadora *v) {
    char c;
    unsigned int buffer = 0;
    int bufferLength = 0;
    
    while (fread(&c, 1, 1, arqTexto)) {  // Lê um caractere do arquivo de texto
        for (int i = 0; i < 32; i++) {   // Procura na tabela de compactação
            if (v[i].simbolo == c) {
                buffer = (buffer << v[i].tamanho) | v[i].codigo;  // Adiciona ao buffer
                bufferLength += v[i].tamanho;

                while (bufferLength >= 8) {  // Se o buffer tem pelo menos 8 bits, escreve no arquivo
                    char byte = (buffer >> (bufferLength - 8)) & 0xFF;
                    fwrite(&byte, 1, 1, arqBin);
                    bufferLength -= 8;
                }
                break;
            }
        }
    }

    // Processar EOT
    buffer = (buffer << v[31].tamanho) | v[31].codigo;
    bufferLength += v[31].tamanho;
    while (bufferLength > 0) {  // Escreve os bits restantes
        char byte = (buffer >> (bufferLength - 8)) & 0xFF;
        if (bufferLength < 8) {
            byte &= (0xFF >> (8 - bufferLength));  // Máscara para zerar os bits mais significativos
        }
        fwrite(&byte, 1, 1, arqBin);
        bufferLength -= 8;
    }
}

void descompacta(FILE *arqBin, FILE *arqTexto, struct compactadora *v) {
    int byte, bit;
    unsigned int codigoAtual = 0;
    int tamanhoAtual = 0;
    int encontrado;

    while ((byte = fgetc(arqBin)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            bit = (byte >> i) & 1;
            codigoAtual = (codigoAtual << 1) | bit;
            tamanhoAtual++;

            encontrado = 0;  // 'encontrado' inicializado como falso (0)
            for (int j = 0; j < 32; j++) {
                if (v[j].tamanho == tamanhoAtual && v[j].codigo == codigoAtual) {
                    if (v[j].simbolo == 4) { // EOT
                        return;
                    }
                    fputc(v[j].simbolo, arqTexto);
                    codigoAtual = 0;
                    tamanhoAtual = 0;
                    encontrado = 1;  // 'encontrado' é verdadeiro (1)
                    break;
                }
            }
            if (!encontrado && tamanhoAtual == 32) { // Limite máximo de bits para segurança
                codigoAtual = 0;
                tamanhoAtual = 0;
            }
        }
    }
}
