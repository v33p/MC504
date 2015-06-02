/* 
   Task 2 - mkfs.h
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do mkfs do nosso filesystem.
*/

#include "filesystem.c"

/* DEFINITIONS */


/* ENUM */


/* STRUCTS */


/* FUNCTIONS */

/*
  Main: Funcao que chama os metodos iniciais do programa.
 */
int main (int argc, char** argv);

/*
  Parse Input: Trata a entrada recebida pelo programa, chamando as funcoes
necessarias para tratar determinado problema.
  param:
    int argc = tamanho do vetor de strings
    char** argv = vetor de strings de entrada
 */
void parseInput (int argc, char** argv);

/*
  
 */
int parseSize (char* string);
