/* GABRIEL DA SILVA MARQUES FERREIRA 2210442 3WA*/
/* PEDRO AUGUSTO VILLARINHO DOS SANTOS 2211358 3WA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilalinb.h"

#define MAXVAR 4
#define MAXPARAM 2
#define MAXLINHAS 50

unsigned int r10_ops1[] = {0xe2, 0xea, 0xf2, 0xfa};
unsigned int r10_ops2[] = {0xd4, 0xd5, 0xd6, 0xd7};
unsigned int varp_conditions[] = {0xfc, 0xfd, 0xfe, 0xff};
unsigned int var_local[] = {0xca, 0xc0, 0xbc, 0xb8};

int line = 1;

static void error(const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

funcp compilaLinB(FILE *f, unsigned char codigo[]) {
  int i = 0;
  int num_desvios = 0;
  int desvios[MAXLINHAS];
  long labels[MAXLINHAS];

  // Configuração inicial da pilha

  //  push %rbp
  codigo[i++] = 0x55;
  // movq %rsp %rbp
  codigo[i++] = 0x48;
  codigo[i++] = 0x89;
  codigo[i++] = 0xe5;

  // subq $48, %rsp
  codigo[i++] = 0x48;
  codigo[i++] = 0x83;
  codigo[i++] = 0xec;
  codigo[i++] = 0x30;

  codigo[i++] = 0x4c;codigo[i++] = 0x89;codigo[i++] = 0x65;codigo[i++] = 0xf8; // mov %r12, -0x8(%rbp),
  codigo[i++] = 0x4c;codigo[i++] = 0x89;codigo[i++] = 0x6d;codigo[i++] = 0xea; // mov %r13, -0x16(%rbp)
  codigo[i++] = 0x4c;codigo[i++] = 0x89;codigo[i++] = 0x75;codigo[i++] = 0xdc; // mov %r14, -0x24(%rbp)
  codigo[i++] = 0x4c;codigo[i++] = 0x89;codigo[i++] = 0x7d;codigo[i++] = 0xce;  // mov %r15, -0x32(%rbp)

  char c;
  while ((c = fgetc(f)) != EOF) {
    labels[line] = i;
    switch (c) {
    case 'r': { // retorno
      char c0;
      if (fscanf(f, "et%c", &c0) != 1) {
        error("comando invalido", line);
      }
      printf("ret\n");
      // movq %r12d %eax
      codigo[i++] = 0x44;
      codigo[i++] = 0x89;
      codigo[i++] = 0xe0;

      // mov -0x8(%rbp), %r12
      codigo[i++] = 0x4c;
      codigo[i++] = 0x8b;
      codigo[i++] = 0x65;
      codigo[i++] = 0xf8; 

      // mov -0x16(%rbp), %r13
      codigo[i++] = 0x4c;
      codigo[i++] = 0x8b;
      codigo[i++] = 0x6d;
      codigo[i++] = 0xea;

      // mov -0x24(%rbp), %r14
      codigo[i++] = 0x4c;
      codigo[i++] = 0x8b;
      codigo[i++] = 0x75;
      codigo[i++] = 0xdc;

      // mov -0x32(%rbp), %r15
      codigo[i++] = 0x4c;
      codigo[i++] = 0x8b;
      codigo[i++] = 0x7d;
      codigo[i++] = 0xce; 

      // leave e ret
      codigo[i++] = 0xc9;
      codigo[i++] = 0xc3;
      break;
    }
    case 'v':   
    case 'p': { // atribuicao
      int idx0, idx1, idx2;
      char var0 = c, var1, var2, op;
      if (fscanf(f, "%d <= %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2,
                 &idx2) != 6) {
        error("comando invalido detectado", line);
      }
     
      int boolvar0 = (var0 == 'v');
      int boolvar1 = (var1 == 'v');
      int boolvar2 = (var2 == 'v');

      if (var1 == '$') {
        codigo[i++] = 0x41;
        codigo[i++] = 0xba; // movl %r10d, constante
        *((int *)&codigo[i]) = idx1;
        i += 4; // $idx1
      } else {
        codigo[i++] = boolvar1 ? 0x45 : 0x41;
        codigo[i++] = 0x89; // mov %r10d, registrador
        codigo[i++] =
            boolvar1 ? r10_ops1[idx1 - 1]
                   : r10_ops1[idx1 % 2 + 2]; 
      }

      switch (op) {
      case '+': {
        if (var2 == '$') {
          codigo[i++] = 0x41;
          codigo[i++] = 0x81;
          codigo[i++] = 0xc2; // add %r10d, constante
          *((int *)&codigo[i]) = idx2;
          i += 4; // $idx2
          break;
        }
        codigo[i++] = boolvar2 ? 0x45 : 0x41;
        codigo[i++] = 0x01; // add %r10d
        codigo[i++] =
        boolvar2 ? r10_ops1[idx2 - 1] : r10_ops1[idx2 % 2 + 2]; 
        break;
      }
      case '-': {
        if (var2 == '$') {
          codigo[i++] = 0x41;
          codigo[i++] = 0x81;
          codigo[i++] = 0xea; // sub %r10d
          *((int *)&codigo[i]) = idx2;
          i += 4; // $idx2
          break;
        }
        codigo[i++] = boolvar2 ? 0x45 : 0x41;
        codigo[i++] = 0x29; // sub %r10d
        codigo[i++] =
            boolvar2 ? r10_ops1[idx2 - 1] : r10_ops1[idx2 % 2 + 2]; // hex do registrador
        break;
      }
      case '*': {
        if (var2 == '$') {
          codigo[i++] = 0x45;
          codigo[i++] = 0x69;
          codigo[i++] = 0xd2; // imul %r10d
          *((int *)&codigo[i]) = idx2;
          i += 4; // $idx2
          break;
        }
        codigo[i++] = boolvar2 ? 0x45 : 0x44;
        codigo[i++] = 0x0f;
        codigo[i++] = 0xaf; // imul %r10d
        codigo[i++] = boolvar2 ? r10_ops2[idx2 - 1] : r10_ops2[idx0 % 2 + 2]; // hex do registrador
        break;
      }
      }
      codigo[i++] = boolvar0 ? 0x45 : 0x44;
      codigo[i++] = 0x89; // mov $r10d
      codigo[i++] =
          boolvar0 ? r10_ops2[idx0 - 1] : r10_ops2[idx0 % 2 + 2]; // hex do registrador
      if (boolvar0) {
        
        codigo[i++] = 0x44;
        codigo[i++] = 0x89;
        codigo[i++] = 0x65 + 8 * (idx0 - 1);
        codigo[i++] = var_local[idx0 - 1];
        
      }
      
      printf("%c%d = %c%d %c %c%d\n", var0, idx0, var1, idx1, op, var2, idx2);
      break;
    }
    case 'i': { // desvio
      char var0;
      int idx0, num;
      if (fscanf(f, "f %c%d %d", &var0, &idx0, &num) != 3) {
        error("comando invalido detectado", line);
      }
     
      int isVar = (var0 == 'v');

      // cmp
      if (isVar)
        codigo[i++] = 0x41;
      codigo[i++] = 0x83;
      codigo[i++] = isVar ? varp_conditions[idx0 - 1] : varp_conditions[idx0 % 2 + 2];
      codigo[i++] = 0x00;

      // jne
      codigo[i++] = 0x0f;
      codigo[i++] = 0x85;
      for (int k = 0; k < 4; k++)
        codigo[i++] = 0x00;

      desvios[num_desvios++] = num;
      printf("if %c%d %d\n", var0, idx0, num);
      break;
    }
    default:
      error("comando invalido detectado", line);
    }
    line++;
    if (line > MAXLINHAS)
      error("atingiu o máximo de linhas", line);
    fscanf(f, " ");
  }
  int jmp = 0;
  for (int k = 0; k < i; k++) {
    if (codigo[k] == 0x0f && codigo[k + 1] == 0x85) {
      int destino = desvios[jmp++];
      int offset = labels[destino] - (k + 6);
      *((int *)&codigo[k + 2]) = offset;
    }
  }

  return (funcp)codigo;
}
