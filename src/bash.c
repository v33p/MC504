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

  if (strcmp (argv[1], "-b") == 0) {
    //-b <arquivo_fs>: inicia o bash mode
	bash(argv[2]);
  }
  else if (strcmp (argv[1], "-i") == 0) {
    //-i <pathname_do_arq_origem_fs_sistema> <pathname_do_arq_destino_dentro_do_ufs> <arquivo_fs>: transfere/inclui um *novo* aquivo dentro do UFS. Sua permissão será total: leitura, escrita e execução. Se pathname não existir no UFS, ou o arquivo já existir, um erro deve ser dado.
    ufsInput(argv[2], argv[3], argv[4]);
  }
  else if (strcmp (argv[1], "-o") == 0) {
    //-o <pathname_do_arq_origem_ufs> <pathname_do_arq_destino_fs_sistema> <arquivo_fs>: transfere/copia um aquivo de dentro do UFS para o fs exterior. Não há necessidade de conversão de permissões. Se pathname não existir no UFS, um erro deve ser dado.
    ufsOutput(argv[2], argv[3], argv[4]);
  }
  else if (strcmp (argv[1], "-d") == 0) {
    //-d <arquivo_fs>: imprime ("Used Inodes: %i \n Used Directories: %i \n Used Data Blocks: %i\n)
	//Filesystem fs = fileToFilesystem (argv[2]);
	//printf("Used Inodes: %i \n Used Directories: %i \n Used Data Blocks: %i\n", fs->superblock->number_of_inodes, fs->superblock->number_of_dir, fs->superblock->number_of_blocks);
  }
  else {
    error ("Wrong parameters.");
  }
}

void bash (char *file_name){

    /*
    ls: lista todas as entradas do diretório corrente. Parâmetro extra "-l": lista o tamanho dos arquivos(num_blocks*tam_blocks), as permissões e o time stamp.
    chmod <permissões> <nome_arquivo>: troca as permissões de nome_arquivo. As permissões respeitam a ordem RWE, onde chmod 111 representa permissão total, e chmod 100 apenas leitura.
    mkdir <nome_dir>: cria um diretório chamado nome_dir.
    chdir <nome_dir>: troca o diretório corrente para o nome_dir.
    rm <alvo>: remove alvo do sistema de arquivos, seja arquivo ou diretório. Caso diretório, ele deve visitar cada filho e remove-lo.
    echo <dado> <alvo>: cria um arquivo alvo e insere dado. Caso alvo exista, seus blocos devem ser removidos e dado adicionado na nova lista de blocos. Alvo deve ser um arquivo no diretório corrente. Caso dado contenha espaços, '"' deve ser usado, p.ex.: echo "uma frase de exemplo" arquivo1.
    cat <alvo>: o conteúdo arquivo alvo é lido e impresso na tela.
    exit: termina o bash mode e sai da ferramenta.
	*/

	char command_line[COMMAND_SIZE];
	char command[COMMAND_SIZE];
	char last;
	int i = 0;

	printf("Entrei no Bash!\n");

	while(1) {
		//fgets(command_line, sizeof(command_line), stdin);
		scanf("%c", &command[0]);
		for(i=1;i<COMMAND_SIZE-1;i++){
			if(isspace(command[i-1]))
				break;
			scanf("%c", &command[i]);
		}
		last = command[i-1];
		command[i-1] = 0;
		printf("%s", command);
		if(strcmp(command, "ls") == 0) {
			//ls
			//printf("Comando ls\n");
		}
		else if(strcmp(command, "chmod") == 0) {
			//chmod
			//printf("Comando chmod\n");
		}
		else if(strcmp(command, "mkdir") == 0){
			//mkdir
			//printf("Comando mkdir\n");
		}
		else if(strcmp(command, "chdir") == 0){
			//chdir
			//printf("Comando chdir\n");
		}
		else if(strcmp(command, "rm") == 0){
			//rm
			//printf("Comando rm\n");
		}
		else if(strcmp(command, "echo") == 0){
			//echo
			//printf("Comando echo\n");
		}
		else if(strcmp(command, "cat") == 0){
			//cat
			//printf("Comando cat\n");
		}
		else if(strcmp(command, "exit") == 0){
			//exit
			//printf("Comando exit\n");
			return;
		}
		else {
			warning ("Comando invalido!");
		}

	}
}

void ufsInput(char *arq_sistema, char *arq_ufs, char *fs_name) {
	FILE* arq = fopen(arq_sistema, "rb");
	Filesystem fs = fileToFilesystem (fs_name);
	FILE* ufs = fopen(fs_name, "r+b");
	int32_t file_size = 0;
	int32_t file_blocks = 0;
	int32_t bsize, indirecao;
	int32_t i = 0, j = 0;
	datablock dblock;
	Inode inode;

	if (arq == NULL) error ("Native FS -> Null file.");
	if (fs == NULL) error ("Filesystem -> Null file.");

	bsize = fs->superblock->block_size;
	//TODO: Tratar nome arquivo

	while(fgetc(arq) != EOF){
        file_size++;
	}
	//fseek(arq, 0, SEEK_END);
	//file_size = ftell(arq);
	//fseek(arq, 0, SEEK_SET);

	printf("File Size: %d\n", file_size);

	if(fs->superblock->number_of_inodes >= 1024)
		error ("Filesystem is full! (Unavailable Inodes)");

	if(file_size == 0)
		file_blocks = 0;
	else
		file_blocks = (file_size + bsize-1) / bsize;

	if(fs->superblock->number_of_blocks + file_blocks > FILE_SIZE/bsize)
		error ("Filesystem is full! (Not enough Datablocks)");

	inode = getFreeInode(fs);
    for(indirecao=1;i<BLOCKS_PER_INODE;indirecao++)
            if(indirecao*bsize/4 >= MAX_BLOCKS_PER_INODE) break;

	for (j=0;j<file_blocks;j++){
        clearBlock(&dblock);
		for(i=0;i<bsize;i++) {
			dblock.content[i] = fgetc(arq);
			if(dblock.content[i] == 'EOF')
				break;
		}
		getFreeDatablock(fs, &dblock);
		inode->number_of_blocks++;
        if(j < BLOCKS_PER_INODE-1 -indirecao){
            inode->blocks[j] = dblock.id;
            writeBlock (dblock.id, ufs, &dblock, bsize);
        }
        //Fazer tratamento de indirecao
    }




	fclose(arq);
	fclose(ufs);
}
void ufsOutput(char *arq_ufs, char *arq_sistema, char *fs_name) {

}

int32_t getInodefromPathname (char *pathname, Filesystem fs){

    char* tok = strtok(pathname, "/");
    Inode dir = fs->inodes[0];
    char* child;
    char valid = 1;
    int32_t x;

    while (tok != NULL) {
        child = tok;
        x = isInDir(child, dir, fs);
        if(x >= 0 && valid) {
            dir = fs->inodes[x];
        }
        else if (valid) {
            dir = NULL;
            valid = 0;
        }
        else {
            warning("Invalid pathname!");
            return -2;
        }
        tok = strtok(NULL,"/");
    }

    if(valid)
        return dir;

    return -1;
}
