
#include "Archivos.h"

int openfiles_inicializada=0;
struct OPENFILES openfiles[16];

extern int disk_n;
extern int secboot_en_memoria;
extern int inodesmap_en_memoria;
extern int secbloques_en_memoria;
extern int inodes_en_memoria;
extern struct SECBOOT secboot;
extern unsigned char mapa_bits_nodosi [SECTOR_SIZE];
extern unsigned char mapa_bits_bloques [SECTOR_SIZE * 2];
extern struct INODE inodes[INODE_SIZE];



void init_open_files_table(){
	if(!openfiles_inicializada){
		for(int i=3; i<16;i++){
			openfiles[i].inuse = 0;
			openfiles[i].currbloqueenmemoria = -1;
		}
		openfiles_inicializada = 1;
	}
	
}

//Función que obtiene el gid
int getgid(){
	return 022;
}

//Función que obtiene el uid
int getuid(){
	return 022;
}

//Obtiene un descriptor de archivo libre
int get_free_fd(){
	int i = 3;
	while(openfiles[i].inuse && i < 16)
		i++;

	if(i>=16){
		fprintf(stderr, "No hay descriptores de archivos disponibles\n");
		return -1;
	}
	
	return i;
}

//Función qu retorna un puntero al bloque donde 
//se encuentra la posición dada
unsigned short *postoptr(int fd,int pos){
	int currinode;
	unsigned short *currptr;
	unsigned short indirect;

	currinode = openfiles[fd].inode;

	if((pos/1024)>=2068){
		fprintf(stderr, "El archivo ha alcanzado su tamaño máximo\n");
		return NULL;
	}
	else if((pos/1024)<20){
		currptr = &inodes[currinode].blocks[pos/2048];
	}
	else if((pos/1024)<2048+20){

		//Si el indirecto está vacío, asignarle un bloque
		if(inodes[currinode].indirect1 == 0){
			//Obtener primer bloque disponible
			indirect = nextfreeblock();
			assignblock(indirect);
			inodes[currinode].indirect1 = indirect;
			DEBUG("Puntero indirecto [%d]\n", indirect);
		}		
		currptr = &openfiles[fd].buffindirect[pos/2048-10];
		//DEBUG("%s\n", *currptr);
	}
	else
		return NULL;

	return currptr;

}

//Regresa el puntero al bloque donde se encuentra el puntero del archivo
unsigned short *currpostoptr(int fd){
	// openfiles[fd].currpos es la posición actual del puntero del 
	// archivo
	return postoptr(fd,openfiles[fd].currpos);
}

//Función para abrir un archivo
int vdopen(char *filename, unsigned short mode){
	int numinode;
	int i;
	unsigned short currblock;

	//Ver si existe el archivo
	numinode = searchinode(filename);
	if(numinode == -1){
		fprintf(stderr,"vdopen: El archivo no existe\n");
		return -1;
	}

	//Inicializar tabla si es necesario
	init_open_files_table();

	//Buscar un lugar en tabla de archivos
	i = get_free_fd();

	if(i == -1)
		return -1;
	

	openfiles[i].inuse = 1;
	openfiles[i].inode = numinode;
	openfiles[i].currpos = 0;

	//Si existe apuntador indirecto, cargarlo
	if(inodes[numinode].indirect1 != 0){
		DEBUG("Cargando buffer indirecto %d\n", inodes[numinode].indirect1 );
		readblock(inodes[numinode].indirect1, 
				 (char *) openfiles[i].buffindirect);
	}

	//Cargar el buffer con el bloque actual, en este caso el primer bloque
	currblock = *currpostoptr(i);

	readblock(currblock, openfiles[i].buffer);
	openfiles[i].currbloqueenmemoria = currblock;

	return i;
}

//Función para crear un archivo
int vdcreat(char *filename, unsigned short perms){
	int numinode, i;

	//Comprobar que no exista el archivo
	numinode = searchinode(filename);

	//Si el archivo no existe
	if(numinode == -1){
		numinode=nextfreeinode();
		if(numinode == -1){
			fprintf(stderr, "vdcreat: Sistema de archivos lleno\n");
			return -1;
		}
	}else
		removeinode(numinode);
	//Escribir el archivo en el inodo
	setninode(numinode, filename, perms, getuid(), getgid());
	//assigninode(numinode);

	//Verificar que la tabla dea rchvios abiertos esté en memoria
	init_open_files_table();

	//Buscar un lugar en tabla de archivos
	i = get_free_fd();

	if(i == -1)
		return -1;
	

	//Poner el archivo en la tabla de archivos abiertos
	openfiles[i].inuse = 1;
	openfiles[i].inode = numinode;
	openfiles[i].currpos = 0;
	openfiles[i].currbloqueenmemoria = -1;

	return i;
}

//Elimina un archivo
int vdunlink(char *filename){
	int numinode, i;

	numinode = searchinode(filename);
	if(numinode == -1){
		fprintf(stderr,"vdunlink: El archivo no existe\n");
		return -1;
	}
	DEBUG("Removing inode %d\n", numinode);
	removeinode(numinode);
}

//Posiciona el puntero de un archivo
int vdseek(int fd, int offset, int whence){
	unsigned short oldblock, newblock;

	if(openfiles[fd].inuse == 0){
		DEBUG("No está abierto el archivo\n");
		return -1;
	}

	oldblock = *currpostoptr(fd);

	if(whence == 0){
		if(offset < 0 || 
		   openfiles[fd].currpos+offset > inodes[openfiles[fd].inode].size){
			DEBUG("vdseek: Error whence 0\n");
			return -1;
		}
		openfiles[fd].currpos = offset;
	}else if(whence == 1){
			if(openfiles[fd].currpos+offset > inodes[openfiles[fd].inode].size
			  || openfiles[fd].currpos+offset < 0){
				DEBUG("vdseek: Error whence 1\n");
				return -1;
			}
			openfiles[fd].currpos+=offset;
	}else if(whence == 2){
		if(offset > inodes[openfiles[fd].inode].size ||
			openfiles[fd].currpos-offset < 0){
			DEBUG("vdseek: Error whence 2\n");
			return -1;
		}
		openfiles[fd].currpos = inodes[openfiles[fd].inode].size-offset;
	}else{
		DEBUG("Whence no válido\n");
		return -1;
	}

	newblock=*currpostoptr(fd);

	if(newblock!=oldblock)
	{
		writeblock(oldblock,openfiles[fd].buffer);
		readblock(newblock,openfiles[fd].buffer);
		openfiles[fd].currbloqueenmemoria=newblock;
	}

	return(openfiles[fd].currpos);
}

//Escribe en un archivo
int vdwrite(int fd, char *buffer, int bytes){
	int currblock, currinode, sector, i, result;
	int cnt = 0;
	unsigned short *currptr;

	//Checar que tabla de archivos abiertos esté en memoria
	init_open_files_table();

	//Checar que el archivo esté abierto
	if(openfiles[fd].inuse == 0){
		fprintf(stderr,"vdwrite: Archivo no abierto\n");
		return -1;
	}

	//Obtener inodo del archivo
	currinode = openfiles[fd].inode;

	//Escribir buffer al archivo
	while(cnt < bytes){
		currptr = currpostoptr(fd);

		if(currptr == NULL){
			DEBUG("vdwrite: Apuntador a nulo\n");
			return -1;
		}

		currblock = *currptr;

		//Si el bloque está en blanco, asignarle un nuevo bloque
		if(currblock == 0){
			currblock = nextfreeblock();
			*currptr = currblock;
			DEBUG("Asignando nuevo bloque %d\n", currblock);

			assignblock(currblock);
			//Escribir el sector del nodo i en disco
			sector = (currinode/8);

			result = vdwritesl(get_secl_tabla_nodos_i()+sector, 
					 (char *) &inodes[sector *8]);

		}

		//Cargar bloque al buffer del archivo
		if(openfiles[fd].currbloqueenmemoria!=currblock){
			readblock(currblock, openfiles[fd].buffer);
			openfiles[fd].currbloqueenmemoria = currblock;
		}

		//Copia caracter
		openfiles[fd].buffer[openfiles[fd].currpos%2048]=buffer[cnt];

		//Incrementa la posición
		openfiles[fd].currpos++;

		//Si la posición es mayor que el tamaño, modificar size
		if(openfiles[fd].currpos > inodes[currinode].size)
			inodes[openfiles[fd].inode].size = openfiles[fd].currpos;

		//Incrementar contador
		cnt++;

		//Si se llena el buffer escribir en disco
		if(openfiles[fd].currpos%2048==0){
			writeblock(inodes[currinode].indirect1,
					  (char *) openfiles[fd].buffindirect);
			writeblock(currblock,openfiles[fd].buffer);
		}

		if(buffer[cnt-1]=='\0')
			break;
	}
	return cnt;
}

//Lee un archivo
int vdread(int fd, char *buffer, int bytes){
	int currblock, currinode, sector, i, result;
	int cnt = 0;
	unsigned short *currptr;

	//Checar que tabla de archivos abiertos esté en memoria
	init_open_files_table();

	//Checar que el archivo esté abierto
	if(openfiles[fd].inuse == 0){
		fprintf(stderr,"vdread: Archivo no abierto\n");
		return -1;
	}

	//Obtener inodo del archivo
	currinode = openfiles[fd].inode;

	//Leer del archivo
	while(cnt < bytes){
		currptr = currpostoptr(fd);

		//Si apunta a nulo, salir
		if(currptr == NULL){
			return -1;
		}

		currblock = *currptr;

		if(currblock == 0){
			DEBUG("vdread: Bloque vacío\n");
			return 0;
		}

		//Cargar bloque si no está en memoria
		if(openfiles[fd].currbloqueenmemoria != currblock){
			readblock(currblock, openfiles[fd].buffer);
			openfiles[fd].currbloqueenmemoria = currblock;
		}

		if(openfiles[fd].buffer[openfiles[fd].currpos%2048] == '\0')
			return cnt;

		//Copiar al buffer el caracter actual
		buffer[cnt] = openfiles[fd].buffer[openfiles[fd].currpos%2048];

		
		//Incrementar posición
		openfiles[fd].currpos++;
		//Incrementar puntero
		cnt++;

	}
	return cnt;
}

//Cierra un archivo
int vdclose(int fd){
	int currinode, currblock, sector, result, saveblock;
	char *buffer;

	//Checar que tabla de archivos abiertos esté en memoria
	init_open_files_table();

	//Checar que el archivo esté abierto
	if(openfiles[fd].inuse == 0){
		fprintf(stderr,"vdclose: Archivo no abierto\n");
		return -1;
	}

	currinode = openfiles[fd].inode;

	sector = (currinode/8);

	result = vdwritesl(get_secl_tabla_nodos_i()+sector, 
					  (char *) &inodes[sector *8]);

	currblock = openfiles[fd].currbloqueenmemoria;
	if(currblock != -1){
		buffer = calloc(2048, sizeof(char));
		saveblock = readblock(currblock, buffer);
		if(strcmp(buffer,openfiles[fd].buffer) != 0){
			DEBUG("vdclose: Escribiendo bloque del buffer\n");
			writeblock(currblock, openfiles[fd].buffer);
		}
	}
	writeblock(inodes[currinode].indirect1, 
			  (char *) openfiles[fd].buffindirect);
	openfiles[fd].inuse = 0;
	openfiles[fd].currbloqueenmemoria = -1;
	memset(openfiles[fd].buffer,0,2048);
	memset((char *)openfiles[fd].buffindirect, 0, 2048);

	return 1;
}

//
int dir_root(){
	load_sec_boot();
	load_sec_mapa_nodosi();
	load_inodes();
	int i, j, nfiles;
	printf("%6s\t%20s\t%8s\n", "Nodo i","Nombre de Archivo","Tam");
	printf("----------------------------------------------------\n");
	for(i=0, nfiles=0; i< INODE_SIZE; i++)
		if(mapa_bits_nodosi[i/8] & 1<<(i%8)){
			printf("%6d\t%20s\t%8d\n", i, inodes[i].name,inodes[i].size);
			nfiles++;
		}
	printf("----------------------------------------------------\n");
	printf("Total: %d\n", nfiles);

	return 1;

}