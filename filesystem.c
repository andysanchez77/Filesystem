#include "filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int disk_n = 0;
int secboot_en_memoria = 0;
int inodesmap_en_memoria = 0;
int secbloques_en_memoria = 0;
struct SECBOOT secboot;
unsigned char mapa_bits_nodosi [SECTOR_SIZE];
unsigned char mapa_bits_bloques [SECTOR_SIZE * 2];


int main(){
	printf("-----------MAIN DE PRUEBA----------\n");
	int i,j,k;
	struct DATE mydate = {2011, 9, 21,23,45,12};
	int intdate = datetoint(mydate);
	struct DATE mydatetoint;
	inttodate(&mydatetoint, intdate);
	printf("Hora Actual: %d\n", currdatetimetoint());
	printf("datetoint () --> %d\n", intdate);
	printf("inttodate () --> %d-%d-%d %d:%d:%d\n",mydatetoint.year, mydatetoint.month,
		        mydatetoint.day,mydatetoint.hour, mydatetoint.min, mydatetoint.sec);
	
	load_sec_boot();
	printf("Sector lógico mapa bits nodos i: %d\n",get_secl_mapa_nodos_i());
	printf("Sector lógico mapa bits bloques: %d\n",get_secl_mapa_bloques());
	printf("Sector lógico mapa tabla nodos i: %d\n",get_secl_tabla_nodos_i());
	printf("Sector lógico mapa área de datos: %d\n",get_secl_data());
	load_sec_mapa_bloques();
	load_sec_mapa_nodosi();
	
	if(mapa_bits_bloques[0] & (1) && inodesmap_en_memoria == 1){
		printf("\nTodo está en memoria!!!!\n\n");
//PARA PROBAR FUNCIONES DE NODOS I
		/*assigninode(0); assigninode(63); assigninode(60); assigninode(34); assigninode(15);
		printf("nodos i Libres --> [63]%d [62]%d [0]%d [29]%d [15]%d\n", isinodefree(63), isinodefree(62), isinodefree(0), isinodefree(29), isinodefree(15));
		unassigninode(63); unassigninode(0); unassigninode(15);
		printf("Próximo nodo libre: %d\n", nextfreeinode());
		for(i=0;i<=47;i++)
		     assigninode(i);
		printf("Próximo nodo libre: %d\n", nextfreeinode());
		for(;i<=63;i++)
		     assigninode(i);
		printf("Próximo nodo libre: %d\n", nextfreeinode());*/
//PARA PROBAR FUNCIONES DE BLOQUES
        //assignblock(2); assignblock(3); assignblock(345); assignblock(4000);assignblock(1234);
        //printf("bloques libres: [2]%d [4]%d [345]%d [4000]%d [1233]%d\n", isblockfree(2), isblockfree(4), isblockfree(345), isblockfree(4000), isblockfree(1233));
		//unassignblock(2); unassignblock(3); unassignblock(345); unassignblock(4000); unassignblock(1234);
		//printf("bloques libres: [2]%d [4]%d [345]%d [4000]%d [1233]%d\n", isblockfree(2), isblockfree(4), isblockfree(345), isblockfree(4000), isblockfree(1233));
        printf("primer bloque libre: %d\n", nextfreeblock());
        assignblock(1); assignblock(2);
        printf("primer bloque libre: %d\n", nextfreeblock());
        for(i=0;i<=4397;i++)
            assignblock(i);
        printf("primer bloque libre: %d\n", nextfreeblock());
		/*
		char buff [512]; memset(buff, 0xAA, 512);
		char buffBlock [512*4]; memset(buff, 0xEE, 512*4);
		vdwritesl(3,1, buff);
		
		char leer[512];
		printf ("leyendo %d\n", vdreadsl(3,1, leer));
		printf("%x\n", leer [1]);
		vdwritesl(6,1, leer);
		*/
	}


}


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
	/*
	int tmp_cyl = t.cyls - 1;
	// Calculamos el número de la superficie
	*head = (int) (secl / (t.cyls * t.secfis));
	secl -= *head * t.cyls * t.secfis;
	// Calculamos el número de cilindro
	*cyl = (int) (secl / t.secfis);
	secl -= *cyl * t.secfis;
	// Calculamos el número de sector físico
	*secf = secl + 1;//+ 1;
	*/
	*head = (secl / secboot.sec_x_bloque) % secboot.heads;
	*cyl = (secl / (secboot.sec_x_bloque * secboot.heads));
	*secf = (secl % secboot.sec_x_bloque) + 1;
	return 1;
}

//Lee y guarda en buffer la información del sector lógico sec_loc
int vdreadsl(int sec_loc, int nsectors, char *buffer) {
	int head, cyl, secf;
	if (calculateParams(sec_loc, &head, &cyl, &secf) == -1) {
		fprintf(stderr, "No fue posible calcular los parámetros de lectura física\n");
		return -1;
	}
	//printf("READ sup %d cil %d secf %d\n", head, cyl, secf);
	vdreadsector(disk_n, head, cyl, secf, nsectors, buffer);  

	return 1;  
}

//Escribe la información de buffer en el sector lógico sec_loc
int vdwritesl(int sec_loc, int nsectors, char *buffer) {
	int head, cyl, secf;
	if (calculateParams(sec_loc, &head, &cyl, &secf) == -1) {
		fprintf(stderr, "No fue posible calcular los parámetros de lectura física\n");
		return -1;
	}
	//printf("sup %d cil %d secf %d\n", head, cyl, secf);
	vdwritesector(disk_n, head, cyl, secf, nsectors, buffer);

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

//Carga en memoria el sector de mapa de bits de nodos i si es que no está cargado
void load_sec_mapa_nodosi(){
	if(!inodesmap_en_memoria){
		int seclnodosi = get_secl_mapa_nodos_i();
		int bhead, bcyl, bsecf;
		
		calculateParams(seclnodosi, &bhead, &bcyl, &bsecf);
		//printf("sl: %d sup %d cil %d secf %d\n", seclnodosi, bhead, bcyl, bsecf);
		vdreadsector(disk_n, bhead, bcyl, bsecf,
			        secboot.sec_mapa_bits_nodos_i, mapa_bits_nodosi);
		inodesmap_en_memoria = 1;
	}
}


//Carga en memoria el sector de mapa de bits de bloques si es que no está cargado
void load_sec_mapa_bloques(){
	if(!secbloques_en_memoria){
		int seclbloques = get_secl_mapa_bloques();
		int bhead, bcyl, bsecf;

		calculateParams(seclbloques, &bhead, &bcyl, &bsecf);
		//printf("sl: %d sup %d cil %d secf %d\n", seclbloques, bhead, bcyl, bsecf);
		vdreadsector(disk_n, bhead, bcyl, bsecf,
			        secboot.sec_mapa_bits_bloques, mapa_bits_bloques);
		secbloques_en_memoria = 1;
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
    printf("total %d\n", total_bytes);
    //total_bytes = secboot.sec_mapa_bits_nodos_i * SECTOR_SIZE;
    for(i=0; (mapa_bits_nodosi[i] == 0xFF && 
             i < total_bytes); i++);
    
    if(i < total_bytes){
        printf("i=%d total_bytes=%d\n",i,total_bytes);
        j = 0;
        while(mapa_bits_nodosi[i] & (1<<j) && (j<8))
            j++;
        return (i*8+j);
    }else
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
    return vdwritesl(secl_inodesmap,1, mapa_bits_nodosi);
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
    return vdwritesl(secl_inodesmap,1, mapa_bits_nodosi);
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
    valid_block = (secboot.sec_log_unidad -(secboot.sec_res + secboot.sec_mapa_bits_nodos_i + 
                  secboot.sec_mapa_bits_bloques + secboot.sec_tabla_nodos_i))/secboot.sec_x_bloque;
                  
    for(i=0; (mapa_bits_bloques[i] == 0xFF && i < total_bytes - 1); i++);
    
    if(i < total_bytes && i <= valid_block){
        printf("i=%d %x total_bytes=%d\n",i,mapa_bits_bloques[i], total_bytes);
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
    return vdwritesl(secl_blockmap+sector,1 , mapa_bits_bloques+sector*SECTOR_SIZE);
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
    return vdwritesl(secl_blockmap+sector,1 , mapa_bits_bloques+sector*SECTOR_SIZE);
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
    sector_inicio = inicio_area_datos + (bloque-1) * secboot.sec_x_bloque;
    
    //for(i=0;i<secboot.sec_x_bloque;i++)
	//	vdwritesl(inicio_area_datos+(block-1)*secboot.sec_x_bloque+i,buffer+512*i);
    return vdwritesl(sector_inicio, secboot.sec_x_bloque, buffer);
}

//Lee del bloque y escribe el contenido en buffer
int readblock(int bloque, char *buffer){
    int inicio_area_datos;
    int sector_inicio;
    
    load_sec_boot();
    inicio_area_datos = get_secl_data();
    sector_inicio = inicio_area_datos + (bloque-1) * secboot.sec_x_bloque;
    
    //for(i=0;i<secboot.sec_x_bloque;i++)
	//	vdreadsl(inicio_area_datos+(block-1)*secboot.sec_x_bloque+i,buffer+512*i);
    return vdreadsl(sector_inicio, secboot.sec_x_bloque, buffer);
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

