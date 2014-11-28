#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"
#include "vdisk.h"

int isNumber(char const *text);
int format(int disk_number);

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Número de parámetros incorrecto\n");
		exit(-1);
	} else if (isNumber(argv[1])) {
		int disco = atoi(argv[1]);
		printf("Disco = %d\n", disco);
		printf("Size sector boot = %d\n",  sizeof(struct SECBOOT ));
		format(disco);
	} else {
		fprintf(stderr, "El segundo parámetro debe ser un número\n");
		exit(-1);
	}
	return 0;
}

int isNumber(char const *text) {
	for (int i = 0; i < strlen(text); ++i) {
		if (text[i] < '0' || text [i] > '9')
			return 0;
	}

	return 1;
}

// Para la función vdwritesector
// 1: Se manda el número de disco
// 2: Se manda la superficie que empieza en 0
// 3: Se manda el cilindro que empieza en 0
// 4: Se manda el sector pero este empieza en 1
// 5: Se manda la cantidad de sectores que se van a escribir
// 6: Se manda el buffer que contiene lo que se va a escribir
int format(int disk_number) {
	// Procedemos a crear la tabla de parámetros del disco
	struct SECBOOT t;
	sprintf(t.nombre_disco, "disco%d.vd", disk_number);
	t.sec_res = 1;
	t.sec_mapa_bits_nodos_i = 2;
	t.sec_mapa_bits_bloques = 3;
	t.sec_tabla_nodos_i = 9;
	t.sec_log_unidad = 17600;
	t.sec_x_bloque = 4;
	t.heads = 16;
	t.cyls = 100;
	t.secfis = 11;

	if (vdwritesector(disk_number, 0, 0, t.sec_res, 1,  (char*) &t) != 1) {
		fprintf(stderr,
			"Ocurrió un error mientras escribíamos la tabla de parámetros\n");
	}
	printf("Éxito escribiendo la tabla de parámetros en el disco\n");

	// Procedemos ahora a limpiar los bits del mapa de bits de nodos-i
	char buff[SECTOR_SIZE];
	memset(buff, 0x00, SECTOR_SIZE);
	if (vdwritesector(disk_number, 0, 0, t.sec_mapa_bits_nodos_i, 1,  buff)
		!= 1) {
			fprintf(stderr,
				"Ocurrió un error al querer limpiar el mapa de bits de nodos-i\n");
	}
	printf("Éxito escribiendo el mapa de bits de nodos-i en el disco\n");

	// Ahora solo cambiamos el primer bit para el mapa de bits del área de datos
	buff[0] = 0x01;
	if (vdwritesector(disk_number, 0, 0, t.sec_mapa_bits_bloques, 1,  buff)
		!= 1) {
			fprintf(stderr,
				"Ocurrió un error al querer limpiar el mapa de bits de datos\n");
	}
	printf("Éxito escribiendo el mapa de bits de datosen el disco\n");
}