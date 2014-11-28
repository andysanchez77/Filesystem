#define SECTOR_SIZE		512

struct SECBOOT {	
	char jump[4];
	char nombre_disco[8];
	unsigned char sec_res;
	unsigned char sec_mapa_bits_nodos_i;
	unsigned char sec_mapa_bits_bloques;
	unsigned short sec_tabla_nodos_i;
	unsigned short sec_log_unidad;
	unsigned char sec_x_bloque;
	unsigned char heads;
	unsigned char cyls;
	unsigned char secfis;
	char restante[487];
};

struct INODE {
	char name[20];
	unsigned short uid;
	unsigned short gid;
	unsigned short perms;
	unsigned int datetimecreat;
	unsigned int datetimemodif;
	unsigned int size;
	unsigned short blocks[10];
	unsigned short indirect1;
	unsigned short indirect2;
};