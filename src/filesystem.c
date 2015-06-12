/* 
   Task 2 - filesystem.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Codigo geral sobre file system. Utilizado tanto pelo mkfs quanto pelo bash
   para construcao de um filesystem.
*/

#include "filesystem.h"

// Error 
void error (const char* message) {
  printf ("\n[ERROR]: %s\n", message);
  exit (EXIT_FAILURE);
}

// Warning
void warning (const char* message) {
  printf ("\n[WARNING] %s\n", message);
}

// CreateFileSystem
Filesystem createFileSystem (int32_t block_size) {
  Filesystem fs = malloc (sizeof (filesystem));
  int32_t number_of_blocks = FILE_SIZE / block_size;
  fs->superblock = createSuperBlock (block_size);
  fs->inode_bitmap = createBitmap (1024);
  fs->inode_bitmap->map[0] = 1;
  fs->datablock_bitmap = createBitmap (number_of_blocks);
  for (int32_t i = 1; i < MAX_INODES; i++)
    fs->inodes[i] = NULL;
  fs->inodes[0] = createInode (0, -1, 111, "", "", 1);
  fs->first_datablock = NULL;
  //printf ("fs\n");
  return fs;
}

// CreateSuperBlock
Superblock createSuperBlock (int32_t block_size) {
  Superblock superblock = malloc (sizeof (superblock));
  superblock->magic_number = 119785; // nao sei como configurar o magic number
  superblock->root_position = 0;
  superblock->number_of_inodes = 1;
  superblock->number_of_blocks = 0; // precisa calcular
  superblock->block_size = block_size;
  //printf ("sb\n");
  return superblock;
}

// CreateBitmap
Bitmap createBitmap (int32_t size) {
  char* map = malloc (size * sizeof (char));
  Bitmap bitmap = malloc (sizeof (bitmap));
  bitmap->map = map;
  //printf ("bm\n");
  return bitmap;
}

// CreateInode
Inode createInode (int32_t number, int32_t father, int32_t permition, char* type, char* name, char dir) {
  Inode inode = malloc (sizeof (inode));
  inode->number = number;
  inode->father = father;
  inode->permition = permition;
  inode->timestamp = (int32_t) time(NULL);
  strcpy (inode->type, type);
  strcpy (inode->name, name);
  inode->dir = dir;
  inode->number_of_blocks = 0;
  for (int32_t i = 0; i < BLOCKS_PER_INODE; i++)
    inode->blocks[i] = -1;
  //printf ("i\n");
  return inode;
}

// FilesystemToFile
void filesystemToFile (Filesystem fs, char* file_name) {
  FILE* file = fopen (file_name, "w");
  int32_t i, j;
  if (file == NULL) error ("Null file.");
  for (i = 0; i < FILE_SIZE; i++)
    fputc (0, file);
  
  // SUPERBLOCK
  Datablock block = malloc (sizeof (datablock));
  int32_t bsize = fs->superblock->block_size;
  int32_t atual = 0;
  int32_t total_blocks = FILE_SIZE / bsize;
  block->id = atual;
  memcpy (block->content, (void*) &fs->superblock->magic_number, sizeof (int32_t));
  memcpy (block->content+4, (void*) &fs->superblock->root_position, sizeof (int32_t));
  memcpy (block->content+8, (void*) &fs->superblock->number_of_inodes, sizeof (int32_t));
  memcpy (block->content+12, (void*) &fs->superblock->number_of_blocks, sizeof (int32_t));
  memcpy (block->content+16, (void*) &fs->superblock->block_size, sizeof (int32_t));
  writeBlock (atual, file, block, fs->superblock->block_size);

  printf ("superblock\n");
  
  // INODE BITMAP
  block->id = ++atual;
  memcpy (block->content, (void*) fs->inode_bitmap->map, MIN(bsize, 1024));
  writeBlock (atual, file, block, fs->superblock->block_size);
  if (bsize < 1024) {
    block->id = ++atual;
    memcpy (block->content, (void*) fs->inode_bitmap->map+bsize, bsize);
    writeBlock (atual, file, block, bsize);
  }

  printf ("inode bitmap\n");

  // DATABLOCK BITMAP
  for (i = 0; i < total_blocks/bsize; i++) {
    block->id = ++atual;
    memcpy (block->content, (void*) fs->datablock_bitmap->map + (i*bsize), bsize);
    writeBlock (atual, file, block, bsize);
  }
  if (total_blocks % bsize != 0) {
    block->id = ++atual;
    memcpy (block->content, (void*) fs->datablock_bitmap->map + ((total_blocks/bsize) * bsize), total_blocks % bsize);
    // resto do bloco eh lixo
    for (j = total_blocks % bsize; j < bsize; j++)
      block->content[j] = 0;
    writeBlock (atual, file, block, bsize);
  }

  printf ("datablock bitmap\n");

  // INODES
  int32_t inodes_per_blocks = bsize / INODE_SIZE; 
  int32_t total_blocks_inodes = MAX_INODES / inodes_per_blocks;
  Inode root = fs->inodes[fs->superblock->root_position];
  block->id = ++atual;
  memcpy (block->content, (void*) &root->number, sizeof (int32_t));
  memcpy (block->content + 4, (void*) &root->father, sizeof (int32_t));
  memcpy (block->content + 8, (void*) &root->permition, sizeof (int32_t));
  memcpy (block->content + 12, (void*) &root->timestamp, sizeof (int32_t));
  memcpy (block->content + 16, (void*) root->type, INODE_TYPE_SIZE * sizeof (char));
  memcpy (block->content + (16 + INODE_TYPE_SIZE), (void*) root->name, INODE_NAME_SIZE * sizeof (char));
  memcpy (block->content + (16 + INODE_TYPE_SIZE + INODE_NAME_SIZE), (void*) &root->dir, sizeof (char));
  memcpy (block->content + (17 + INODE_TYPE_SIZE + INODE_NAME_SIZE), (void*) &root->number_of_blocks, sizeof (int32_t));
  memcpy (block->content + (21 + INODE_TYPE_SIZE + INODE_NAME_SIZE), (void*) root->blocks, BLOCKS_PER_INODE * sizeof (int32_t));
  for (i = (21 + INODE_TYPE_SIZE + INODE_NAME_SIZE + (4 * BLOCKS_PER_INODE)); i < bsize; i++)
    block->content[i] = 0;
  
  // escrevendo root
  writeBlock (atual, file, block, bsize);
  
  // zerando conteudo
  for (i = 0; i < bsize; i++)
    block->content[i] = 0;
  // escrevendo outros inodes
  for (i = 1; i < total_blocks_inodes; i++) {
    block->id = ++atual;
    writeBlock (atual, file, block, bsize);
  }

  printf ("inodes\n");

  /*
  fs->superblock->number_of_blocks = atual;
  for (i = 0; i < atual; i++)
    fs->datablock_bitmap->map[i] = 1;
  */
  
  
  //printf ("%s\n", superblock->content);
  fclose (file);
}

// FileToFilesystem
Filesystem fileToFilesystem (char* file_name) {
  FILE* file = fopen (file_name, "r");

  int32_t soi32 = sizeof (int32_t);
  int32_t current = 0;

  int32_t i;
  
  // CHECANDO FILESYSTEM
  int32_t magic_number;
  fread (&magic_number, soi32, 1, file); 
  if (magic_number != 119785)
    error ("Cannot read this filesystem"); 

  
  int32_t block_size;
  //void* pointer = &block_size; 
  fseek (file, soi32 * 4, SEEK_SET);
  fread (&block_size, soi32, 1, file); 
  Filesystem fs = createFileSystem (block_size);
  fseek (file, 0, SEEK_SET);

  // SUPERBLOCK
  Datablock block = readBlock (current, file, block_size);
  fs->superblock->root_position = getIntAtBlock (soi32, block);
  fs->superblock->number_of_inodes = getIntAtBlock (soi32*2, block);
  fs->superblock->number_of_blocks = getIntAtBlock (soi32*3, block);

  /*printf ("Testando: %d %d %d %d %d\n", fs->superblock->magic_number,
	  fs->superblock->root_position,
	  fs->superblock->number_of_inodes,
	  fs->superblock->number_of_blocks,
	  fs->superblock->block_size);
  */
  
  // INODE BITMAP
  clearBlock (block);
  current++;
  block = readBlock (current, file, block_size);
  getStringAtBlock (0, block, MIN(block_size, 1024), fs->inode_bitmap->map);
  if (block_size < 1024) {
    current++;
    block = readBlock (current, file, block_size);
    getStringAtBlock (0, block, block_size, fs->inode_bitmap->map+block_size);
  }

  // DATABLOCK BITMAP
  clearBlock (block);
  int32_t total_blocks = FILE_SIZE / block_size;
  for (i = 0; i < total_blocks/block_size; i++) {
    current++;
    clearBlock (block);
    block = readBlock (current, file, block_size);
    getStringAtBlock (0, block, block_size, fs->datablock_bitmap->map + (i * block_size));
  }
  if (total_blocks % block_size != 0) {
    current++;
    clearBlock (block);
    getStringAtBlock (0, block, (total_blocks % block_size), fs->datablock_bitmap->map + ((total_blocks / block_size) * block_size));
  }
  
  // INODE
  clearBlock (block);
  
  
  // COMPLETAR
  
  
  
  fclose (file);
  return fs;
}


// ReadBlock
Datablock readBlock (int32_t id, FILE* file, int32_t block_size) {
  Datablock datablock = malloc (sizeof (datablock));
  datablock->id = id;
  fseek (file, id * block_size, SEEK_SET);
  fread (datablock->content, sizeof (char), block_size, file);
  return datablock;
}

// WriteBlock
void writeBlock (int32_t id, FILE* file, Datablock datablock, int32_t block_size) {
  fseek (file, id * block_size, SEEK_SET);
  fwrite (datablock->content, sizeof (char), block_size, file); 
}

// FUNCOES AUXILIARES

int32_t getIntAtBlock (int32_t position, Datablock block) {
  int32_t value;
  memcpy ((void*) &value, block->content + position, sizeof (int32_t));
  return value;
}

void setIntAtBlock (int32_t position, Datablock block, int32_t value) {
  memcpy (block->content + position, (void*) &value, sizeof (int32_t));
}

void getStringAtBlock (int32_t position, Datablock block, int32_t size, char* string) {
  memcpy (string, block->content + position, size * sizeof (char));
}

void setStringAtBlock (int32_t position, Datablock block, int32_t size, char* string) {
  memcpy (block->content + position, (void*) string, size * sizeof (char));
}

void clearBlock (Datablock block) {
  for (int i = 0; i < MAX_BLOCK_SIZE; i++)
    block->content[i] = 0;
}

// TODO: Para todo codigo de create precisamos criar um codigo de free;
// TODO: FilesystemToFile
// TODO: FileToFilesystem
