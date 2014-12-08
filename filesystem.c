#include "filesystem.h"

int disk_n = 0;
int secboot_en_memoria = 0;
int inodesmap_en_memoria = 0;
int secbloques_en_memoria = 0;
int inodes_en_memoria = 0;
struct SECBOOT secboot;
unsigned char mapa_bits_nodosi [SECTOR_SIZE];
unsigned char mapa_bits_bloques [SECTOR_SIZE * 2];
struct INODE inodes[64];

int calculateParams(int secl, int *head, int *cyl, int *secf) {
	// Leemos los parámetros del disco
	//vdreadsector(disk_n, 0, 0, 1, 1, (char*) &t);
	if(!secboot_en_memoria){
	    fprintf(stderr,"Sector de boot no está en memoria\n");
	    return -1;
	}

	if (secl < 0 || secl >= secboot.sec_log_unidad) {
		fprintf(stderr,
			"Sector lógico fuera del espacio de direcciones del disco\n");
		return -1;
	}
	*head = (secl / 11) % secboot.heads;
	*cyl = (secl / (11 * secboot.heads));
	*secf = (secl % 11) + 1;
	return 1;
}

//Lee y guarda en buffer la información del sector lógico sec_loc
int vdreadsl(int sec_loc, char *buffer) {
	int head, cyl, secf;
	if (calculateParams(sec_loc, &head, &cyl, &secf) == -1) {
		fprintf(stderr, 
		"No fue posible calcular los parámetros de lectura física\n");
		return -1;
	}
	//printf("READ sup %d cil %d secf %d\n", head, cyl, secf);
	vdreadsector(disk_n, head, cyl, secf, 1, buffer);  

	return 1;  
}

//Escribe la información de buffer en el sector lógico sec_loc
int vdwritesl(int sec_loc, char *buffer) {
	int head, cyl, secf;
	if (calculateParams(sec_loc, &head, &cyl, &secf) == -1) {
		fprintf(stderr, 
		"No fue posible calcular los parámetros de lectura física\n");
		return -1;
	}
	//DEBUG(" sl %d sup %d cil %d secf %d\n", sec_loc, head, cyl, secf);
	vdwritesector(disk_n, head, cyl, secf, 1, buffer);

	return 1;
}


/*************************************************************
 * Funciones que obtienen los nums. de sectores lógicos
 * ***********************************************************/
 
//Obtiene el número de sector lógico del mapa de nodos i
int get_secl_mapa_nodos_i(){
	return secboot.sec_res;	
}

//Obtiene el número de sector lógico del mapa de bloques
int get_secl_mapa_bloques(){
	return (secboot.sec_res + secboot.sec_mapa_bits_nodos_i);	
}

//Obtiene el número de sector lógico de la tabla de nodos i
int get_secl_tabla_nodos_i(){
	return (secboot.sec_res + secboot.sec_mapa_bits_nodos_i +
		secboot.sec_mapa_bits_bloques);	
}

//Obtiene el número de sector lógico del sector de datos
int get_secl_data(){
	return (secboot.sec_res + secboot.sec_mapa_bits_nodos_i +
		secboot.sec_mapa_bits_bloques + secboot.sec_tabla_nodos_i);	
}

/*************************************************************
 * Funciones que cargan los sectores principales
 * ***********************************************************/

//Carga en memoria el sector de boot si es que no está cargado
void load_sec_boot(){
	if(!secboot_en_memoria){
		vdreadsector(disk_n,0,0,1,1,(char *) &secboot);
		secboot_en_memoria = 1;
	}
}

//Carga en memoria el sector de mapa de bits de nodos i si es que no 
//está cargado
void load_sec_mapa_nodosi(){
	if(!inodesmap_en_memoria){
		int seclnodosi = get_secl_mapa_nodos_i();
		int bhead, bcyl, bsecf;
		
		calculateParams(seclnodosi, &bhead, &bcyl, &bsecf);
		//printf("sl: %d sup %d cil %d secf %d\n", seclnodosi, bhead, 
		//bcyl, bsecf);
		vdreadsector(disk_n, bhead, bcyl, bsecf,
			        secboot.sec_mapa_bits_nodos_i, mapa_bits_nodosi);
		inodesmap_en_memoria = 1;
	}
}


//Carga en memoria el sector de mapa de bits de bloques si es que no 
//está cargado
void load_sec_mapa_bloques(){
	if(!secbloques_en_memoria){
		int seclbloques = get_secl_mapa_bloques();
		int bhead, bcyl, bsecf;

		calculateParams(seclbloques, &bhead, &bcyl, &bsecf);
		//printf("sl: %d sup %d cil %d secf %d\n", seclbloques, bhead, 
		//bcyl, bsecf);
		vdreadsector(disk_n, bhead, bcyl, bsecf,
			        secboot.sec_mapa_bits_bloques, mapa_bits_bloques);
		secbloques_en_memoria = 1;
	}
}

//Carga en memoria la tabla de nodos i si es que no está cargado
void load_inodes(){
	if(!inodes_en_memoria){
		load_sec_boot();
		int seclnodosi = get_secl_tabla_nodos_i();
		for(int i=0; i < secboot.sec_tabla_nodos_i; i++){
			vdreadsl(seclnodosi + i, (char *)&(inodes[i*8]));
		}
		inodes_en_memoria = 1;
	}
}

/*************************************************************
 * Funciones para manejo del mapa de nodos i
 * ***********************************************************/

//Regresa 1 si el nodo está libre, sino regresa 0
int isinodefree(int inode){
    int offset = inode/8;
    int shift = inode%8;
    int result;
    
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos i esté en memoria
    load_sec_mapa_nodosi();
    
    if(mapa_bits_nodosi[offset] & (1<<shift))
        return 0;
        
    return 1;
}

//Regresa el número del primer nodo i libre
int nextfreeinode(){
    int i, j;
    int total_bytes;
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos i esté en memoria
    load_sec_mapa_nodosi();
    
    total_bytes = (secboot.sec_tabla_nodos_i * SECTOR_SIZE)/INODE_SIZE/8;
    //total_bytes = secboot.sec_mapa_bits_nodos_i * SECTOR_SIZE;
    for(i=0; (mapa_bits_nodosi[i] == 0xFF && 
             i < total_bytes); i++);
    
    if(i < total_bytes){
        j = 0;
        while(mapa_bits_nodosi[i] & (1<<j) && (j<8))
            j++;
        DEBUG("Nextfreenode = %d\n", i*8+j);
        return (i*8+j);
    }
    return -1;
}

//Marca como ocupado un nodo i libre y regresa 1.
int assigninode(int inode){
    int offset=inode/8;
    int shift=inode%8;
    int secl_inodesmap = get_secl_mapa_nodos_i();
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos i esté en memoria
    load_sec_mapa_nodosi();
    
    mapa_bits_nodosi[offset]|=(1<<shift);
    return vdwritesl(secl_inodesmap, mapa_bits_nodosi);
}

//Marca como libre un nodo i libre y regresa 1.
int unassigninode(int inode){
    int offset=inode/8;
    int shift=inode%8;
    int secl_inodesmap = get_secl_mapa_nodos_i();
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos i esté en memoria
    load_sec_mapa_nodosi();
    
    mapa_bits_nodosi[offset]&= (char) ~(1<<shift);
    return vdwritesl(secl_inodesmap, mapa_bits_nodosi);
}

/*************************************************************
 * Funciones para el manejo del mapa de bits de bloques
 * ***********************************************************/
 
//Regresa 1 si el bloque está libre, sino regresa 0
int isblockfree(int bloque){
    int offset=bloque/8;
    int shift=bloque%8;
    
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos de bloques esté en memoria
    load_sec_mapa_bloques();
    
    if(mapa_bits_bloques[offset] & (1<<shift))
        return 0;
    return 1;
 }

//Función que regresa el número del primer bloque vacío.
int nextfreeblock(){
    int i, j;
    int total_bytes, valid_block;
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos de bloques esté en memoria
    load_sec_mapa_bloques();
    
    //total_bytes = secboot.sec_mapa_bits_bloques*SECTOR_SIZE;
    total_bytes = secboot.sec_mapa_bits_bloques * SECTOR_SIZE;
    valid_block = (secboot.sec_log_unidad -(
    			  secboot.sec_res + secboot.sec_mapa_bits_nodos_i + 
                  secboot.sec_mapa_bits_bloques + secboot.sec_tabla_nodos_i)
    			  )/secboot.sec_x_bloque;
                  
    for(i=0; (mapa_bits_bloques[i] == 0xFF && i < total_bytes - 1); i++);
    
    if(i < total_bytes){
        //printf("i=%d %x total_bytes=%d\n",i,mapa_bits_bloques[i],total_bytes);
        j=0;
        while(mapa_bits_bloques[i] & (1<<j) && j<8)
            j++;
        int bloque = i*8+j;
        if (bloque <= valid_block)
            return bloque;
    }
    return -1;
}

//Función que asigna un bloque como ocupado.
int assignblock(int bloque){
    int offset=bloque/8;
    int shift=bloque%8;
    int sector;
    int secl_blockmap;
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos de bloques esté en memoria
    load_sec_mapa_bloques();
    
    mapa_bits_bloques[offset]|=(1<<shift);
    
    secl_blockmap = get_secl_mapa_bloques();
    sector = offset/SECTOR_SIZE;
    return vdwritesl(secl_blockmap+sector, 
    				mapa_bits_bloques+sector*SECTOR_SIZE);
}

//Función que asigna un bloque como libre.
int unassignblock(int block){
    int offset=block/8;
    int shift=block%8;
    int sector;
    int secl_blockmap;
    //Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos de bloques esté en memoria
    load_sec_mapa_bloques();
    
    mapa_bits_bloques[offset]&=(char) ~(1<<shift);
    
    secl_blockmap = get_secl_mapa_bloques();
    sector = offset/SECTOR_SIZE;
    return vdwritesl(secl_blockmap+sector,
    				mapa_bits_bloques+sector*SECTOR_SIZE);
}

/*************************************************************
 * Lectura y escritura de bloques
 * ***********************************************************/
//Escribe en el bloque indicado lo que hay en buffer
int writeblock(int bloque, char *buffer){
    int inicio_area_datos;
    int sector_inicio;
    load_sec_boot();
    inicio_area_datos = get_secl_data();
    sector_inicio = inicio_area_datos + ((bloque-1) * secboot.sec_x_bloque);
    //DEBUG("Empieza escritura de bloque %d en sl %d\n", bloque, sector_inicio);
    for(int i=0;i<secboot.sec_x_bloque;i++)
		vdwritesl(sector_inicio+i, buffer+512*i);
	return 1;
}

//Lee del bloque y escribe el contenido en buffer
int readblock(int bloque, char *buffer){
    int inicio_area_datos;
    int sector_inicio;    
    load_sec_boot();
    inicio_area_datos = get_secl_data();
    sector_inicio = inicio_area_datos + (bloque-1) * secboot.sec_x_bloque;
    //DEBUG("Empieza lectura de bloque %d en sl %d\n", bloque, sector_inicio);
    for(int i=0;i<secboot.sec_x_bloque;i++)
		vdreadsl(inicio_area_datos+(bloque-1)*secboot.sec_x_bloque+i,
				buffer+512*i);
   	return 1;
}


/*************************************************************
 *                  Manejo de fechas
 * ***********************************************************/

 //Regresa el valor de la fecha en entero
unsigned int datetoint(struct DATE date){
	unsigned int val=0;

	val=date.year-1970;
	val<<=4;
	val|=date.month;
	val<<=5;
	val|=date.day;
	val<<=5;
	val|=date.hour;
	val<<=6;
	val|=date.min;
	val<<=6;
	val|=date.sec;
	
	return(val);
}

//Convierte un entero a formato fecha. Lo guarda en 'date'
int inttodate(struct DATE *date,unsigned int val){
	date->sec=val&0x3F;
	val>>=6;
	date->min=val&0x3F;
	val>>=6;
	date->hour=val&0x1F;
	val>>=5;
	date->day=val&0x1F;
	val>>=5;
	date->month=val&0x0F;
	val>>=4;
	date->year=(val&0x3F) + 1970;
	return(1);
}

//Regresa el entero de la fecha actual
unsigned int currdatetimetoint(){
	struct tm *tm_ptr;
	time_t the_time;
	
	struct DATE now;

	(void) time(&the_time);
	tm_ptr=gmtime(&the_time);

	now.year=tm_ptr->tm_year-70;
	now.month=tm_ptr->tm_mon+1;
	now.day=tm_ptr->tm_mday;
	now.hour=tm_ptr->tm_hour;
	now.min=tm_ptr->tm_min;
	now.sec=tm_ptr->tm_sec;
	return(datetoint(now));
}

//Guardar información del nodoi n y asignarlo en el mapa de nodos i
int setninode(int num, char *filename, unsigned short atribs, 
			  int uid, int gid){

	if(num<0||num>63)
		return -1;

	int i;
	//Checar si el sector del superbloque está en secboot_en_memoria
    load_sec_boot();
    //Checar que mapa de nodos i esté en memoria
    load_inodes();

    strncpy (inodes[num].name, filename, 20);

    if(strlen(inodes[num].name) > 19)
    	inodes[num].name[19]='\0';

    inodes[num].datetimecreat=currdatetimetoint();
	inodes[num].datetimemodif=currdatetimetoint();
	inodes[num].uid=uid;
	inodes[num].gid=gid;
	inodes[num].perms=atribs;
	inodes[num].size=0;
	
	for(i=0;i<10;i++)
		inodes[num].blocks[i]=0;

	inodes[num].indirect1=0;
	inodes[num].indirect2=0;

	i = (int) num/8;
	DEBUG("Saving '%s' on inode %d i = %d ptr = %d ", inodes[num].name, num, get_secl_tabla_nodos_i()+i,  &(inodes[i*8+8]) );
	assigninode(num);
	vdwritesl(get_secl_tabla_nodos_i()+i, (char *) &(inodes[i*8]));
	
	return num;
}

//Búsqueda de un archivo por nombre en la tabla de nodos i
int searchinode(char *filename){
	int i;

	//Checar si el sector del superbloque está en secboot_en_memoria
	load_sec_boot();
	//Checar que mapa de nodos i esté en memoria
    load_inodes();

    if(strlen(filename) > 19)
    	filename[19] = '\0';

    i = 0;
    while(strcmp(inodes[i].name, filename) && i < INODE_SIZE){
    	i++;
    }

    if(i>=64){
    	DEBUG("Archivo '%s' no encontrado\n", filename);
    	return -1;
    }
    DEBUG("Archivo '%s' encontrado en nodo i %d\n", inodes[i].name, i);
    return i;
}

//Eliminar un nodo i del mapa de nodos
int removeinode(int numinode){
	int i;
	unsigned short temp[2048];

	//Recorrer apuntadores directos y desasignar bloques
	for(i=0; i<10; i++)
		if(inodes[numinode].blocks[i]!=0)
			unassignblock(inodes[numinode].blocks[i]);
	//Recorrer apuntadores del indirecto si es que se usa
	if(inodes[numinode].indirect1 != 0){
		readblock(inodes[numinode].indirect1, (char *) temp);

	for(i=0; i<1024; i++){
		if(temp[i]!=0){
			DEBUG("Unassign %d\n",temp[i]);
			unassignblock(temp[i]);
		}
	}
		unassignblock(inodes[numinode].indirect1);
		inodes[numinode].indirect1 = 0;
	}

	//Desasignar nodo i
	unassigninode(numinode);
	DEBUG("Archivo eliminado\n");
	return 1;
}