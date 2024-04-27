#include <stdio.h>
#include <stdlib.h>
#include "codifica.h"


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
    Node *root = createNode('\0');
    for (int i = 0; i < 32; i++) {
        insert(root, v[i].codigo, v[i].tamanho, v[i].simbolo);
    }
    
    Node *current = root;
    int bit, byte;
    while ((byte = fgetc(arqBin)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            bit = (byte >> i) & 1;
            current = bit ? current->right : current->left;
            if (current->simbolo != '\0') {
                if (current->simbolo == 4) { // EOT
                    return;
                }
                fputc(current->simbolo, arqTexto);
                current = root; // Reset to root for the next character
            }
        }
    }
}

void freeTree(Node *root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}
