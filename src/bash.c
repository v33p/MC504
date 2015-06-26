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
	Filesystem fs = fileToFilesystem (argv[2]);
	printf("Used Inodes: %i \n Used Directories: %i \n Used Data Blocks: %i\n", fs->superblock->number_of_inodes, fs->superblock->number_of_dir, fs->superblock->number_of_blocks);
  }
  else if (strcmp (argv[1], "-p") == 0) {
    Filesystem fs = fileToFilesystem (argv[2]);
    printFilesystem (fs);
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

  //char command_line[COMMAND_SIZE];
  //char command[COMMAND_SIZE];
  //char last;
  //int i = 0;
  
  printf("Entrei no Bash! %s\n", file_name);
  
  Filesystem fs = fileToFilesystem (file_name);
  int32_t ni = fs->superblock->number_of_inodes;
  int32_t nd = fs->superblock->number_of_dir;
  int32_t block_size = fs->superblock->block_size;
	FILE* file = fopen(file_name, "r+");
	int32_t fib = fs->first_inodeblock;
	int32_t fdb = fs->first_datablock;
	char ib[1024];
	int32_t i = 0;
	ib[0] = 1;
	for(i=1;i<1024;i++)
		ib[i] = fs->inode_bitmap->map[i];
	inode current_dir;
	current_dir.number = 0;
	current_dir.permition = 111;
	current_dir.father = -1;
	current_dir.number_of_blocks = 0;
	current_dir.dir = 1;
	strcpy(current_dir.name,"");
	strcpy(current_dir.type,"");
	free(fs);
  char input[256];
  char* tok;
  while(1) {
    // Tratando entrada	  
    char c = getchar();
    int i = 0;
    while (c != '\n') {	    
      input[i] = c;
      c = getchar ();
      i++;
    }
    input[i] = '\0';
    
    //printf ("%s\n", input); 
    tok = strtok (input, " ");
    if(strcmp(tok, "ls") == 0) {
      //ls
      //printf("Comando ls\n");
      tok = strtok (NULL, " ");
      //if (tok == NULL)
	//ls (fs, fs->inodes[fs->current_dir], "");
      //else{}
	//ls (fs, fs->inodes[fs->current_dir], tok);
    }
    else if(strcmp(tok, "chmod") == 0) {
      //chmod
      //printf("Comando chmod\n");  
    }
    else if(strcmp(tok, "mymkdir") == 0){
      //mkdir
      //printf("Comando mkdir\n");
      tok = strtok (NULL, " ");
      if (tok == NULL)
	printf ("Wrong parameters for mkdir");
      else {
	//mkdir (fs, fs->inodes[fs->current_dir], tok);
	mkdir2 (ib, fib, &ni, &nd, block_size, file, &current_dir, tok);
	//filesystemToFile (fs, file_name);
	//printFilesystem (fs);
      }
    }/*
    else if(strcmp(tok, "chdir") == 0){
      //chdir
      //printf("Comando chdir\n");
      tok = strtok (NULL, " ");
      if (tok == NULL)
	printf ("Wronf parameters for chdir");
      else {
	for (int i = 0; i < MAX_INODES; i++) {
	 // if (fs->inodes[i] != NULL) {
	 //   if (fs->inodes[i]->dir == 1) {
	 //     if (strcmp (fs->inodes[i]->name, tok) == 0)
	//	chdir (fs, fs->inodes[i]);
	    }
	  }
	}
      }
    }
    else if(strcmp(tok, "rm") == 0){
      //rm
      //printf("Comando rm\n");
      tok = strtok (NULL, " ");
      if (tok == NULL)
	printf ("Wrong parameter for rm");
      else {
	for (int i = 0; i < MAX_INODES; i++) {
	//  if (fs->inodes[i] != NULL) {
	    char* tok2 = strtok (tok, ".");
	    char* name = tok2;
	    tok2 = strtok (NULL, ".");
	    if (tok2 == NULL) {
	      if (strcmp (fs->inodes[i]->name, name) == 0){
		rm (fs, fs->inodes[i]);
	      }
	    }
	    else {
	      if (strcmp (fs->inodes[i]->name, name) == 0){
		rm (fs, fs->inodes[i]);
	      }
	    }
	  }
	}
      }
    }
    else if(strcmp(tok, "echo") == 0){
      //echo
      //printf("Comando echo\n");
    }
    else if(strcmp(tok, "cat") == 0){
      //cat
      //printf("Comando cat\n");
      tok = strtok (NULL, " ");
      if (tok == NULL)
	printf ("Wrong parameter for cat");
      else {
	char* tok2 = strtok (NULL, ".");
	Inode inode = searchInodeOnDirByName (fs, fs->inodes[fs->current_dir], tok2);
	if (inode != NULL)
	  cat (fs, inode);
      }
    } */
    else if(strcmp(tok, "exit") == 0){
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
	FILE* arq = fopen(arq_sistema, "r");
	Filesystem fs = fileToFilesystem (fs_name);
	printSuperblock(fs->superblock);
	FILE* ufs = fopen(fs_name, "r+");
	int32_t file_size = 0; //tamanho do file
	int32_t file_blocks = 0; //# blocos usado pelo file
	int32_t bsize; //block size
	int32_t i = 0, j = 0; //contadores
	int32_t c = 0;
	datablock dblock;
	char* full_name;
	Inode inode;

	if (arq == NULL) error ("Native FS -> Null file.");
	if (fs == NULL) error ("Filesystem -> Null file.");

    c = getFatherfromPathname(arq_ufs, fs, fs->inodes[0]);
    if(c < 0) error("Invalid pathname!");
    if(fs->inodes[c]->number_of_blocks >= MAX_BLOCKS_PER_INODE)
        error("Diretory is full!");

	bsize = fs->superblock->block_size;
	
	printAllInodes(fs);

	printf("\n\nAntes de calcular tamanho arquivo\n\n");
    do{
        fgetc(arq);
        if( feof(arq) )
            break;
        else {
            file_size++;
            printf("\nLoop!\n");
        }
    }while(true);

	fseek(arq, -file_size, SEEK_CUR);

	printf("File Size: %d\n", file_size);

	if(fs->superblock->number_of_inodes >= 1024)
		error ("Filesystem is full! (Unavailable Inodes)");

	if(file_size == 0)
		file_blocks = 0;
	else
		file_blocks = (file_size + bsize-1) / bsize;

	if(fs->superblock->number_of_blocks + file_blocks > FILE_SIZE/bsize)
		error ("Filesystem is full! (Not enough Datablocks)");

	fs->inodes[c]->number_of_blocks++;
	//inode = getFreeInode(fs);
	inode = createEmptyInode (fs, getFreeInode (fs));
	printf("\nPeguei Inode Livre\n");
	
	full_name = getFullNamefromPathname(arq_sistema);
	j = checkNameValidity(full_name);
	if(j<0) 
		error("Invalid filename!");
	if(j==1){}
	else
		strncpy(inode->type, getType(full_name), INODE_TYPE_SIZE);
	strncpy(inode->name, getName(full_name), INODE_NAME_SIZE);

    insertBlockInInode(inode->number, fs->inodes[c], fs, ufs);
    printf("\nInseri bloco no inode\n\n");
	dblock.id = findDatablockByInode(c, fs);
	readBlocktoBlock(&dblock, bsize, ufs);
	setInodeAtBlock(c, &dblock, fs->inodes[c]);
	writeBlock(dblock.id, ufs, &dblock, bsize);
	printf("\nSalvei o novo inode pai!\n");
	printAllInodes(fs);
	return;

	for (j=0;j<file_blocks;j++){
        clearBlock(&dblock);
        dblock.id=j;
        if (j != file_blocks-1)
            readBlocktoBlock(&dblock, bsize, arq);
		else{
            for(i=0;i<bsize;i++) {
                c = fgetc(arq);
                if(feof(arq) == EOF)
                    break;
                dblock.content[i] = (char) c;
            }
		}
		getFreeDatablock(fs, &dblock);
		inode->number_of_blocks++;
        if(j < BLOCKS_PER_INODE-1){
            inode->blocks[j] = dblock.id;
            writeBlock (dblock.id, ufs, &dblock, bsize);
        }
        //Fazer tratamento de indirecao
    }
    //salvar o inode de alguma forma
    c = findInodePosAtBlock(inode, bsize);
    if(c < 0) 
		error("Couldn't find InodePosAtBlock");
	
	dblock.id = findInodeBlock(inode, bsize);
	readBlocktoBlock(&dblock, bsize, ufs);
	setInodeAtBlock(c, &dblock, inode);
	writeBlock(dblock.id, ufs, &dblock, bsize);


    free(inode);
    free(fs);
	fclose(arq);
	fclose(ufs);
}
void ufsOutput(char *arq_ufs, char *arq_sistema, char *fs_name) {
    FILE* arq = fopen(arq_sistema, "w");
    Filesystem fs = fileToFilesystem(fs_name);
    FILE* ufs = fopen(fs_name, "r");
    int32_t x = 0;
    Inode inode;
    Datablock block;
    int32_t i = 0;
    int32_t* array;
    int32_t bsize = fs->superblock->block_size;

    if (arq == NULL) error("Native FS -> Null file");
    if (ufs == NULL) error("User FS -> Null file");

    x = getInodeNumberfromPathname(arq_ufs, fs, fs->inodes[0]);
    if (x<0)
        error("Pathname Invalida!");
    inode = fs->inodes[x];
    if(inode->dir)
        error("Pathname de diretorio!");
    if(inode->permition < 100)
        error("Sem permissao de leitura!");

	if(inode->number_of_blocks > BLOCKS_PER_INODE-1){
		int32_t indirection_lines = (bsize/(sizeof(int32_t))) - 1;
		int32_t indirection_blocks = 0;
		int32_t index = inode->number_of_blocks;
		index = index - (BLOCKS_PER_INODE-1);
		
		while(index>0){
			index = index - indirection_lines;
			indirection_blocks++;
		}
		index = index + indirection_lines - 1; //ultima linha a procurar no ultimo bloco
	}

	array = getBlocksFromInode(fs, inode);

    for(i=0;i<inode->number_of_blocks;i++){
		block = readBlock (array[i], ufs, bsize);
		memcpy(arq+i*bsize, block, bsize);
		free(block);
	}
        /*if(i<BLOCKS_PER_INODE-1) {
			memcpy(arq+i*bsize,inode->blocks[i], bsize);
		}
		else { //tratar indirecao

			block.id = inode->blocks[BLOCKS_PER_INODE-1]; //copiar bloco pro bloco
			readBlocktoBlock(&block, fs->superblock->block_size, file);
			for(j=0;j< current_indir;j++) {
				temp = block;
				temp.id = getIntAtBlock (indirection_lines, &block);
				readBlocktoBlock (&temp, fs->superblock->block_size, file);
				block = temp;
			}
			for(k=0;k<m;
			if(i==0) {
				getFreeDatablock(fs, &temp);
				setIntAtBlock(fs->superblock->block_size-sizeof(int32_t), &block, temp.id); 
				writeBlock(block.id, file, &block, fs->superblock->block_size);
				block = temp;
			}
			setIntAtBlock (i*sizeof(int32_t), &block, filho);
			writeBlock(block.id, file, &block, fs->superblock->block_size);
			return 1;
		}*/

    fclose(arq);
    fclose(ufs);
    free(fs);
}

int32_t getFatherfromPathname (char* pathname, Filesystem fs, Inode current_dir) {

    char* tok = malloc((INODE_TYPE_SIZE+INODE_NAME_SIZE)*sizeof(char));
    tok = strtok(pathname, "/");
    Inode dir = current_dir;
    char* child = malloc((INODE_TYPE_SIZE+INODE_NAME_SIZE)*sizeof(char));
    char valid = true;
    Inode x;
    Inode father;

    while (tok != NULL) {
        child = tok;
        father = dir;
        x = searchInodeOnDirByName(fs, dir, child);
        if(x != NULL && valid) {
            dir = fs->inodes[x->number];
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

    return father->number;
}

int32_t getInodeNumberfromPathname (char *pathname, Filesystem fs, Inode current_dir){

    char* tok = malloc((INODE_TYPE_SIZE+INODE_NAME_SIZE)*sizeof(char));
    tok = strtok(pathname, "/");
    Inode dir = current_dir;
    char* child = malloc((INODE_TYPE_SIZE+INODE_NAME_SIZE)*sizeof(char));
    char valid = true;
    Inode x;

    while (tok != NULL) {
        child = tok;
        x = searchInodeOnDirByName(fs, dir, child);
        if(x != NULL && valid) {
            dir = fs->inodes[x->number];
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
        return dir->number;
    else
        return -1; //path existe a menos do ultimo
}

char* getFullNamefromPathname (char* pathname){

    char* name = malloc((INODE_NAME_SIZE+INODE_TYPE_SIZE+1)*sizeof(char));
    char* tok = malloc(256);
    int32_t i,p = 0;

    tok = strtok(pathname, "/");

    while(tok != NULL) {
        name = tok;
        tok = strtok(NULL, "/");
    }

    free(tok);

    //Check name size
    for(i=0;i<INODE_NAME_SIZE;i++) {
        if(name[i] == '.') {
            p = i+1;
            break;
        }
        if(name[i] == '\0') break;
    }
    if(i==INODE_NAME_SIZE) {
        warning ("Invalid Filename size! Format(size): <INODE_NAME_SIZE>.<INODE_TYPE_SIZE>");
        free(name);
        return NULL;
    }

    //Check type size
    if(p == 0) return name;
    
    for(i=0;i<INODE_TYPE_SIZE;i++){
        if(name[p+i] == '\0') break;
    }
    if(i==INODE_TYPE_SIZE) {
        warning ("Invalid Filename size! Format(size): <INODE_NAME_SIZE>.<INODE_TYPE_SIZE>");
        free(name);
        return NULL;
    }

	printf("\n\n Peguei o full name\n\n");
    return name;
}

int32_t checkNameValidity (char* name){

	int32_t i = 0;
	int32_t p = 0;
	
	if(name == NULL)
		error("Invalid Filename!");

	for(i=0;i<INODE_NAME_SIZE;i++) {
        if(name[i] == '.') {
            p = i+1;
            break;
        }
        if(name[i] == '\0') break;
    }
    if(i==INODE_NAME_SIZE) {
        //warning ("Invalid Filename size! Format(size): <INODE_NAME_SIZE>.<INODE_TYPE_SIZE>");
        free(name);
        return -1; // InvalidName
    }

    //Check type size
    if(p == 0) return +1; // Name (No type)
    
    for(i=0;i<INODE_TYPE_SIZE;i++){
        if(name[p+i] == '\0') break;
    }
    if(i==INODE_TYPE_SIZE) {
        //warning ("Invalid Filename size! Format(size): <INODE_NAME_SIZE>.<INODE_TYPE_SIZE>");
        free(name);
        return -2; //Name.InvalidType
    }
	
	return +2; // Name.Type
}

char* getName(char* full_name) {

	char* name = malloc(INODE_NAME_SIZE);

	for(int i=0;i<INODE_NAME_SIZE;i++) {
		
        if(full_name[i] == '.')
            break;
        name[i] = full_name[i];
        
        if(full_name[i] == '\0') break;
    }

    return name;
	
}

char* getType(char* full_name) {

	char* type = malloc(INODE_TYPE_SIZE);

	int32_t i = 0;
	int32_t p = 0;

	for(i=0;i<INODE_NAME_SIZE;i++) {
        if(full_name[i] == '.') {
            p = i+1;
            break;
        }
        if(full_name[i] == '\0') break;
    }

    //Check type size
    
    for(i=0;i<INODE_TYPE_SIZE;i++){
		type[i] = full_name[i];
        if(full_name[p+i] == '\0') break;
    }
	
	return type;
}



