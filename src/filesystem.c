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
Filesystem createFileSystem (int datablock_size) {
  Filesystem fs = malloc (sizeof (filesystem));
  int number_of_blocks = FILE_SIZE / datablock_size;
  fs->superblock = createSuperBlock (datablock_size);
  fs->inode_bitmap = createBitmap (1024);
  fs->datablock_bitmap = createBitmap (number_of_blocks);
  for (int i = 1; i < MAX_INODES; i++) {
    fs->inodes[i] = NULL;
  }
  fs->inodes[0] = fs->superblock->root_position;
  fs->first_datablock = NULL;
  return fs;
}

// CreateSuperBlock
Superblock createSuperBlock (int datablock_size) {
  Superblock superblock = malloc (sizeof (superblock));
  superblock->magic_number = 0; // nao sei como configurar o magic number
  superblock->root_position = createInode (0, NULL, 111, "", "", true);
  superblock->number_of_inodes = 1;
  superblock->number_of_blocks = 0; // precisa calcular
  superblock->datablock_size = datablock_size;
}

// CreateBitmap
Bitmap createBitmap (int size) {
  Bool* map = malloc (size * sizeof (Bool));
  Bitmap bitmap = malloc (sizeof (bitmap));
  bitmap->map = map;
  return bitmap;
}

// CreateInode
Inode createInode (int number, Inode father, int permition, char* type, char* name, Bool dir) {
  Inode inode = malloc (sizeof (inode));
  inode->number = number;
  inode->father = father;
  inode->permition = permition;
  inode->timestamp = (int) time(NULL);
  inode->type = type;
  inode->name = name;
  inode->dir = dir;
  inode->number_of_blocks = 0;
  for (int i = 0; i < BLOCKS_PER_INODE; i++) inode->blocks[i] = NULL;
  return inode;
}

// FilesystemToFile
void filesystemToFile (Filesystem fs, char* file_name) {
  FILE* file;
  file = fopen (file_name, "w");
  if (file == NULL) error ("Null file.");
  
  // COMPLETAR
  
}

// FileToFilesystem
Filesystem fileToFilesystem (char* file_name) {
  Filesystem fs = malloc (sizeof (filesystem));
  
  // COMPLETAR

  return fs;
}


// ReadBlock
Datablock readBlock (int number, FILE *file) {
  Datablock datablock = malloc (sizeof (datablock));
  datablock->id = number;

  // COMPLETAR
  
  return datablock;
}

// WriteBlock
void writeBlock (int number, FILE *file, Datablock datablock) {

  // COMPLETAR
  
}


// TODO: Para todo codigo de create precisamos criar um codigo de free;
// TODO: FilesystemToFile
// TODO: FileToFilesystem
