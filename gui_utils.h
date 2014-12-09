#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "Archivos.h"

#define BUFFERSIZE 512

extern int disk_n;
extern int secboot_en_memoria;
extern int inodesmap_en_memoria;
extern int secbloques_en_memoria;
extern int inodes_en_memoria;
extern struct SECBOOT secboot;
extern unsigned char mapa_bits_nodosi [SECTOR_SIZE];
extern unsigned char mapa_bits_bloques [SECTOR_SIZE * 2];
extern struct INODE inodes[INODE_SIZE];
extern struct OPENFILES openfiles[16];

int copyuv(char *arg1, char *arg2);
struct INODE* dir_root_gui();
int catv(char *arg1, char **buffer, int *size);
void saveEdit(char *filename, char *buffer);
int copyuv(char *arg1, char *arg2);
int copyvu(char *arg1, char *arg2);
int copyvv(char *arg1, char *arg2);