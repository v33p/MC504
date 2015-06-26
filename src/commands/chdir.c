/* 
   Task 2 - chdir.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando chdir no nosso filesystem.
*/

#include "chdir.h"

void chdir (Filesystem fs, Inode dir) {
  if (dir != NULL) fs->current_dir = dir->number;
}

void chdir2 (Inode current_dir, char* name, char* ib, int32_t block_size, FILE* file, int32_t fib) {
  int32_t i;
  datablock dblock;
  inode new_inode;
  
  if (name == NULL) {
	  dblock.id = findIdByInode(0, fib, block_size);
	  readBlocktoBlock(&dblock, block_size, file);
	  getInodeAtBlock(0, &dblock, &new_inode);
	current_dir->number = new_inode.number;
	current_dir->father = new_inode.father;
	current_dir->permition = new_inode.permition;
	current_dir->timestamp = new_inode.timestamp;
	current_dir->dir = new_inode.dir;
	current_dir->number_of_blocks = new_inode.number_of_blocks;
	for (int32_t j = 0; j < BLOCKS_PER_INODE; j++)
	  current_dir->blocks[j] = new_inode.blocks[j];
	strcpy (current_dir->name, new_inode.name);
	return;
	}
	  
  for (i = 0; i < MAX_INODES; i++) {
    if (ib[i] == 1) {
      dblock.id = findIdByInode (i, fib, block_size);
      readBlocktoBlock (&dblock, block_size, file);
      getInodeAtBlock (findPosInodeByBlock (i, block_size), &dblock, &new_inode);
      if (strcmp (new_inode.name, name) == 0) {
	current_dir->number = new_inode.number;
	current_dir->father = new_inode.father;
	current_dir->permition = new_inode.permition;
	current_dir->timestamp = new_inode.timestamp;
	current_dir->dir = new_inode.dir;
	current_dir->number_of_blocks = new_inode.number_of_blocks;
	for (int32_t j = 0; j < BLOCKS_PER_INODE; j++)
	  current_dir->blocks[j] = new_inode.blocks[j];
	strcpy (current_dir->name, new_inode.name);
	// type nao precisa copiar pq e sempre vazio. 
      }
    }
  }
}
