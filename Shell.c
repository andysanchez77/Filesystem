#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Archivos.h"

#define MAXLEN 80
#define BUFFERSIZE 512

extern struct INODE inodes[64];

void locateend(char *cmd);
int executecmd(char *cmd);
int isinvd(char *arg);
int copyuu(char *arg1, char *arg2);
int copyuv(char *arg1, char *arg2);
int copyvu(char *arg1, char *arg2);
int copyvv(char *arg1, char *arg2);
int catv(char *arg1);
int catu(char *arg1);
int dirv();
int diru(char *arg1);

int main(){
	char linea[MAXLEN];
	int result=1;
	// char a1[80] = "/home/ines/Documents/testfiles/texas.txt";
	// char a2[80];
	// int n = 0;
	// a2[2] = '\0';
	// for(int j=65; j<75; j++){
	// 	a2[0] = (char) j;
	// 	for(int i=65; i< 75; i++){
	// 	 	a2[1] = (char) i;
	//  		copyuv(a1, a2);
	//  		n++;
	//  		if(n == 40) break;
	// 	}
	// 	if(n == 40) break;
	// }
	while(result){
		printf("\033[1;36mvshell >\033[0m ");
		fflush(stdout);
		read(0,linea,80);
		locateend(linea);
		if(linea[0] == '\0')
			continue;
		result=executecmd(linea);
	}



}

void locateend(char *linea){
	int i=0;
	while(i<MAXLEN && linea[i]!='\n')
		i++;
	linea[i] = '\0';
}

int executecmd(char *linea){
	char *cmd;
	char *arg1;
	char *arg2;
	char *search = " ";

	cmd = strtok(linea, " ");
	arg1 = strtok(NULL, " ");
	arg2 = strtok(NULL, " ");

	if(strcmp(cmd, "exit") == 0)
		return 0;
	if(strcmp(cmd, "copy") == 0){
		if(arg1 == NULL || arg2 == NULL){
			fprintf(stderr, "Error en los argumentos\n");
			return 1;
		}
		if(!isinvd(arg1) && !isinvd(arg2))
			copyuu(&arg1[2],&arg2[2]);

		else if(!isinvd(arg1) && isinvd(arg2))
			copyuv(&arg1[2],arg2);

		else if(isinvd(arg1) && !isinvd(arg2))
			copyvu(arg1,&arg2[2]);
		
		else if(isinvd(arg1) && isinvd(arg2))
			copyvv(arg1,arg2);
	}

	if(strcmp(cmd,"cat")==0)
	{
		if(arg1 == NULL){
			fprintf(stderr, "Error en los argumentos\n");
			return 1;
		}

		if(isinvd(arg1))
			catv(arg1);
		else
			catu(&arg1[2]);
	}

	if(strcmp(cmd, "dir") == 0){
		if(arg1 == NULL)
			dirv();
		else if(!isinvd(arg1))
			diru(&arg1[2]);
	}

	if(strcmp(cmd, "delete") == 0){
		if(isinvd(arg1))
			vdunlink(arg1);
		else
			unlink(&arg1[2]);
	}
	return 1;
}

//Regresa 0 si es un nombre de archivo de Unix
int isinvd(char *arg){
	if(strncmp(arg, "//", 2)!= 0)
		return 1;
	return 0;
}

//Copia entre sistemas de archivos de Unix
int copyuu(char *arg1, char *arg2){
	int sfile, dfile;
	char buffer[BUFFERSIZE];
	int ncars;

	sfile = open(arg1, 0);
	dfile = creat(arg2, 0640);

	do{
		ncars = read(sfile, buffer, BUFFERSIZE);
		write(dfile, buffer, ncars);
	}while(ncars == BUFFERSIZE);
	close(sfile);
	close(dfile);
	return 1;
}

//Copia archivo de unix a sistema de archvio virtual
int copyuv(char *arg1, char *arg2){
	int sfile, dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	sfile = open(arg1,0);
	dfile = vdcreat(arg2, 0640);

	do{
		ncars = read(sfile, buffer, BUFFERSIZE);
		vdwrite(dfile, buffer, ncars);
	}while(ncars == BUFFERSIZE);
	close(sfile);
	vdclose(dfile);
	return 1;
}

//Copia un archivo de disco virtual a un archivo de unix

int copyvu(char *arg1, char *arg2){
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;

	sfile = vdopen(arg1, 0);
	dfile = creat(arg2, 0640);
	vdseek(sfile, 0, 0);
	do{
		ncars = vdread(sfile, buffer, BUFFERSIZE);
		write(dfile, buffer, ncars);
	}while(ncars == BUFFERSIZE);
	vdclose(sfile);
	close(dfile);
	return 1;
}


//Copia entre archivos de disco virtual
int copyvv(char *arg1, char *arg2){
	int sfile, dfile;
	char buffer[BUFFERSIZE];
	int ncars;

	sfile=vdopen(arg1, 0);
	dfile = vdcreat(arg2, 0640);
	vdseek(sfile, 0, 0);
	do{
		ncars = vdread(sfile, buffer, BUFFERSIZE);
		vdwrite(dfile, buffer, ncars);
	}while(ncars == BUFFERSIZE);
	vdclose(sfile);
	vdclose(dfile);
	return 1;
}

//Despliega contenido de un archivo virtual
int catv(char *arg1){
	int sfile, ncars;
	char buffer[BUFFERSIZE];

	sfile = vdopen(arg1,0);
	vdseek(sfile, 0, 0);
	do{
		ncars = vdread(sfile, buffer, BUFFERSIZE);
		DEBUG("ncars = %d\n", ncars);
		write(1, buffer, ncars);
	}while(ncars == BUFFERSIZE);
	vdclose(sfile);
	return 1;
}

//Despliega contenido de un archivo de unix
int catu(char *arg1){
	int sfile, ncars;
	char buffer[BUFFERSIZE];

	sfile = open(arg1,0);
	do{
		ncars = read(sfile, buffer, BUFFERSIZE);
		write(1, buffer, ncars);
	}while(ncars == BUFFERSIZE);
	close(sfile);
	return 1;
}


int dirv(){
	dir_root();
}

int diru(char *arg1){
	DIR *dd;
	struct dirent *entry;
	int entries = 0;
	if(arg1[0] == '\0')
		strcpy(arg1,".");

	printf("Directorio %s\n",arg1);

	dd = opendir(arg1);
	if(dd == NULL){
		fprintf(stderr,"Error al abrir directorio\n");
		return -1;
	}
	printf("%8s\t%30s\t%5s\n", "Nodo i","Nombre de Archivo","Tipo");
	printf("---------------------------------------------------------------\n");

	while((entry=readdir(dd))!=NULL){
		if(entry->d_type == 4)
			printf("\033[1;32m%8d\t%30s\t%5d\n\033[0m", entry->d_ino,entry->d_name,"dir");
		else

		printf("%8d\t%30s\t%5d\n", entry->d_ino,entry->d_name,"file");
		entries++;
	}
	printf("---------------------------------------------------------------\n");
	printf("Total: %d\n", entries);
	closedir(dd);
}