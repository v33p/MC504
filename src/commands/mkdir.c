/* 
   Task 2 - mkdir.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando mkdir no nosso filesystem.
*/

#include "mkdir.h"

Inode mkdir (Filesystem fs, Inode dir, char* name_inode) {
  if (dir->number_of_blocks == MAX_INODES) error ("Diretory full.");
  Inode inode = getFreeInode (fs);
  if (inode != NULL) {
    inode->father = dir;
    inode->dir = 1;
    strcpy (name_inode, fs->file_name);
    inode->permition = 110;
    
    if (dir->number_of_blocks < BLOCKS_PER_INODE-1) {
      dir->blocks[dir->number_of_blocks-1] = inode->number;
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
  }
  return inode;
}
