#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vdisk.h"

#define SECTOR_SIZE		512
#define INODE_SIZE      64

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

struct DATE {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};

struct OPENFILES {
	int inuse;
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[1024];
	unsigned short buffindirect[512];
};


int calculateParams(int secl, int *head, int *cyl, int *secf);
int vdreadsl(int sec_loc, int nsectors, char *buffer);
int vdwritesl(int sec_loc, int nsectors, char *buffer);
void load_sec_boot();
void load_sec_mapa_nodosi();
void load_sec_mapa_bloques();
int get_secl_mapa_nodos_i();
int get_secl_mapa_bloques();
int get_secl_tabla_nodos_i();
int get_secl_data();

int isinodefree(int inode);
int nextfreeinode();
int assigninode(int inode);
int unassigninode(int inode);
int isblockfree(int bloque);
int nextfreeblock();
int assignblock(int bloque);
int unassignblock(int block);
int writeblock(int bloque, char *buffer);
int readblock(int bloque, char *buffer);
unsigned int datetoint(struct DATE date);
int inttodate(struct DATE *date,unsigned int val);
unsigned int currdatetimetoint();
