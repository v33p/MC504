/* 
   Task 2 - bash.c
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do bash no nosso filesystem.
*/

#include "bash.h"

// Main
int main (int argc, char** argv) {
  parseInput (argc, argv);
  return EXIT_SUCCESS;
}

// ParseInput
void parseInput (int argc, char** argv) {
  if (argc < 3) error ("Missing parameters.");
  if (argc > 5) error ("Wrong number of parameters.");

  if (strcmp (argv[2], "-b")) {
    //
  }
  else if (strcmp (argv[2], "-i")) {
    //
  }
  else if (strcmp (argv[2], "-o")) {
    //
  }
  else if (strcmp (argv[2], "-d")) {
    //
  }
  else {
    error ("Wrong parameters.");
  }
}
