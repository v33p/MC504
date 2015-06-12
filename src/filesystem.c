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
Filesystem createFileSystem (int block_size) {
  Filesystem fs = malloc (sizeof (filesystem));
  int number_of_blocks = FILE_SIZE / block_size;
  fs->superblock = createSuperBlock (block_size);
  fs->inode_bitmap = createBitmap (1024);
  fs->datablock_bitmap = createBitmap (number_of_blocks);
  for (int i = 1; i < MAX_INODES; i++)
    fs->inodes[i] = NULL;
  fs->inodes[0] = createInode (0, -1, 111, "", "", 1);
  fs->first_datablock = NULL;
  //printf ("fs\n");
  return fs;
}

// CreateSuperBlock
Superblock createSuperBlock (int block_size) {
  Superblock superblock = malloc (sizeof (superblock));
  superblock->magic_number = 0; // nao sei como configurar o magic number
  superblock->root_position = 0;
  superblock->number_of_inodes = 1;
  superblock->number_of_blocks = 0; // precisa calcular
  superblock->block_size = block_size;
  //printf ("sb\n");
  return superblock;
}

// CreateBitmap
Bitmap createBitmap (int size) {
  char* map = malloc (size * sizeof (char));
  Bitmap bitmap = malloc (sizeof (bitmap));
  bitmap->map = map;
  //printf ("bm\n");
  return bitmap;
}

// CreateInode
Inode createInode (int number, int father, int permition, char* type, char* name, char dir) {
  Inode inode = malloc (sizeof (inode));
  inode->number = number;
  inode->father = father;
  inode->permition = permition;
  inode->timestamp = (int) time(NULL);
  strcpy (inode->type, type);
  strcpy (inode->name, name);
  inode->dir = dir;
  inode->number_of_blocks = 0;
  for (int i = 0; i < BLOCKS_PER_INODE; i++)
    inode->blocks[i] = -1;
  //printf ("i\n");
  return inode;
}

// FilesystemToFile
void filesystemToFile (Filesystem fs, char* file_name) {
  FILE* file = fopen (file_name, "w");
  int i, j;
  if (file == NULL) error ("Null file.");
  for (i = 0; i < FILE_SIZE; i++)
    fputc (0, file);
  
  // SUPERBLOCK
  Datablock block = malloc (sizeof (datablock));
  int bsize = fs->superblock->block_size;
  int atual = 0;
  int total_blocks = FILE_SIZE / bsize;
  block->id = atual;
  memcpy (block->content, (void*) &fs->superblock->magic_number, sizeof (int));
  memcpy (block->content+4, (void*) &fs->superblock->root_position, sizeof (int));
  memcpy (block->content+8, (void*) &fs->superblock->number_of_inodes, sizeof (int));
  memcpy (block->content+12, (void*) &fs->superblock->number_of_blocks, sizeof (int));
  memcpy (block->content+16, (void*) &fs->superblock->block_size, sizeof (int));
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
  int inodes_per_blocks = bsize / INODE_SIZE; 
  int total_blocks_inodes = MAX_INODES / inodes_per_blocks;
  Inode root = fs->inodes[fs->superblock->root_position];
  block->id = ++atual;
  memcpy (block->content, (void*) &root->number, sizeof (int));
  memcpy (block->content+4, (void*) &root->father, sizeof (int));
  memcpy (block->content+8, (void*) &root->permition, sizeof (int));
  memcpy (block->content+12, (void*) &root->timestamp, sizeof (int));
  memcpy (block->content+16, (void*) root->type, INODE_TYPE_SIZE * sizeof (char));
  memcpy (block->content+ (16 + INODE_TYPE_SIZE), (void*) root->name, INODE_NAME_SIZE * sizeof (char));
  memcpy (block->content+ (16 + INODE_TYPE_SIZE + INODE_NAME_SIZE), (void*) &root->dir, sizeof (char));
  memcpy (block->content + (17 + INODE_TYPE_SIZE + INODE_NAME_SIZE), (void*) &root->number_of_blocks, sizeof (int));
  memcpy (block->content + (21 + INODE_TYPE_SIZE + INODE_NAME_SIZE), (void*) root->blocks, BLOCKS_PER_INODE * sizeof (int));
  for (i = (21 + INODE_TYPE_SIZE + INODE_NAME_SIZE + (4 * BLOCKS_PER_INODE)); i < bsize; i++)
    block->content[i] = 0;

  printf ("suruba\n");
  
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
  
  //printf ("%s\n", superblock->content);
  fclose (file);
}

// FileToFilesystem
Filesystem fileToFilesystem (char* file_name) {
  FILE* file = fopen (file_name, "r");
  int block_size;
  //void* pointer = &block_size; 
  fseek (file, sizeof (int) * 5, SEEK_SET);
  fread (&block_size, sizeof (int), 1, file); 
  Filesystem fs = createFileSystem (block_size);
  fseek (file, 0, SEEK_SET);
  // COMPLETAR
  fclose (file);
  return fs;
}


// ReadBlock
Datablock readBlock (int id, FILE* file, int block_size) {
  Datablock datablock = malloc (sizeof (datablock));
  datablock->id = id;
  fseek (file, id * block_size, SEEK_SET);
  fread (datablock->content, sizeof (char), block_size, file);
  return datablock;
}

// WriteBlock
void writeBlock (int id, FILE* file, Datablock datablock, int block_size) {
  fseek (file, id * block_size, SEEK_SET);
  fwrite (datablock->content, sizeof (char), block_size, file); 
}

// FUNCOES AUXILIARES

void copyIntToCharArray (char* array, int* value) {
  for (int i = 0; i < 4; i++) {
    array[i] = (char) *(value + (i * sizeof(char)));
  }
}

// TODO: Para todo codigo de create precisamos criar um codigo de free;
// TODO: FilesystemToFile
// TODO: FileToFilesystem
