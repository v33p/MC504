/*
   Task 2 - cat.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando cat no nosso filesystem.
*/

#include "cat.h"

// precisamos fazer uma funcao que dado um diretorio e um nome encontra
// o inode. caso nao esteja no diretorio, retorna alguma coisa.
// Fiz o isInDir no filesystem.c, mas ele so olha o NAME...
// tem que fazer olhar pro type tambem

void cat (Filesystem fs, Inode inode) {
  int32_t i, j;
  if (inode->permition < 100) {
    warning ("Permition Denied.");
    return;
  }
  if (inode->dir == 1) {
    warning ("You cannot use this command in a diretory");
    return;
  }
  Datablock block;
  int32_t* array = getBlocksFromInode (fs, inode);
  for (i = 0; i < inode->number_of_blocks; i++) {
    block = readBlockByFilesystem (array[i], fs);
    for (j = 0; j < fs->superblock->block_size; j++) {
      if (block->content[j] == EOF) break;
      printf ("%c", block->content[j]);
    }
    printf ("\n");
    free (block);
  }
  free (array);
}
