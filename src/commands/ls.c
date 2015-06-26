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
      timeinfo = localtime ((time_t*) &(fs->inodes[array[i]]->timestamp));
      if (fs->inodes[array[i]]->dir == 1)
	printf ("%s %d %s %s\n", permition, printSizeInode (fs, fs->inodes[array[i]]), asctime (timeinfo), fs->inodes[array[i]]->name);
      else
	printf ("%s %d %s %s.%s\n", permition, printSizeInode (fs, fs->inodes[array[i]]), asctime (timeinfo), fs->inodes[array[i]]->name, fs->inodes[array[i]]->type);
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
  //free (array);
}

void ls2 (int32_t block_size, int32_t fib, FILE* file, Inode current_dir, char* param) {

	int32_t i = 0;
	int32_t number;
	datablock dblock;
	inode atual;

	if (param == NULL){
		for(i=0;i<current_dir->number_of_blocks;i++){
			clearBlock(&dblock);
			number = current_dir->blocks[i];
			dblock.id = findIdByInode(number, fib, block_size);
			readBlocktoBlock(&dblock, block_size, file);
			getInodeAtBlock(findPosInodeByBlock(number, block_size),&dblock, &atual);
			printf("%s\n", atual.name);
		}
		return;
	}
	
	if(strcmp(param, "-l") == 0) {
		for(i=0;i<current_dir->number_of_blocks;i++){
			clearBlock(&dblock);
			number = current_dir->blocks[i];
			dblock.id = findIdByInode(number, fib, block_size);
			readBlocktoBlock(&dblock, block_size, file);
			getInodeAtBlock(findPosInodeByBlock(number, block_size),&dblock, &atual);
			printf("%s     %d\n", atual.name, atual.number_of_blocks*block_size);
		}
	}
}

