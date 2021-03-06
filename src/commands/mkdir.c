/* 
   Task 2 - mkdir.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando mkdir no nosso filesystem.
*/

#include "mkdir.h"

Inode mkdir (Filesystem fs, Inode dir, char* name_inode) {
  if (dir->number_of_blocks == MAX_INODES) error ("Diretory full.");
  Inode inode = createInode (fs, getFreeInode (fs), dir->number, 110, "", name_inode, 1);
  if (inode != NULL) {
    if (dir->number_of_blocks < BLOCKS_PER_INODE-1) {
      dir->blocks[dir->number_of_blocks] = inode->number;
    }
    else {
      // calcular qual e o bloco que deve armazenar o novo bloco
      int32_t inodes_per_indirection_block = (fs->superblock->block_size / sizeof (int32_t)) - 1;
      int32_t i;
      Datablock block;
      for (i = 0; i < inode->number_of_blocks; i++) {
	if (i > (BLOCKS_PER_INODE -1)) {
	  if (i == (BLOCKS_PER_INODE-1)) {
	    block = readBlockByFilesystem (inode->blocks[BLOCKS_PER_INODE-1], fs);
	  }
	  else if ((i - BLOCKS_PER_INODE-1) % inodes_per_indirection_block == 0) {
	    free (block);
	    block = readBlockByFilesystem (getIntAtBlock (inodes_per_indirection_block * sizeof (int32_t), block), fs);
	  }
	}
      }
      // caso tenha acabado espaco no bloco de indirecao
      if ((i+1 - BLOCKS_PER_INODE-1) % inodes_per_indirection_block == 0) {
	Datablock newBlock = malloc (sizeof (datablock));
	getFreeDatablock (fs, newBlock);
	setIntAtBlock (((i+1 - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block, newBlock->id);
	setIntAtBlock (0, newBlock, inode->number);
      }
      // grava no bloco atual de indirecao
      else {
	setIntAtBlock (((i+1 - BLOCKS_PER_INODE-1) % inodes_per_indirection_block) * sizeof (int32_t), block, inode->number);
      }
    }
    dir->number_of_blocks++;
    fs->superblock->number_of_dir++;
  }

  // salvando no arquivo o inode
  //printf("%d\n", findDatablockByInode(inode->number, fs));
  //Datablock dblock = readBlockByFilesystem (findDatablockByInode(inode->number, fs), fs);
  //readBlocktoBlock (&dblock, fs->superblock->block_size, file);
  
  //setInodeAtBlock (findInodePosAtBlock (inode, fs->superblock->block_size), dblock, inode);
  //writeBlockByFilesystem (findDatablockByInode(inode->number, fs), fs, dblock);
  //free (dblock);
  
  //fclose (file);
  printf ("Criou inode\n");
  return inode;
}

void mkdir2 (char* ib,int32_t fib, int32_t* ni, int32_t* nd, int32_t block_size, FILE* file, Inode dir, char* name_inode) {
  if (dir->number_of_blocks == MAX_INODES) error ("Diretory full.");
  inode child;
  int32_t i = 0;
  
  
	int32_t n = getFreeInodeNumber(ib);
	if ( n == -1)
		error("Sem Inodes disponíveis!");
		
	createInode2(&child,n, dir->number, 110, "", name_inode, 1, ni, nd);
	
	printf("New Inode Number: %d\n", n);
	printf("Nome do inode: %s\n", name_inode);
	/*printf("Nome do inode: %s\n", child.name);*/

  
    dir->blocks[dir->number_of_blocks] = child.number;
    dir->number_of_blocks++;
  
 
  

  // salvando no arquivo o inode
  datablock dblock;
  /*dblock.id = 100;
  readBlocktoBlock(&dblock, block_size, file);
  getInodeAtBlock(0, &dblock, &child);
  printInode(&child);*/
  dblock.id = findIdByInode(n, fib, block_size);
  clearBlock(&dblock);
  printf("Block ID (filho): %d\n", dblock.id);
  readBlocktoBlock(&dblock, block_size, file);
  printf("Pos Inode by Block (filho): %d\n", findPosInodeByBlock(n,block_size));
  setInodeAtBlock(findPosInodeByBlock(n, block_size), &dblock, &child);
  writeBlock(dblock.id, file, &dblock, block_size);
  
  //save father
  dblock.id = findIdByInode(dir->number, fib, block_size);
  clearBlock(&dblock);
  printf("Block ID (pai): %d\n", dblock.id);
  readBlocktoBlock(&dblock, block_size, file);
  printf("Pos Inode by Block (pai): %d\n", findPosInodeByBlock(n,block_size));
  child.number = dir->number;
  child.father = dir->father;
  child.permition = dir->permition;
  strcpy(child.name, dir->name);
  strcpy(child.type, dir->type);
  child.timestamp = dir->timestamp;
  child.dir = dir->dir;
  child.number_of_blocks = dir->number_of_blocks;
  for(i=0;i<BLOCKS_PER_INODE;i++)
	child.blocks[i] = dir->blocks[i];
  setInodeAtBlock(findPosInodeByBlock(child.number, block_size), &dblock, &child);
  writeBlock(dblock.id, file, &dblock, block_size);
  /*readBlocktoBlock(&dblock, block_size, file);
  getInodeAtBlock(0, &dblock, &child);
  printInode(&child);*/

  //printf ("Criou inode no %d\n", dblock.id);
}
