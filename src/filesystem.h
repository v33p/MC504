/* 
   Task 2 - filesystem.h
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Codigo geral sobre file system. Utilizado tanto pelo mkfs quanto pelo bash
   para construcao de um filesystem.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* DEFINITIONS */
#define FILE_SIZE 25165824        // 24Mb
#define MAX_BLOCK_SIZE 4096       // 4Kb
#define MIN_BLOCK_SIZE 512        // 512b
#define MAX_MAP_SIZE 1024         // 1Kb
#define MAX_INODES 1024           // 1Kb
#define MAX_BLOCKS_PER_INODE 1024 // 1Kb

/* ENUM */
typedef enum {false = 0, true = 1} Bool;

/* STRUCTS */

// superblock
typedef struct superblock {
  int magic_number;      // identificador do filesystem
  int root_position;     // posicao do diretorio root
  int number_of_inodes;  // numero de inodes em uso
  int number_of_blocks;  // numero de blocos em uso
} superblock, *Superblock;

// datablock 
typedef struct datablock {
  char *content;  // conteudo
  struct datablock *next; // bloco subsequente
  struct datablock *prev; // bloco anterior
} datablock, *Datablock;

// bitmap 
typedef struct bitmap {
  Bool *map; // vetor de booleanos = mapa de bits
} bitmap, *Bitmap;

// inode
typedef struct inode {
  int number;               // numero de identficacao 0 - 1023
  struct inode *father;     // METADADO: apontador para o pai
  int permition;            // METADADO: valor da permissao do arquivo
  int timestamp;            // METADADO: time stamp convertido pra int 
  char *type;               // METADADO: tipo do dado
  char *name;               // METADADO: nome do arquivo
  Datablock datablocks;     // Vetor de data blocks (?)
} inode, *Inode;

// file system
typedef struct filesystem {
  Superblock superblock;    // 16 b
  Bitmap inode_bitmap;      // 1024 b
  Bitmap datablock_bitmap;  // x b => x = total - estrutura / datablocks
  Inode inodes[MAX_INODES]; // 1024 * inodes b
  Datablock datablock_list; // resto
  // (importante: esse resto nao pode ser menor do que 88% do tamanho total)
} filesystem, *Filesystem;


/* FUNCTIONS */



/*
  Error: Imprime uma mensagem para debug e corta a execucao do programa.
  param:
    const char* message = mensagem de debug
 */
void error (const char* message);

/*
  Warning: Similar a funcao error, essa funcao imprime uma mensagem para
debug, porem nao corta a execucao do programa.
  param:
    const char* message = mensagem para debug
 */
void warning (const char* message);


