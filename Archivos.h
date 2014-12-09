#include "filesystem.h"
#include <unistd.h>

struct OPENFILES {
	int inuse;
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[2048];
	unsigned short buffindirect[1024];
};


unsigned short *postoptr(int fd,int pos);
unsigned short *currpostoptr(int fd);
int vdopen(char *filename, unsigned short mode);
int vdcreat(char *filename, unsigned short perms);
int vdunlink(char *filename);
int vdseek(int fd, int offset, int whence);
int vdwrite(int fd, char *buffer, int bytes);
int vdread(int fd, char *buffer, int bytes);
int vdclose(int fd);
int dir_root();