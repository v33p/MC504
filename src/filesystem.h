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
#define DATA_BLOCKS_SIZE 22020096 // 21Mb
#define MAX_OVERHEAD 3145728      // 3072Kb

/* ENUM */
typedef enum {false = 0, true = 1} Bool;

/* STRUCTS */

// superblock
typedef struct superblock {
  int magic_number;      // identificador do filesystem
  int root_position;     // posicao do diretorio root
  int number_of_inodes;  // numero de inodes em uso
  int number_of_blocks;  // numero de blocos em uso
  int datablock_size;    // tamanho do bloco de dado
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
  Bool dir;                 // true = inode dir ou false = inode file
  int number_of_blocks;     // Numero de blocks
  void* blocks;             // Lista de data blocks (?)
} inode, *Inode;

// file system
typedef struct filesystem {
  Superblock superblock;    // 20b
  Bitmap inode_bitmap;      // 1024b
  Bitmap datablock_bitmap;  // X b
  Inode inodes[MAX_INODES]; // (16 + 3-4 + 20 + 4 + (4 * 200))b por inode em media 
  Datablock first_datablock; // resto => X * Y = 21Mb
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

/*
  CreateFileSystem: Cria uma estrutura Filesystem com os valores padroes e com
blocos de tamanho 'datablock_size' tomando cuidado para nao ultrapassar o 
overhead.
  param:
    int datablock_size = tamanho de cada bloco
 */
Filesystem createFileSystem (int datablock_size);

/*
  CreateSuperBlock: Cria uma estrutura Superblock com os valores padroes e com
um total de 'number_of_blocks' de blocos de tamanho 'datablock_size'. 
  param:
    int number_of_blocks = numero total de blocos do filesystem
    int datablock_size = tamanho dos blocos do filesystem
 */
Superblock createSuperBlock (int number_of_blocks, int datablock_size);

/*
  CreateBitmap: Cria um bitmap com vetor de Bool do tamanho de 'size'.
  param:
    int size = tamanho do vetor do bitmap
 */
Bitmap createBitmap (int size);

/*
  CreateInode: Cria um inode com alguns parametros.
  param:
    int number = identificador unico do inode no filesystem
    Inode father = apontador para o pai desse inode
    int permition = variavel de controle de permissao
    char* type = tipo do inode
    char* name = nome do inode para consumo humano
    Bool dir = variavel booleana para indicar se e diretorio ou nao
 */
Inode createInode (int number, Inode father, int permition, char* type, char* name, Bool dir);

/*
  CreateDataBlock: Cria um datablock atrelado com os datablocks anterior
e posterior deles na lista de blocos do inode.
  param:
    Datablock next = apontador para o proximo bloco da lista
    Datablock prev = apontador para o bloco anterior da lista
 */
Datablock CreateDataBlock (Datablock next, Datablock prev);

/*
  FilesystemToFile: Dado uma estrutura de filesystem transforma ela num
arquivo passado como parametro para funcao.
  param:
    Filesystem fs = apontador para o fs que ira transformar em file
    char* file_name = nome do arquivo que deseja salvar o fs
 */
void filesytemToFile (Filesystem fs, char* file_name);

/*
  FileToFilesystem: Dado um arquivo 'file_name' entrar dentro dele e 
construir, a partir dele, uma estrutura de filesystem para iteragir
com bash.fs.
  param:
    char* file_name = nome do arquivo no qual se transformara em fs
 */
Filesystem fileToFilesystem (char* file_name);
