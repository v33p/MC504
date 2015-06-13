/* 
   Task 2 - filesystem.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Codigo geral sobre file system. Utilizado tanto pelo mkfs quanto pelo bash
   para construcao de um filesystem.
*/

#include "filesystem.h"

// error 
void error (const char* message) {
  printf ("\n[ERROR]: %s\n", message);
  exit (EXIT_FAILURE);
}

// warning
void warning (const char* message) {
  printf ("\n[WARNING] %s\n", message);
}

// createFileSystem
Filesystem createFileSystem (int32_t block_size) {
  Filesystem fs = malloc (sizeof (filesystem));
  int32_t number_of_blocks = FILE_SIZE / block_size;
  fs->superblock = createSuperBlock (block_size);
  fs->inode_bitmap = createBitmap (MAX_INODES);
  fs->inode_bitmap->map[0] = 1;
  fs->datablock_bitmap = createBitmap (number_of_blocks);
  adjustInitialFileSystem (fs, block_size);
  for (int32_t i = 1; i < MAX_INODES; i++)
    fs->inodes[i] = NULL;
  fs->inodes[0] = createInode (0, -1, 111, "", "", 1);
  fs->first_datablock = NULL;
  //printf ("fs\n");
  return fs;
}

// createSuperBlock
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

// createBitmap
Bitmap createBitmap (int32_t size) {
  char* map = malloc (size * sizeof (char));
  Bitmap bitmap = malloc (sizeof (bitmap));
  bitmap->map = map;
  //printf ("bm\n");
  return bitmap;
}

// createInode
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

// adjustInitialFileSystem
void adjustInitialFileSystem (Filesystem fs, int32_t block_size) {
  int32_t inodes_per_block = block_size / INODE_SIZE;
  int32_t blocks_inodes = (MAX_INODES + inodes_per_block - 1) / inodes_per_block; //Pega teto
  int32_t blocks_inode_bitmap = (MAX_INODES + block_size - 1) / block_size; //Pega teto
  int32_t blocks_datablock_bitmap = ((FILE_SIZE / block_size) + block_size - 1) / block_size; //Pega teto
  int32_t blocks_superblock = 1;
  fs->superblock->number_of_blocks = blocks_superblock + blocks_inode_bitmap + blocks_datablock_bitmap + blocks_inodes;
  for (int32_t i = 0; i < fs->superblock->number_of_blocks; i++)
    fs->datablock_bitmap->map[i] = 1; 
}

// filesystemToFile
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

  int32_t soi32 = sizeof (int32_t);
  
  block->id = atual;
  setIntAtBlock (0, block, fs->superblock->magic_number);
  setIntAtBlock (soi32, block, fs->superblock->root_position);
  setIntAtBlock (soi32*2, block, fs->superblock->number_of_inodes);
  setIntAtBlock (soi32*3, block, fs->superblock->number_of_blocks);
  setIntAtBlock (soi32*4, block, fs->superblock->block_size);
  writeBlock (atual, file, block, fs->superblock->block_size);

  //printf ("superblock\n");
  
  // INODE BITMAP
  block->id = ++atual;
  clearBlock (block);
  
  setStringAtBlock (0, block, MIN(bsize, MAX_INODES), fs->inode_bitmap->map);
  writeBlock (atual, file, block, bsize);
  if (bsize < MAX_INODES) {
    block->id = ++atual;
    clearBlock (block);
    setStringAtBlock (0, block, bsize, fs->inode_bitmap->map + bsize);
    writeBlock (atual, file, block, bsize);
  }

  //printf ("inode bitmap\n");

  // DATABLOCK BITMAP
  for (i = 0; i < total_blocks/bsize; i++) {
    block->id = ++atual;
    clearBlock (block);
    
    setStringAtBlock (0, block, bsize, fs->datablock_bitmap->map + (i*bsize));
    writeBlock (atual, file, block, bsize);
  }
  if (total_blocks % bsize != 0) {
    block->id = ++atual;
    clearBlock (block);

    setStringAtBlock (0, block, (total_blocks % bsize), fs->datablock_bitmap->map + ((total_blocks/bsize) * bsize));
    writeBlock (atual, file, block, bsize);
  }

  //printf ("datablock bitmap\n");

  // INODES
  int32_t inodes_per_block = bsize / INODE_SIZE; 
  int32_t total_blocks_inodes = MAX_INODES / inodes_per_block;
  /*
  Inode root = fs->inodes[fs->superblock->root_position];

  block->id = ++atual;
  clearBlock (block);

  setInodeAtBlock (0, block, root);
  // escrevendo root
  writeBlock (atual, file, block, bsize);
  
  // zerando conteudo
  for (i = 0; i < bsize; i++)
    block->content[i] = 0;
  // escrevendo outros inodes
  for (i = 1; i < total_blocks_inodes; i++) {
    block->id = ++atual;
    clearBlock (block);
    
    writeBlock (atual, file, block, bsize);
  }
  */
  Inode inode;
  
  for (i = 0; i < total_blocks_inodes; i++) {
    block->id = ++atual;
    clearBlock (block);
    for (j = 0; j < inodes_per_block; j++) {
      if (fs->inodes[(i * inodes_per_block)+j] != NULL) {
	inode = fs->inodes[(i * inodes_per_block)+j];
	setInodeAtBlock ((j*INODE_SIZE), block, inode);
      }
    }
    writeBlock (atual, file, block, bsize);
  }

  //printf ("inodes\n");

  printFilesystem (fs);
  
  fclose (file);
}

// fileToFilesystem
Filesystem fileToFilesystem (char* file_name) {
  FILE* file = fopen (file_name, "r");

  int32_t soi32 = sizeof (int32_t);
  int32_t current = 0;

  int32_t i;
  
  // CHECANDO FILESYSTEM
  int32_t magic_number;
  fread (&magic_number, soi32, 1, file); 
  if (magic_number != 119785)
    error ("Magic Number mismatch! Cannot read this filesystem"); 

  // pegando tamanho do bloco
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
  
  // INODE BITMAP
  clearBlock (block);
  current++;
  block = readBlock (current, file, block_size);
  getStringAtBlock (0, block, MIN(block_size, MAX_INODES), fs->inode_bitmap->map);
  if (block_size < MAX_INODES) {
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
  int32_t inodes_per_block = block_size / INODE_SIZE;
  int32_t total_blocks_inodes = MAX_INODES / inodes_per_block;
  int32_t value;
  Inode inode;

  for (i = 0; i < total_blocks_inodes; i++) {
    current++;
    clearBlock (block);
    block = readBlock (current, file, block_size);
    for (j = 0; j < inodes_per_block; j++) {
      value = getIntAtBlock((j*INODE_SIZE), block);
      // pega os inodes cujo inode number eh diferente da posicao do inode 
      if (value != (i * inodes_per_block)+j) {
	inode = malloc (sizeof (inode));
	getInodeAtBlock ((j*INODE_SIZE), block, inode);
	fs->inode[(i * inodes_per_block) + j] = inode;
      }
    }
  }

  printFilesystem (fs);
  
  fclose (file);
  return fs;
}


// readBlock
Datablock readBlock (int32_t id, FILE* file, int32_t block_size) {
  Datablock datablock = malloc (sizeof (datablock));
  datablock->id = id;
  fseek (file, id * block_size, SEEK_SET);
  fread (datablock->content, sizeof (char), block_size, file);
  return datablock;
}

// writeBlock
void writeBlock (int32_t id, FILE* file, Datablock datablock, int32_t block_size) {
  fseek (file, id * block_size, SEEK_SET);
  fwrite (datablock->content, sizeof (char), block_size, file); 
}

// FUNCOES AUXILIARES

// getIntAtBlock
int32_t getIntAtBlock (int32_t position, Datablock block) {
  int32_t value;
  memcpy ((void*) &value, block->content + position, sizeof (int32_t));
  return value;
}

// setIntAtBlock
void setIntAtBlock (int32_t position, Datablock block, int32_t value) {
  memcpy (block->content + position, (void*) &value, sizeof (int32_t));
}

// getStringAtBlock
void getStringAtBlock (int32_t position, Datablock block, int32_t size, char* string) {
  memcpy (string, block->content + position, size * sizeof (char));
}

// setStringAtBlock
void setStringAtBlock (int32_t position, Datablock block, int32_t size, char* string) {
  memcpy (block->content + position, (void*) string, size * sizeof (char));
}

// clearBlock
void clearBlock (Datablock block) {
 for (int i = 0; i < MAX_BLOCK_SIZE; i++)
    block->content[i] = 0;
}

// setInodeAtBlock
void setInodeAtBlock (int32_t position, Datablock block, Inode inode) {
  int32_t soi32 = sizeof (int32_t);
  setIntAtBlock (position, block, inode->number);
  setIntAtBlock (position+soi32, block, inode->father);
  setIntAtBlock (position+(soi32*2), block, inode->permition);
  setIntAtBlock (position+(soi32*3), block, inode->timestamp);
  setStringAtBlock (position+(soi32*4), block, INODE_TYPE_SIZE, inode->type);
  setStringAtBlock (position+(soi32*4)+INODE_TYPE_SIZE, block, INODE_NAME_SIZE, inode->name);
  setStringAtBlock (position+(soi32*4)+INODE_TYPE_SIZE+INODE_NAME_SIZE, block, 1, &inode->dir);
  setIntAtBlock (position+(soi32*4)+INODE_TYPE_SIZE+INODE_NAME_SIZE+1, block, inode->number_of_blocks);
  // criar funcoa pra isso depois
  memcpy (block->content+position+(soi32*5)+INODE_TYPE_SIZE+INODE_NAME_SIZE+1, (void*) inode->blocks, BLOCKS_PER_INODE * soi32);
}

// getInodeAtBlock
void getInodeAtBlock (int32_t position, Datablock block, Inode inode) {
  int32_t soi32 = sizeof (int32_t);
  inode->number = getIntAtBlock (position, block);
  inode->father = getIntAtBlock (position+soi32, block);
  inode->permition = getIntAtBlock (position+(2*soi32), block);
  inode->timestamp = getIntAtBlock (position+(3*soi32), block);
  getStringAtBlock (position+(4*soi32), block, INODE_TYPE_SIZE, inode->type);
  getStringAtBlock (position+(4*soi32)+INODE_TYPE_SIZE, block, INODE_NAME_SIZE, inode->name);
  getStringAtBlock (position+(4*soi32)+INODE_TYPE_SIZE+INODE_NAME_SIZE, block, 1, &inode->dir);
  inode->number_of_blocks = getIntAtBlock (position+(4*soi32)+INODE_TYPE_SIZE+INODE_NAME_SIZE+1, block);
  // criar funcao pra isso depois
  memcpy ((void *) inode->blocks, block->content+position+(soi32*5)+INODE_TYPE_SIZE+INODE_NAME_SIZE+1, BLOCKS_PER_INODE * soi32);
}

// PRINTERS

// printSuperblock
void printSuperblock (Superblock sb) {
  printf ("Superblock:\n");
  printf ("Magic Number: %d\n", sb->magic_number);
  printf ("Root Position: %d\n", sb->root_position);
  printf ("Inodes in use: %d\n", sb->number_of_inodes);
  printf ("Blocks in use: %d\n", sb->number_of_blocks);
  printf ("Block Size: %d\n", sb->block_size);
}

// printBitmap
void printBitmap (Bitmap bm, int32_t size) {
  printf ("Bitmap:\n");
  for (int32_t i = 0; i < size; i++)
    printf ("%c ", bm->map[i]);
  printf ("\n");
}

// printInode
void printInode (Inode inode) {
  printf ("Inode %d:\n", inode->number);
  printf ("father: %d\n", inode->father);
  printf ("permitions: %d\n", inode->permition);
  printf ("timestamp: %d\n", inode->timestamp);
  printf ("Name: ");
  for (int32_t i = 0; i < INODE_NAME_SIZE; i++)
    if (inode->name[i] != 0) printf("%c", inode->name[i]);
  printf (".");
  for (int32_t i = 0; i < INODE_TYPE_SIZE; i++)
    if (inode->type[i] != 0) printf("%c", inode->type[i]);
  printf ("\ndir: %c\n", inode->dir);
  printf ("number of blocks: %d\n", inode->number_of_blocks);
}

// printAllInodes
void printAllInodes (Filesystem fs) {
  for (int32_t i = 0; i < MAX_INODES; i++)
    if (fs->inodes[i] != NULL) printInode (fs->inodes[i]);
}

// printFileSystem
void printFilesystem (Filesystem fs) {
  printSuperblock (fs->superblock);
  printBitmap (fs->inode_bitmap, MAX_INODES);
  printBitmap (fs->datablock_bitmap, FILE_SIZE / fs->superblock->block_size);
  printAllInodes (fs);
}

// TODO: Para todo codigo de create precisamos criar um codigo de free;
// TODO: FilesystemToFile
// TODO: FileToFilesystem
