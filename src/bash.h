/* 
   Task 2 - bash.h
   01/06/2015
   Lucas Padilha - 119785 | Pedro Tadahiro - 103797
   Implementacao do bash no nosso filesystem.
*/

#include "filesystem.c"
#include "commands/cat.c"
#include "commands/chdir.c"
#include "commands/chmod.c"
#include "commands/echo.c"
#include "commands/exit.c"
#include "commands/ls.c"
#include "commands/mkdir.c"
#include "commands/rm.c"

/* DEFINITIONS */
#define BUFFER_SIZE 256
#define COMMAND_SIZE 256

/* ENUM */


/* STRUCTS */


/* FUNCTIONS */

/*
  Main: Funcao que chama os metodos iniciais do programa.
 */
int main (int argc, char** argv);

/*
  Parse Input: Trata a entrada recebida pelo programa, chamando as funcoes
necessarias para tratar determinado problema.
  param:
    int argc = tamanho do vetor de strings
    char** argv = vetor de strings de entrada
 */
void parseInput (int argc, char** argv);

/*
 * 	Bash: Modo Bash, fica em um loop recebendo comandos e chamando as
 * respectivas funcoes para tratar delas.
 * Comandos:
 * ls [-ls] - lista entradas do diretorio corrente. -l mostra tamanho gasto por blocos
 * chmod <permissoes> <alvo> - modifica permissoes de <alvo> para <permissoes>
 * mkdir <nome_dir> - cria diretorio chamado <nome_dir>
 * chdir <nome_dir> - muda diretorio corrente para <nome_dir>
 * rm <alvo> - remove <alvo>
 * echo <dado> <alvo> - salva <dado> em <alvo> (overwrite)
 * cat <alvo> - print do conteudo de <alvo>
 * exit - termina o bash mode
 * 	param:
 * 		char* file_name = nome do arquivo do filesystem
*/

void bash (char *file_name);
