#include "Archivos.h"

extern int disk_n;
extern int secboot_en_memoria;
extern int inodesmap_en_memoria;
extern int secbloques_en_memoria;
extern struct SECBOOT secboot;
extern unsigned char mapa_bits_nodosi [SECTOR_SIZE];
extern unsigned char mapa_bits_bloques [SECTOR_SIZE * 2];
extern struct INODE inodes[64];

extern int openfiles_inicializada;
extern struct OPENFILES openfiles[16];

int main(){
	printf("-----------MAIN DE PRUEBA----------\n");
	
	load_sec_boot();
	DEBUG("Sector lógico mapa bits nodos i: %d\n",get_secl_mapa_nodos_i());
	DEBUG("Sector lógico mapa bits bloques: %d\n",get_secl_mapa_bloques());
	DEBUG("Sector lógico mapa tabla nodos i: %d\n",get_secl_tabla_nodos_i());
	DEBUG("Sector lógico mapa área de datos: %d\n",get_secl_data());
	
	int i;

	struct INODE inode;

	char filename[20] = "myfile\0";
	vdcreat(filename,0777);
	// for(i=0; i<48; i++)
	// 	assigninode(i);

	// int free_node;
	// char filename[20];
	// int letra = 64;
	// for(; i<64; i++){
	// 	letra++;
	// 	free_node = nextfreeinode();
	// 	for(int j=0; j<20; j++)
	// 		filename[j] = letra;
	// 	if(free_node>-1 && free_node == setninode(free_node, filename, 0666, 20, 3))
	// 		DEBUG("Éxito guardando!\n");

	// }
	// int search_inode = searchinode(filename);
	//removeinode(62);

}