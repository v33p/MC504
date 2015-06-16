/* 
   Task 2 - echo.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando echo no nosso filesystem.
*/

#include "echo.h"

void echo (Filesystem fs, Inode dir, char* dado, char* alvo) {
  Inode inode = searchInodeOnDirByName (alvo);
  if (inode != NULL) {
    freeInode (inode);
  }
  char* newAlvo = strtok (alvo, ".");
  char* name = newAlvo;
  char* newAlvo = strtok (NULL, ".");
  char* type = newAlvo;
  inode = createInode (fs, getFreeInode(fs), dir->number, 110, type, name, 0);
  int32_t len = strlen (dado);
  if (len < fs->superblock->block_size) {
    Datablock block = malloc (sizeof (datablock));
    strcpy (block->content, dado);
    getFreeDatablock (fs, block);
    inode->blocks[0] = block->id;
    inode->number_of_blocks++;
    writeBlockByFilesystem (block->id, fs, block);
    free (block);
  }
  else {
    // tratar
    int32_t number_of_blocks = len / fs->superblock->block_size;
  }
}
