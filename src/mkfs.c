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
  if (argc < 2) error ("Missing parameters.");
  if (argc > 3) error ("Wrong number of parameters.");

  if (strcmp (argv[2], "-b")) {
    // 
  }
  else {
    error ("Wrong parameters.");
  }
}
