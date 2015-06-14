/* 
   Task 2 - rm.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando rm no nosso filesystem.
*/

#include "rm.h"

void rm (Filesystem fs, Inode inode) {
  freeInode (fs, inode);
}
