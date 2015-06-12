/* 
   Task 2 - mkfs.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do mkfs do nosso filesystem.
*/

#include "mkfs.h"

// Main
int main (int argc, char** argv) {
  parseInput (argc, argv);
  return EXIT_SUCCESS;
}

// parseInput
void parseInput (int argc, char** argv) {
  int block_size = 512;
  if (argc < 2) error ("Missing parameters.");
  if (argc > 4) error ("Wrong number of parameters.");
  
  if (strcmp ("-b", argv[1]) == 0) {
    if (argc < 4) error ("Missing parameters."); 
    block_size = parseSize (argv[2]);
    Filesystem fs = createFileSystem (block_size);
    filesystemToFile (fs, argv[3]);
    fileToFilesystem (argv[3]);
  }
  else if (argc == 2) {
    // Criar arquivo com valor default.
    Filesystem fs = createFileSystem (block_size);
    filesystemToFile (fs, argv[1]);
  }
  else {
    error ("Wrong parameters.");
  }
}

// parseSize
int parseSize (char* string) {
  int size = strlen (string);
  int multiplier  = 1;
  if (string[size -2] == 'K') {
    multiplier = 1024;
  }
  return atoi (string) * multiplier;
}

