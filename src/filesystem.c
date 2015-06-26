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
  printf ("\n[WARNING]: %s\n", message);
}

// createFileSystem
Filesystem createFileSystem (int32_t block_size, char* file_name) {
  Filesystem fs = malloc (sizeof (filesystem));
  int32_t number_of_blocks = FILE_SIZE / block_size;
  char* name = malloc ((strlen (file_name) + 1) * sizeof (char));
  strcpy (name, file_name);
  fs->file_name = name;
  fs->file_name[strlen (file_name)] = '\0';
  fs->superblock = createSuperBlock (block_size);
  fs->inode_bitmap = createBitmap (MAX_INODES);
  fs->inode_bitmap->map[0] = 1;
  fs->datablock_bitmap = createBitmap (number_of_blocks);
  fs->current_dir = 0;
  adjustInitialFileSystem (fs, block_size);
  for (int32_t i = 1; i < MAX_INODES; i++)
    fs->inodes[i] = NULL;
  fs->inodes[0] = createInode (fs, 0, -1, 111, "", "", 1);
  fs->first_datablock = -1;
  fs->first_inodeblock = -1;
  return fs;
}

// createSuperBlock
Superblock createSuperBlock (int32_t block_size) {
  Superblock superblock = malloc (sizeof (superblock));
  superblock->magic_number = 119785; // nao sei como configurar o magic number
  superblock->root_position = 0;
  superblock->number_of_inodes = 0;
  superblock->number_of_blocks = 0; // precisa calcular
  superblock->block_size = block_size;
  superblock->number_of_dir = 1;
  return superblock;
}

// createBitmap
Bitmap createBitmap (int32_t size) {
  char* map = malloc (size * sizeof (char));
  Bitmap bitmap = malloc (sizeof (bitmap));
  bitmap->map = map;
  return bitmap;
}

Inode createEmptyInode (Filesystem fs, int32_t number) {
  if (fs->superblock->number_of_blocks < MAX_INODES) {
    Inode inode = malloc (sizeof (inode));
    int32_t i;
    inode->number = number;
    inode->timestamp = (int32_t) time(NULL);
    for (i = 0; i < INODE_NAME_SIZE; i++)
      inode->name[i] = '\0';
    for (i = 0; i < INODE_TYPE_SIZE; i++)
      inode->type[i] = '\0';
    inode->number_of_blocks = 0;
    for (i = 0; i < BLOCKS_PER_INODE; i++)
      inode->blocks[i] = -1;
    fs->inode_bitmap->map[number] = 1;
    fs->inodes[number] = inode;
    fs->superblock->number_of_inodes++;
    return inode;
  }
  else {
    warning ("There is no more inodes.");
    return NULL;
  }
}

// createInode
Inode createInode (Filesystem fs, int32_t number, int32_t father, int32_t permition, char* type, char* name, char dir) {
  Inode inode = createEmptyInode (fs, number);
  if (inode != NULL) {
    inode->father = father;
    inode->permition = permition;
    strcpy (inode->type, type);
    strcpy (inode->name, name);
    inode->dir = dir;
  }
  return inode;
}

void createInode2 (Inode child, int32_t number, int32_t father, int32_t permition, char* type, char* name, char dir, int32_t* ni, int32_t* nd) {
	int32_t i = 0;
	
	child->number = number;
	child->father = father;
	child->permition = permition;
	child->timestamp = (int32_t) time(NULL);
	strcpy(child->name, name);
	strcpy(child->type, type);
	for(i=0;i<BLOCKS_PER_INODE;i++){
		child->blocks[i] = -1;
	}
	child->dir = dir;
	if (dir == 1)
		(*nd)++;
	(*ni)++;
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
  setIntAtBlock (soi32*5, block, fs->superblock->number_of_dir);
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

  //printf ("ibm\n");

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

  //printf ("dbbm\n");
  
  // INODES
  int32_t inodes_per_block = bsize / INODE_SIZE;
  int32_t total_blocks_inodes = MAX_INODES / inodes_per_block;

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

  //printf ("inodes");
  
  //printFilesystem (fs);

  fclose (file);
}

// fileToFilesystem
Filesystem fileToFilesystem (char* file_name) {
  FILE* file = fopen (file_name, "r");

  if (file == NULL) error("Could not open filesystem (invalid pathname)!");

  int32_t soi32 = sizeof (int32_t);
  int32_t current = 0;

  int32_t i, j;

  // CHECANDO FILESYSTEM
  int32_t magic_number;
  fread (&magic_number, soi32, 1, file);
  if (magic_number != 119785)
    error ("Magic Number mismatch! Cannot read this filesystem");

  // pegando tamanho do bloco
  int32_t block_size;
  fseek (file, soi32 * 4, SEEK_SET);
  fread (&block_size, soi32, 1, file);
  Filesystem fs = createFileSystem (block_size, file_name);

  /* if (fs->inodes[0] != NULL) {
    Inode root = fs->inodes[0];
    fs->inodes[0] = NULL;
    free (root);
    }*/

  fseek (file, 0, SEEK_SET);

  // SUPERBLOCK
  Datablock block = readBlock (current, file, block_size);
  fs->superblock->root_position = getIntAtBlock (soi32, block);
  fs->superblock->number_of_inodes = getIntAtBlock (soi32*2, block);
  fs->superblock->number_of_blocks = getIntAtBlock (soi32*3, block);
  fs->superblock->number_of_dir = getIntAtBlock (soi32*5, block);

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

  fs->first_inodeblock = current+1;
  
  for (i = 0; i < total_blocks_inodes; i++) {
    current++;
    clearBlock (block);
    block = readBlock (current, file, block_size);
    for (j = 0; j < inodes_per_block; j++) {
      value = getIntAtBlock((j*INODE_SIZE), block);
      // pega os inodes cujo inode number eh diferente da posicao do inode
      if (value == (i * inodes_per_block)+j) {
	inode = malloc (sizeof (inode));
	getInodeAtBlock ((j*INODE_SIZE), block, inode);
	fs->inodes[(i * inodes_per_block) + j] = inode;
      }
    }
  }

  fs->first_datablock = current+1;
  
  //printFilesystem (fs);

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

// readBlocktoBlock
void readBlocktoBlock (Datablock datablock, int32_t block_size, FILE* file) {
  fseek (file, datablock->id * block_size, SEEK_SET);
  fread (datablock->content, sizeof (char), block_size, file);
  fseek (file, 0, SEEK_SET);
}

// other readBlock
Datablock readBlockByFilesystem (int32_t id, Filesystem fs) {
  Datablock datablock = malloc (sizeof (datablock));
  FILE* file = fopen (fs->file_name, "r");
  datablock->id = id;
  fseek (file, id * fs->superblock->block_size, SEEK_SET);
  fread (datablock->content, sizeof (char), fs->superblock->block_size, file);
  fclose (file);
  return datablock;
}

// writeBlock
void writeBlock (int32_t id, FILE* file, Datablock datablock, int32_t block_size) {
  fseek (file, id * block_size, SEEK_SET);
  fwrite (datablock->content, sizeof (char), block_size, file);
}

// other writeBlock
void writeBlockByFilesystem (int32_t id, Filesystem fs, Datablock datablock) {
  FILE* file = fopen (fs->file_name, "r+");
  fseek (file, id * fs->superblock->block_size, SEEK_SET);
  fwrite (datablock->content, sizeof (char), fs->superblock->block_size, file);
  fclose (file);
}

// getFreeInode
int32_t getFreeInode (Filesystem fs) {
  int32_t i;
  for (i = 0; i < MAX_INODES; i++)
    if (fs->inode_bitmap->map[i] == 0)
            break;
  return i;
}

int32_t getFreeInodeNumber (char* ib){
	
	int32_t i;

		for (i = 0; i < MAX_INODES; i++)
			if (ib[i] == 0) {
				ib[i] = 1;
				return i;
			}
				
	return -1;
	
}

// freeInode
void freeInode (Filesystem fs, Inode inode) {
  int32_t i;
  fs->superblock->number_of_inodes--;
  fs->inode_bitmap->map[inode->number] = 0;
  fs->inodes[inode->number] = NULL;
  int32_t* array = getBlocksFromInode (fs, inode);
  if (inode->dir == 0) {
    for (i = 0; i < inode->number_of_blocks; i++) {
      Datablock block = readBlockByFilesystem (array[i], fs);
      freeDatablock (fs, block);
    }
  }
  else {
    for (i = 0; i < inode->number_of_blocks; i++) {
      freeInode (fs, fs->inodes[i]);
    }
  }
  // free memory
  free (inode);
  free (array);
}

// getFreeDatablock
void getFreeDatablock (Filesystem fs, Datablock block) {
  int32_t i;
  int32_t total_of_blocks = FILE_SIZE / fs->superblock->block_size;
  if (fs->superblock->number_of_blocks < total_of_blocks -1) {
    for (i = 0; i < total_of_blocks; i++)
      if (fs->datablock_bitmap->map[i] == 0) break;
    fs->datablock_bitmap->map[i] = 1;
    fs->superblock->number_of_blocks++;
    block->id = i;
  }
  else
    warning ("There is no more datablocks.");
}

int32_t getFreeDatablock2 (char* dbb, int32_t block_size, int32_t* nb) {
  int32_t i;
  int32_t total_of_blocks = FILE_SIZE / block_size;
  if (*nb < total_of_blocks -1) {
	for (i = 0; i < total_of_blocks; i++)
      if (dbb[i] == 0) break;
    dbb[i] = 1;
    (*nb)++;
    return i;
  }
  else {
    warning ("There is no more datablocks.");
	return -1;
  }
}

// freeDatablock
void freeDatablock (Filesystem fs, Datablock block) {
  // falta tratar o inode que continha o bloco
  fs->superblock->number_of_blocks--;
  fs->datablock_bitmap->map[block->id] = 0;
  clearBlock (block);
  writeBlockByFilesystem (block->id, fs, block);
  // free memory?
  free (block);
}

// isInDir
int32_t isInDir(char* child, Inode dir, Filesystem fs){
    int32_t i;

    if(dir == NULL)
        return -3; //dir NULL
    if(dir->dir != 1)
        return -2; //dir nao eh diretorio
    // nao esta tratando caso de indirecao
    for(i=0;i<dir->number_of_blocks;i++){
        if(strcmp(fs->inodes[dir->blocks[i]]->name, child) == 0)
            return fs->inodes[dir->blocks[i]]->number;
    }
    return -1; //Nao esta no diretorio
}

//
Inode searchInodeOnDirByName (Filesystem fs, Inode dir, char* name) {
  int32_t inodes_per_indirection_block = (fs->superblock->block_size / sizeof (int32_t)) -1;
  int32_t i;
  //int32_t number_of_indirection_blocks = (dir->number_of_blocks - BLOCKS_PER_INODE-1) / inodes_per_indirection_block;
  Datablock block;
  if(dir == NULL) {
    warning("Null pointer as Inode");
    return NULL;
  }
  if (dir->dir == 0) {
    warning ("Not a diretory");
    return NULL;
  }
  for (i = 0; i < dir->number_of_blocks; i++) {
    if (i < (BLOCKS_PER_INODE -1)) {
      if (strcmp (fs->inodes[dir->blocks[i]]->name, name) == 0) {
	return fs->inodes[dir->blocks[i]];
      }
    }
    else {
      if (i == (BLOCKS_PER_INODE-1)) {
	block = readBlockByFilesystem (dir->blocks[BLOCKS_PER_INODE-1], fs);
      }
      else if ((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block == 0) {
	free (block);
	block = readBlockByFilesystem (getIntAtBlock (inodes_per_indirection_block * sizeof (int32_t), block), fs);
      }
      if (strcmp (fs->inodes[getIntAtBlock (((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block)]->name, name) == 0)
	return fs->inodes[getIntAtBlock (((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block)];
    }
  }
  return NULL;
}

//
Inode searchInodeOnDirByValue (Filesystem fs, Inode dir, int32_t value) {
  int32_t inodes_per_indirection_block = (fs->superblock->block_size / sizeof (int32_t)) -1;
  int32_t i;
  //int32_t number_of_indirection_blocks = (dir->number_of_blocks - BLOCKS_PER_INODE-1) / inodes_per_indirection_block;
  Datablock block;
  if (dir->dir == 0) {
    warning ("Not a diretory");
    return NULL;
  }
  for (i = 0; i < dir->number_of_blocks; i++) {
    if (i < (BLOCKS_PER_INODE -1)) {
      if (dir->blocks[i] == value) {
	return fs->inodes[dir->blocks[i]];
      }
    }
    else {
      if (i == (BLOCKS_PER_INODE-1)) {
	block = readBlockByFilesystem (dir->blocks[BLOCKS_PER_INODE-1], fs);
      }
      else if ((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block == 0) {
	free (block);
	block = readBlockByFilesystem (getIntAtBlock (inodes_per_indirection_block * sizeof (int32_t), block), fs);
      }
      if (getIntAtBlock (((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block) == value)
	return fs->inodes[getIntAtBlock (((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block)];
    }
  }
  return NULL;
}

//
int32_t searchBlockOnInodeByValue (Filesystem fs, Inode inode, int32_t value) {
  int32_t inodes_per_indirection_block = (fs->superblock->block_size / sizeof (int32_t)) -1;
  int32_t i;
  //int32_t number_of_indirection_blocks = (inode->number_of_blocks - BLOCKS_PER_INODE-1) / inodes_per_indirection_block;
  Datablock block;
  for (i = 0; i < inode->number_of_blocks; i++) {
    if (i < (BLOCKS_PER_INODE -1)) {
      if (inode->blocks[i] == value) {
	return inode->blocks[i];
      }
    }
    else {
      if (i == (BLOCKS_PER_INODE-1)) {
	block = readBlockByFilesystem (inode->blocks[BLOCKS_PER_INODE-1], fs);
      }
      else if ((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block == 0) {
	free (block);
	block = readBlockByFilesystem (getIntAtBlock (inodes_per_indirection_block * sizeof (int32_t), block), fs);
      }
      if (getIntAtBlock (((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block) == value)
	return getIntAtBlock (((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block);
    }
  }
  return -1;
}

//
int32_t* getBlocksFromInode (Filesystem fs, Inode inode) {
  int32_t* array = malloc (sizeof (int32_t) * inode->number_of_blocks);
  int32_t inodes_per_indirection_block = (fs->superblock->block_size / sizeof (int32_t)) - 1;
  int32_t i;
  Datablock block;
  for (i = 0; i < inode->number_of_blocks; i++) {
    if (i < (BLOCKS_PER_INODE -1)) {
      array[i] = inode->blocks[i];
    }
    else {
      if (i == (BLOCKS_PER_INODE-1)) {
	block = readBlockByFilesystem (inode->blocks[BLOCKS_PER_INODE-1], fs);
      }
      else if ((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block == 0) {
	free (block);
	block = readBlockByFilesystem (getIntAtBlock (inodes_per_indirection_block * sizeof (int32_t), block), fs);
      }
      array[i] = getIntAtBlock (((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block);
    }
  }
  return array;
}

//
int32_t findDatablockByInode (int32_t inode_id, Filesystem fs) {
  int32_t inodes_per_block = fs->superblock->block_size / INODE_SIZE;

  return (inode_id / inodes_per_block) + fs->first_inodeblock;
}

int32_t findIdByInode (int32_t inode_number, int32_t fib, int32_t block_size) {
  int32_t inodes_per_block = block_size / INODE_SIZE;

  return (inode_number / inodes_per_block) + fib;
}

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
  inode->timestamp = (int32_t) getIntAtBlock (position+(3*soi32), block);
  getStringAtBlock (position+(4*soi32), block, INODE_TYPE_SIZE, inode->type);
  getStringAtBlock (position+(4*soi32)+INODE_TYPE_SIZE, block, INODE_NAME_SIZE, inode->name);
  getStringAtBlock (position+(4*soi32)+INODE_TYPE_SIZE+INODE_NAME_SIZE, block, 1, &inode->dir);
  inode->number_of_blocks = getIntAtBlock (position+(4*soi32)+INODE_TYPE_SIZE+INODE_NAME_SIZE+1, block);
  // criar funcao pra isso depois
  memcpy ((void *) inode->blocks, block->content+position+(soi32*5)+INODE_TYPE_SIZE+INODE_NAME_SIZE+1, BLOCKS_PER_INODE * soi32);
}

int32_t findInodeBlock(Inode inode, int32_t block_size) {

	if(inode == NULL)
		return -1; //Inode invalido
	
	if(block_size <= 0)
		error("Block Size Invalido!");
		
	return inode->number / (block_size/INODE_SIZE);
		
}

// findInodePosAtBlock
int32_t findInodePosAtBlock (Inode inode, int32_t block_size){
	
	if(inode == NULL)
		return -1; //Inode invalido
		
	if(block_size == 0) {
		error("Block size = 0!");
		return -10;
	}
		
	int32_t q = block_size/INODE_SIZE;
	
	int32_t r = inode->number % q;
	
	return r*INODE_SIZE;
}

int32_t findPosInodeByBlock (int32_t number, int32_t block_size){
		
	int32_t q = block_size/INODE_SIZE;
	
	int32_t r = number % q;
	
	return r*INODE_SIZE;
}

void update(FILE* file, int32_t block_size, int32_t fib, int32_t ni, int32_t nb, int32_t nd, char* ib, char* dbb){
	
	datablock dblock;
	int32_t atual = 0;
	int32_t i = 0;
	int32_t soi = sizeof(int32_t);
	int32_t total_blocks = FILE_SIZE / block_size;
	clearBlock(&dblock);
	dblock.id = 0;
	
	//savesuperblock
	readBlocktoBlock(&dblock, block_size, file);
	setIntAtBlock(soi*2, &dblock, ni);
	setIntAtBlock(soi*3, &dblock, nb);
	setIntAtBlock(soi*5, &dblock, nd);
	writeBlock(0, file, &dblock, block_size);
	

	//ib
	clearBlock(&dblock);
	  // INODE BITMAP
	dblock.id = ++atual;

	setStringAtBlock (0, &dblock, MIN(block_size, MAX_INODES), ib);
	writeBlock (atual, file, &dblock, block_size);
	if (block_size < MAX_INODES) {
		dblock.id = ++atual;
		clearBlock (&dblock);
		setStringAtBlock (0, &dblock, block_size, ib + block_size);
		writeBlock (atual, file, &dblock, block_size);
	  }

  //printf ("ibm\n");

  // DATABLOCK BITMAP
	for (i = 0; i < total_blocks/block_size; i++) {
		dblock.id = ++atual;
		clearBlock (&dblock);

		setStringAtBlock (0, &dblock, block_size, dbb + (i*block_size));
		writeBlock (atual, file, &dblock, block_size);
	  }
	  if (total_blocks % block_size != 0) {
		dblock.id = ++atual;
		clearBlock (&dblock);

		setStringAtBlock (0, &dblock, (total_blocks % block_size), dbb + ((total_blocks/block_size) * block_size));
		writeBlock (atual, file, &dblock, block_size);
	  }
}


int32_t insertBlockInInode(int32_t filho, Inode inode, Filesystem fs, FILE* file){
	if (inode->number_of_blocks >= MAX_BLOCKS_PER_INODE)
		error("Inode is full! Cannot insert Block/Inode to it!");
	
	datablock block,temp;
	int32_t i = inode->number_of_blocks;
	int32_t j = 0; //contador loop
	
	if(i<BLOCKS_PER_INODE-1) {
		inode->blocks[i] = filho;
		inode->number_of_blocks++;
		return 0;
	}
	else { //indirecao
		int32_t indirection_lines = (fs->superblock->block_size/(sizeof(int32_t))) - 1;
		int32_t indirection_blocks = 0;
		i = inode->number_of_blocks;
		i = i - (BLOCKS_PER_INODE-1);
		
		while(i>0){
			i = i - indirection_lines;
			indirection_blocks++;
		}
		i = i + indirection_lines - 1; //ultima linha a procurar no ultimo bloco
		if(i == 0)
				if(fs->superblock->number_of_blocks >= FILE_SIZE/fs->superblock->block_size){
					warning("Not enough datablock for indirection!");
					return -1;
				}
				
		block.id = inode->blocks[BLOCKS_PER_INODE-1]; //copiar bloco pro bloco
		readBlocktoBlock(&block, fs->superblock->block_size, file);
		for(j=0;j<indirection_blocks-1;j++) {
			temp = block;
			temp.id = getIntAtBlock (indirection_lines-1, &block);
			readBlocktoBlock (&temp, fs->superblock->block_size, file);
			block = temp;
		}
		if(i==0) {
			getFreeDatablock(fs, &temp);
			setIntAtBlock(fs->superblock->block_size-sizeof(int32_t), &block, temp.id); 
			writeBlock(block.id, file, &block, fs->superblock->block_size);
			block = temp;
		}
		setIntAtBlock (i*sizeof(int32_t), &block, filho);
		writeBlock(block.id, file, &block, fs->superblock->block_size);
		return 1;
	}
}

// retorna a permissao exclusiva de alguma
Bool valuePermition (int32_t permition, int32_t exclusivePermition) {
  permition = permition / exclusivePermition;
  if (permition % 2 == 0) return false;
  else return true;
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
  for (int32_t i = 0; i < size; i++) {
    if (bm->map[i] == 0) printf ("0 ");
    else printf ("1 ");
  }
  printf ("\n");
}

// printInode
void printInode (Inode inode) {
  printf ("Inode %d:\n", inode->number);
  printf ("father: %d\n", inode->father);
  printf ("permitions: %d\n", inode->permition);
  struct tm* timeinfo;
  timeinfo = localtime ((time_t*) &(inode->timestamp));
  printf ("timestamp: %s\n", asctime (timeinfo));
  printf ("Name: %s.%s\n", inode->name, inode->type);
  if (inode->dir == 0) printf ("dir: nao\n");
  else printf ("dir: sim\n");
  //printf ("dir: %c\n", inode->dir);
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
  printf ("Current dir: %d\n", fs->current_dir);
  printf ("First inode block: %d\n", fs->first_inodeblock);
  printf ("First datablock: %d\n", fs->first_datablock);
}


char* printPermitions (Inode inode) {
  char* permition = "RWE";
  int32_t value = inode->permition;
  if (valuePermition (value, 100) == false) permition[0] = '-';
  if (valuePermition (value, 10) == false) permition[1] = '-';
  if (valuePermition (value, 1) == false) permition[2] = '-';
  return permition;
}

int32_t printSizeInode (Filesystem fs, Inode inode) {
  if (inode->dir != 1) {
    return inode->number_of_blocks * fs->superblock->block_size;
  }
  return 0; // nao sei o caso de diretorio
}

// TODO: Para todo codigo de create precisamos criar um codigo de free;
// TODO: FilesystemToFile
// TODO: FileToFilesystem

