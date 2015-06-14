/* 
   Task 2 - filesystem.h
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Codigo geral sobre file system. Utilizado tanto pelo mkfs quanto pelo bash
   para construcao de um filesystem.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* DEFINITIONS */
#define FILE_SIZE 25165824        // 24Mb
#define MAX_OVERHEAD 3145728      // 3072Kb = 3Mb
#define MAX_BLOCK_SIZE 4096       // 4Kb
#define MIN_BLOCK_SIZE 512        // 512b
#define MAX_INODES 1024           // 1Kb
#define MAX_BLOCKS_PER_INODE 1024 // 1Kb
#define BLOCKS_PER_INODE 30       // 
#define INODE_SIZE 185           //  
#define INODE_TYPE_SIZE 4
#define INODE_NAME_SIZE 40

/**/
#define MIN(a,b) ((a)>(b) ? (b) : (a)) 
#define MAX(a,b) ((a)<(b) ? (b) : (a))

/* ENUM */
typedef enum {false = 0, true = 1} Bool;

/* STRUCTS */

// superblock
typedef struct superblock {
  int32_t magic_number;      // identificador do filesystem
  int32_t root_position;   // apontador para diretorio root
  int32_t number_of_inodes;  // numero de inodes em uso
  int32_t number_of_blocks;  // numero de blocos em uso
  int32_t block_size;    // tamanho do bloco de dado
} superblock, *Superblock;

// bitmap 
typedef struct bitmap {
  char* map; // vetor de booleanos = mapa de bits
} bitmap, *Bitmap;

// inode
typedef struct inode {
  int32_t number;                    // numero de identficacao 0 - 1023
  int32_t father;          // METADADO: apontador para o pai
  int32_t permition;                 // METADADO: valor da permissao do arquivo
  int32_t timestamp;                 // METADADO: time stamp convertido pra int 
  char type[INODE_TYPE_SIZE];    // METADADO: tipo do dado
  char name[INODE_NAME_SIZE];    // METADADO: nome do arquivo
  char dir;                      // true = inode dir ou false = inode file
  int32_t number_of_blocks;          // Numero de blocks
  int32_t blocks[BLOCKS_PER_INODE];  // Lista de data blocks (?)
} inode, *Inode;

// datablock
typedef struct datablock {
  int32_t id;
  char content[MAX_BLOCK_SIZE];
} datablock, *Datablock;

// file system
typedef struct filesystem {
  Superblock superblock;    // 20B
  Bitmap inode_bitmap;      // 1024B
  Bitmap datablock_bitmap;  // X B
  Inode inodes[MAX_INODES]; // 
  Datablock first_datablock;    // 
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
blocos de tamanho 'block_size' tomando cuidado para nao ultrapassar o 
overhead.
  param:
    int32_t block_size = tamanho de cada bloco
 */
Filesystem createFileSystem (int32_t block_size);

/*
  CreateSuperBlock: Cria uma estrutura Superblock com os valores padroes e com
um total de 'number_of_blocks' de blocos de tamanho 'block_size'. 
  param:
    int32_t number_of_blocks = numero total de blocos do filesystem
    int32_t block_size = tamanho dos blocos do filesystem
 */
Superblock createSuperBlock (int32_t block_size);

/*
  CreateBitmap: Cria um bitmap com vetor de char do tamanho de 'size'.
  param:
    int32_t size = tamanho do vetor do bitmap
 */
Bitmap createBitmap (int32_t size);

/*
  CreateInode: Cria um inode com alguns parametros.
  param:
    int32_t number = identificador unico do inode no filesystem
    int32_t father = apontador para o pai desse inode
    int32_t permition = variavel de controle de permissao
    char* type = tipo do inode
    char* name = nome do inode para consumo humano
    char dir = variavel booleana para indicar se e diretorio ou nao
 */
Inode createInode (int32_t number, int32_t father, int32_t permition, char* type, char* name, char dir);

/*
  
 */
void adjustInitialFileSystem (Filesystem fs, int32_t block_size);

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

/*
  ReadBlock: Dado um id de um bloco e um file do filesystem, essa 
funcao cria um datablock e retorna.
  param:
    int32_t id = indice do bloco que quer acessar
    FILE* file = arquivo que representa o hd do filesystem
 */
Datablock readBlock (int32_t id, FILE* file, int32_t block_size);

/*
  WriteBlock: Dado um id de um bloco, um file do filesystem e um
datablock, essa funcao sublistitui o valor do datablock no bloco
cujo id e passado como parametro.
  param:
    int32_t id = indice do bloco que ira ser substituido
    FILE* file = arquivo que representa o hd do filesystem
    Datablock datablock = bloco que substituira o bloco antigo.
 */
void writeBlock (int32_t id, FILE* file, Datablock datablock, int32_t block_size);

/*
 */


// FUNCOES AUXILIARES

/*
  
 */
int32_t getIntAtBlock (int32_t position, Datablock block);
/*
 */
void setIntAtBlock (int32_t position, Datablock block, int32_t value);
/*
 */
void getStringAtBlock (int32_t position, Datablock block, int32_t size, char* string);
/*
 */
void setStringAtBlock (int32_t position, Datablock block, int32_t size, char* string);
/*
 */
void setInodeAtBlock (int32_t position, Datablock block, Inode inode);
/*
 */
void getInodeAtBlock (int32_t position, Datablock block, Inode inode);
/*
 */
void clearBlock (Datablock block);

// PRINTERS

/*
 */
void printSuperblock (Superblock superblock);

/*
 */
void printBitmap (Bitmap bm, int32_t size);

/*
 */
void printInode (Inode inode);

/*
 */
void printAllInodes (Filesystem fs);

/*
 */
void printFilesystem (Filesystem fs);
