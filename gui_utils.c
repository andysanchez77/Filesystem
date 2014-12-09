#include "gui_utils.h"

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

int catv(char *arg1, char **buffer, int *size){
    int sfile, ncars;
    
    sfile = vdopen(arg1,0);
    *size = inodes[openfiles[sfile].inode].size;
    //size = ((size/512) * 512) + 512;
    *buffer = (char*) malloc(*size);
    char *tmp = *buffer;
    char buff[512];
    vdseek(sfile, 0, 0);
    do{
        ncars = vdread(sfile, buff, BUFFERSIZE);
        memcpy(tmp, buff, ncars);
        tmp+=ncars;
    }while(ncars == BUFFERSIZE);
    vdclose(sfile);
    return 1;
}

void saveEdit(char *filename, char *buffer) {
    int sfile=vdopen(filename, 0), size = strlen(buffer), ncars = 0;
    char *ptr = buffer, local_buff[512];
    while(size > 0) {
        ncars = size % 512;
        if (ncars == 0){
            if (size == 512)
                ncars = 512;
        }
        else
            local_buff[ncars] = '\0';
        memcpy(local_buff, ptr, ncars);
        vdwrite(sfile, local_buff, ncars);
        size -= ncars;
        ptr += ncars;
    }
    vdclose(sfile);
}

int copyuv(char *arg1, char *arg2){
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

int copyvu(char *arg1, char *arg2){
    int sfile,dfile;
    char buffer[BUFFERSIZE];
    int ncars;

    sfile = vdopen(arg1, 0);
    dfile = creat(arg2, 0640);
    vdseek(sfile, 0, 0);
    do{
        ncars = vdread(sfile, buffer, BUFFERSIZE);
        write(dfile, buffer, ncars);
    }while(ncars == BUFFERSIZE);
    vdclose(sfile);
    close(dfile);
    return 1;
}

int copyvv(char *arg1, char *arg2){
    int sfile, dfile;
    char buffer[BUFFERSIZE];
    int ncars;

    sfile=vdopen(arg1, 0);
    dfile = vdcreat(arg2, 0640);
    vdseek(sfile, 0, 0);
    do{
        ncars = vdread(sfile, buffer, BUFFERSIZE);
        vdwrite(dfile, buffer, ncars);
    }while(ncars == BUFFERSIZE);
    vdclose(sfile);
    vdclose(dfile);
    return 1;
}