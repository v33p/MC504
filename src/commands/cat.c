/* 
   Task 2 - cat.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando cat no nosso filesystem.
*/

#include "cat.h"

// precisamos fazer uma funcao que dado um diretorio e um nome encontra
// o inode. caso nao esteja no diretorio, retorna alguma coisa.

void cat (Filesystem fs, FILE* file, Inode inode) {
  int32_t i, j;
  if (inode->permition < 100) {
    warning ("Permition Denied.");
    return;
  }
  if (inode->dir == 1) {
    warning ("You cannot use this command in a diretory");
    return;
  }
  Datablock block = malloc (sizeof (datablock));
  if (inode->number_of_blocks > BLOCKS_PER_INODE) {
    for (i = 0; i < BLOCKS_PER_INODE -1; i++) {
      clearBlock (block);
      block = readBlock (inode->blocks[i], file, fs->superblock->block_size);
      for (j = 0; j < fs->superblock->block_size; j++) {
	if (block->content[j] == 'EOF') break;
	printf ("%c", block->content[j]);
      }
    }
  }
  else {
    for (i = 0; i < BLOCKS_PER_INODE; i++) {
      if (inode->blocks[i] == -1) break;
      clearBlock (block);
      block = readBlock (inode->blocks[i], file, fs->superblock->block_size);
      for (j = 0; j < fs->superblock->block_size; j++) {
	if (block->content[j] == 'EOF') break;
	printf ("%c", block->content[j]);
      }
    }
    printf ("\n");
  }
}
