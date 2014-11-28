#define HEADS 16
#define SECTORS 11
#define CYLINDERS 100 

int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);