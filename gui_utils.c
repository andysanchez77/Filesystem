#include "gui_utils.h"

int copyuv(char *arg1, char *arg2) {
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

struct INODE* dir_root_gui(){
    load_sec_boot();
    load_sec_mapa_nodosi();
    load_inodes();
    int i, j, nfiles;
    printf("%6s\t%20s\t%8s\n", "Nodo i","Nombre de Archivo","Tam");
    printf("----------------------------------------------------\n");
    struct INODE* result = (struct INODE*) malloc(sizeof(struct INODE));
    for(i=0, nfiles=0; i< INODE_SIZE; i++) {
        if(mapa_bits_nodosi[i/8] & 1<<(i%8)){
            result[nfiles] = inodes[i];
            //printf("%6d\t%20s\t%8d\n", i, inodes[i].name,inodes[i].size);
            nfiles++;
            result = realloc(result, sizeof(struct INODE) * (nfiles+1));
        }
    }
    result[nfiles].size = -1;
    printf("Total: %d\n", nfiles);

    return result;

}