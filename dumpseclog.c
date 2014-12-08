#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "filesystem.h"

#define LINESIZE 16
#define SECSIZE 512

extern int disk_n;

int main(int argc, char *argv[])
{
	int secl, head, cyl, secf;
	unsigned char buffer[SECSIZE], c;
	if (argc != 2) {
		fprintf(stderr,"Error en los argumentos\n");
		exit(1);
	}

	secl = atoi(argv[1]);
	// Por default siempre sera el disco cero
	disk_n = 0;
	load_sec_boot();
	if (calculateParams(secl, &head, &cyl, &secf) == -1) {
		fprintf(stderr, "No fue posible calcular los parámetros de lectura física\n");
		exit(-1);
	}

	printf("Superficie = %d, Cilindro = %d, SectorF = %d\n\n",
		head, cyl, secf);

	if(vdreadsector(disk_n, head, cyl, secf, 1, buffer) == -1) {
		fprintf(stderr,"Error al abrir disco virtual\n");
		exit(1);
	}
	for(int i=0;i<SECSIZE/LINESIZE;i++) {
		printf("\n %3X -->",i*LINESIZE);
		for(int j=0;j<LINESIZE;j++) {
			c=buffer[i*LINESIZE+j];
			printf("%02X ",c);
		}
		printf(" | ");
		for(int j=0;j<LINESIZE;j++) {
			c=buffer[i*LINESIZE+j]%256;
			if(c>0x1F && c<127)
				printf("%c",c);
			else
				printf(".");
		}
	}
	printf("\n");
}
