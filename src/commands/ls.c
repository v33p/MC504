/* 
   Task 2 - ls.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do comando ls no nosso filesystem.
*/

#include "ls.h"

void ls (Filesystem fs, Inode dir, char* param) {
  int32_t i;
  int32_t* array = getBlocksFromInode (fs, dir);
  if (strcmp (param, "-l")) {
    for (i = 0; i < dir->number_of_blocks; i++) {
      char* permition = printPermitions (fs->inodes[array[i]]);
      struct tm* timeinfo;
      timeinfo = localtime (&((time_t) fs->inodes[array[i]]->timestamp));
      if (fs->inodes[array[i]]->dir == 1)
	printf ("%s %d %s %s\n", permition, printSizeInode (fs->inodes[array[i]]), asctime (timeinfo), fs->inodes[array[i]]->name);
      else
	printf ("%s %d %s %s.%s\n", permition, printSizeInode (fs->inodes[array[i]]), asctime (timeinfo), fs->inodes[array[i]]->name, fs->inodes[array[i]]->type);
    }
  }
  else {
    for (i = 0; i < dir->number_of_blocks; i++) {
      if (fs->inodes[array[i]]->dir == 1)
	printf ("%s\n", fs->inodes[array[i]]->name);
      else
	printf ("%s.%s\n", fs->inodes[array[i]]->name, fs->inodes[array[i]]->type);
    }
  }
  free (array);
}

// retorna a permissao exclusiva de alguma 
Bool valuePermition (int32_t permition, int32_t exclusivePermition) {
  permition = permition / exclusivePermition;
  if (permition % 2 == 0) return false;
  else return true;
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
