                                                                                

// ******************************************************************************
// Para el mapa de bits del área de de datos
// ******************************************************************************

int isblockfree(int block)
{
    int offset=block/8;
    int shift=block%8;
    int result;
    int i;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }

    mapa_bits_bloques= secboot.sec_res+secboot.sec_mapa_bits_nodos_i;

    if(!blocksmap_en_memoria)
    {
        for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
            result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
        blocksmap_en_memoria=1;
    }

    if(blocksmap[offset] & (1<<shift))
        return(0);
    else
        return(1);
}   

int nextfreeblock()
{
    int i,j;
    int result;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }

    mapa_bits_bloques = secboot.sec_res+secboot.sec_mapa_bits_nodos_i;

    if(!blocksmap_en_memoria)
    {
        for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
            result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
        blocksmap_en_memoria=1;
    } 

    i=0;
    while(blocksmap[i]==0xFF && i<secboot.sec_mapa_bits_bloques*512)
        i++;

    if(i<secboot.sec_mapa_bits_bloques*512)
    {
        j=0;
        while(blocksmap[i] & (1<<j) && j<8)
            j++;

        return(i*8+j);
    }
    else
        return(-1);

        
}

int assignblock(int block)
{
    int offset=block/8;
    int shift=block%8;
    int result;
    int i;
    int sector;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }

    mapa_bits_bloques= secboot.sec_res+secboot.sec_mapa_bits_nodos_i;

    if(!blocksmap_en_memoria)
    {
        for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
            result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
        blocksmap_en_memoria=1;
    } 

    blocksmap[offset]|=(1<<shift);

    sector=offset/512;
    vdwriteseclog(mapa_bits_bloques+sector,blocksmap+sector*512);
    //for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
    //  vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
    return(1);
}

int unassignblock(int block)
{
    int offset=block/8;
    int shift=block%8;
    int result;
    char mask;
    int sector;
    int i;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }

    mapa_bits_bloques= secboot.sec_res+secboot.sec_mapa_bits_nodos_i;

    if(!blocksmap_en_memoria)
    {
        for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
            result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
        blocksmap_en_memoria=1;
    }

    blocksmap[offset]&=(char) ~(1<<shift);

    sector=offset/512;
    vdwriteseclog(mapa_bits_bloques+sector,blocksmap+sector*512);
    // for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
    //  vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
    return(1);
}


// *************************************************************************
// Para el mapa de bits del área de nodos i
// *************************************************************************

int isinodefree(int inode)
{
    int offset=inode/8;
    int shift=inode%8;
    int result;

    // Checar si el sector del superbloque está en memoria
    if(!secboot_en_memoria)
    {
        // Si no está en memoria, cárgalo
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }
    mapa_bits_nodos_i= secboot.sec_res;     //Usamos la información del superbloque para 
                        //determinar en que sector inicia el 
                        // mapa de bits de nodos i 
                    
    // Ese mapa está en memoria
    if(!inodesmap_en_memoria)
    {
        // Si no está en memoria, hay que leerlo del disco
        result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
        inodesmap_en_memoria=1;
    }


    if(inodesmap[offset] & (1<<shift))
        return(0);
    else
        return(1);
}   

int nextfreeinode()
{
    int i,j;
    int result;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }
    mapa_bits_nodos_i= secboot.sec_res;

    if(!inodesmap_en_memoria)
    {
        result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
        inodesmap_en_memoria=1;
    }

    // Recorrer byte por byte mientras sea 0xFF sigo recorriendo
    i=0;
    while(inodesmap[i]==0xFF && i<secboot.sec_mapa_bits_nodos_i*512)
        i++;

    if(i<secboot.sec_mapa_bits_nodos_i*512)
    {
        j=0;
        while(inodesmap[i] & (1<<j) && j<8)
            j++;

        return(i*8+j);
    }
    else
        return(-1);

        
}

int assigninode(int inode)
{
    int offset=inode/8;
    int shift=inode%8;
    int result;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }

    mapa_bits_nodos_i= secboot.sec_res;

    if(!inodesmap_en_memoria)
    {
        result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
        inodesmap_en_memoria=1;
    }

    inodesmap[offset]|=(1<<shift);
    vdwriteseclog(mapa_bits_nodos_i,inodesmap);
    return(1);
}

int unassigninode(int inode)
{
    int offset=inode/8;
    int shift=inode%8;
    int result;
    char mask;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }

    mapa_bits_nodos_i= secboot.sec_res;

    if(!inodesmap_en_memoria)
    {
        result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
        inodesmap_en_memoria=1;
    }


    inodesmap[offset]&=(char) ~(1<<shift);
    vdwriteseclog(mapa_bits_nodos_i,inodesmap);
    return(1);
}   

//*******************************************************************************
// Lectura y escritura de bloques
// ******************************************************************************

int writeblock(int block,char *buffer)
{
    int result;
    int i;
    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }

    inicio_area_datos=secboot.sec_res+secboot.sec_mapa_bits_nodos_i +secboot.sec_mapa_bits_bloques+secboot.sec_tabla_nodos_i;


    for(i=0;i<secboot.sec_x_bloque;i++)
        vdwriteseclog(inicio_area_datos+(block-1)*secboot.sec_x_bloque+i,buffer+512*i);
    return(1);  
}

int readblock(int block,char *buffer)
{
    int result;
    int i;

    if(!secboot_en_memoria)
    {
        result=vdreadsector(0,0,0,1,1,(char *) &secboot);
        secboot_en_memoria=1;
    }
    inicio_area_datos=secboot.sec_res+secboot.sec_mapa_bits_nodos_i+secboot.sec_mapa_bits_bloques+secboot.sec_tabla_nodos_i;

    for(i=0;i<secboot.sec_x_bloque;i++)
        vdreadseclog(inicio_area_datos+(block-1)*secboot.sec_x_bloque+i,buffer+512*i);
    return(1);  
}
// ******************************************************************************
// Funciones para el manejo de fechas en los inodos
// ******************************************************************************



unsigned int datetoint(struct DATE date)
{
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

int inttodate(struct DATE *date,unsigned int val)
{
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

unsigned int currdatetimetoint()
{
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