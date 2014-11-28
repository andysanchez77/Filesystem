#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "filesystem.h"
#include "vdisk.h"

#define LINESIZE 16
#define SECSIZE 512

int main(int argc, char *argv[])
{
	int secl, head, cyl, secf;
	unsigned char buffer[SECSIZE], c;
	if (argc != 2) {
		fprintf(stderr,"Error en los argumentos\n");
		exit(1);
	}

	secl = atoi(argv[1]);
	struct SECBOOT t;
	// Por default siempre sera el disco cero
	// Leemos los parámetros del disco
	vdreadsector(0, 0, 0, 1, 1, (char*) &t);

	if (secl < 1 || secl > (t.sec_log_unidad + 1)) {
		fprintf(stderr,
			"Sector lógico fuera del espacio de direcciones del disco\n");
		exit(-1);
	}
	secl --;
	// Calculamos el número de la superficie
	head = (int) (secl / (t.cyls * t.secfis));
	secl -= head * t.cyls * t.secfis;
	// Calculamos el número de cilindro
	cyl = (int) (secl / t.secfis);
	secl -= cyl * t.secfis;
	// Calculamos el número de sector físico
	secf = secl + 1;//+ 1;

	printf("Superficie = %d, Cilindro = %d, SectorF = %d\n\n", head, cyl, secf);

	if(vdreadsector(0, head, cyl, secf, 1, buffer) == -1) {
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