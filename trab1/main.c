#include <stdio.h>
#include <stdlib.h>
#include "codifica.h"

void hex_dump(FILE *fp) {
    unsigned char buffer[16];  // Buffer para armazenar os dados lidos
    size_t bytes_lidos;
    int i;

    if (fp == NULL) {
        fprintf(stderr, "Erro: ponteiro de arquivo inválido.\n");
        return;
    }

    // Lê o arquivo em blocos de 16 bytes
    while ((bytes_lidos = fread(buffer, 1, 16, fp)) > 0) {
        // Mostra cada byte em hexadecimal
        for (i = 0; i < bytes_lidos; i++) {
            printf("%02x ", buffer[i]);
        }
        printf("\n");
    }
}

typedef struct node {
    char simbolo;
    struct node* left;
    struct node* right;
} Node;

Node* createNode(char simbolo) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->simbolo = simbolo;
    newNode->left = newNode->right = NULL;
    return newNode;
}

void insert(Node *root, unsigned int codigo, int tamanho, char simbolo) {
    Node *current = root;
    for (int i = tamanho - 1; i >= 0; i--) {
        int bit = (codigo >> i) & 1;
        if (bit == 0) {
            if (!current->left) {
                current->left = createNode('\0');
            }
            current = current->left;
        } else {
            if (!current->right) {
                current->right = createNode('\0');
            }
            current = current->right;
        }
    }
    current->simbolo = simbolo;
}

void freeTree(Node *root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

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
        {0, 0, 0},  
        {4, 0, 12}  
    };

    if (!arqTexto || !arqBin) {
        perror("Erro ao abrir os arquivos");
        return EXIT_FAILURE;
    }
    compacta(arqTexto, arqBin, tabela);


    fclose(arqTexto);
    fclose(arqBin);

    arqBin = fopen("teste1.bin","rb");

    hex_dump(arqBin);
    fclose(arqBin);

    arqBin = fopen("teste1.bin", "rb");
    FILE *arqTextoOut = fopen("teste1_out.txt", "w");

    if (!arqBin || !arqTextoOut) {
        perror("Erro ao abrir os arquivos");
        return EXIT_FAILURE;
    }

    descompacta2(arqBin, arqTextoOut, tabela);

    fclose(arqBin);
    fclose(arqTextoOut);

    FILE *arqBin2 = fopen("teste2.bin", "rb");
    FILE *arqTextoOut2 = fopen("teste2_out.txt","w");

    if (!arqBin2 || !arqTextoOut2) {
        perror("Erro ao abrir os arquivos");
        return EXIT_FAILURE;
    }

    descompacta2(arqBin2, arqTextoOut2, tabela);

    fclose(arqBin2);
    fclose(arqTextoOut2);

    return EXIT_SUCCESS;
}
