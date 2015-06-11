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
  fs->inodes[0] = fs->superblock->root_position;
  fs->first_datablock = NULL;
  //printf ("fs\n");
  return fs;
}

// CreateSuperBlock
Superblock createSuperBlock (int block_size) {
  Superblock superblock = malloc (sizeof (superblock));
  superblock->magic_number = 0; // nao sei como configurar o magic number
  superblock->root_position = createInode (0, NULL, 111, "", "", true);
  superblock->number_of_inodes = 1;
  superblock->number_of_blocks = 0; // precisa calcular
  superblock->block_size = block_size;
  //printf ("sb\n");
  return superblock;
}

// CreateBitmap
Bitmap createBitmap (int size) {
  Bool* map = malloc (size * sizeof (Bool));
  Bitmap bitmap = malloc (sizeof (bitmap));
  bitmap->map = map;
  //printf ("bm\n");
  return bitmap;
}

// CreateInode
Inode createInode (int number, Inode father, int permition, char* type, char* name, Bool dir) {
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
  if (file == NULL) error ("Null file.");
  for (int i = 0; i < FILE_SIZE; i++)
    fputc ('0', file);
  Datablock superblock = malloc (sizeof (datablock));
  superblock->id = 0;
  copyIntToCharArray (superblock->content, &fs->superblock->magic_number);
  //copyIntToCharArray (superblock->content+4, &fs->superblock->root_position);
  copyIntToCharArray (superblock->content+8, &fs->superblock->number_of_inodes);
  copyIntToCharArray (superblock->content+12, &fs->superblock->number_of_blocks);
  copyIntToCharArray (superblock->content+16, &fs->superblock->block_size);
  // COMPLETAR
  printf ("%s\n", superblock->content);
  writeBlock (0, file, superblock, fs->superblock->block_size);
  //printf ("%s\n", superblock->content);
  fclose (file);
}

// FileToFilesystem
Filesystem fileToFilesystem (char* file_name) {
  FILE* file = fopen (file_name, "r");
  int block_size;
  //void* pointer = &block_size; 
  fseek (file, sizeof (int) * 5, SEEK_END);
  fread (&block_size, sizeof (int), 1, file); 
  Filesystem fs = createFileSystem (block_size);
  fseek (file, 0, SEEK_END);
  // COMPLETAR
  fclose (file);
  return fs;
}


// ReadBlock
Datablock readBlock (int id, FILE* file, int block_size) {
  Datablock datablock = malloc (sizeof (datablock));
  datablock->id = id;
  fseek (file, id * block_size, SEEK_END);
  fread (datablock->content, sizeof (char), block_size, file);
  return datablock;
}

// WriteBlock
void writeBlock (int id, FILE* file, Datablock datablock, int block_size) {
  fseek (file, id * block_size, SEEK_END);
  fwrite (datablock->content, sizeof (char), block_size, file); 
}

// FUNCOES AUXILIARES

void copyIntToCharArray (char* array, int* value) {
  for (int i = 0; i < 4; i++) {
    array[i] = (char) *(value + i);
  }
}

// TODO: Para todo codigo de create precisamos criar um codigo de free;
// TODO: FilesystemToFile
// TODO: FileToFilesystem
