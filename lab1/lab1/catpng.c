#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>   /* for printf().  man 3 printf */
#include <stdlib.h>  /* for exit().    man 3 exit   */
#include <string.h>  /* for strcat().  man strcat   */
#include "lab_png.h"
#include "zutil.h"
#include "crc.h"
#define max_file_in_dir 1000
#define max_search_depth 1000

U8* header(FILE* png_n){
    U8* buffer = malloc(sizeof(char)*8);
    fseek (png_n,0 , SEEK_SET);
    fread(buffer, sizeof(buffer), 1, png_n);
    return buffer;
}


U8* ReadIHDR(FILE* png_n){
    U8* buffer = malloc(sizeof(char)*25);
    fseek(png_n, 8, SEEK_SET);
    fread(buffer, sizeof(char)*25, 1, png_n);
    
    return buffer;
}


data_IHDR_p WrongDiIHDR(FILE* png_n){
    fseek(png_n, 17, SEEK_SET);
    data_IHDR_p png_IHDR_data;
    U8* buffer = malloc(sizeof(char)*25);
    fread(buffer, 1, sizeof(buffer), png_n);
    png_IHDR_data = (data_IHDR_p)buffer;
    return png_IHDR_data;
}

U32 IHDRHieght(FILE* png_n){
    U8* buffer = malloc(sizeof(char)*4);
    fseek(png_n, 20, SEEK_SET);
    fread(buffer, sizeof(char)*4, 1, png_n);
//    for (int i=0; i<4; ++i){
//        printf("%x ", buffer[i]);
//    }
    U32 height = buffer[0] << 24 | buffer[1] << 16 | buffer[2] <<8 | buffer[3];
    printf("%d\n", height);
    return height;
}

U32 IDATLength(FILE* png_n){
    U8* buffer = malloc(sizeof(char)*4);
    fseek(png_n, 33, SEEK_SET);
    fread(buffer, sizeof(buffer), 1, png_n);
    
    return buffer[0] << 24 | buffer[1] << 16 | buffer[2] <<8 | buffer[3];
}

U8* IDATDataFieldUncom(FILE* png_n, U64 *len_inf){
    U8* tmp_buffer;
    U64 len_def = 0;
    *len_inf = 0;
    U8* gp_buf_inf; /* output buffer for mem_inf() */
    
    size_t result;
    int ret = 0;
    int i = 0;
    
    
    
    fseek(png_n, 33, SEEK_SET);
    tmp_buffer = malloc(sizeof(char)*(4));
    fread(tmp_buffer, sizeof(tmp_buffer), 1, png_n);
    len_def = tmp_buffer[0] << 24 | tmp_buffer[1] << 16 | tmp_buffer[2] <<8 | tmp_buffer[3];
    len_def = (U32)len_def;
    fseek(png_n, 41, SEEK_SET);
    free(tmp_buffer);
    tmp_buffer = malloc(sizeof(char)*(len_def));
    result = fread(tmp_buffer,len_def, 1 , png_n);
    gp_buf_inf = malloc(sizeof(char)*10000000);
    ret = mem_inf(gp_buf_inf, len_inf, tmp_buffer, len_def);
    if (ret == 0) { /* success */
        printf("compressed len = %ld, len_inf = %lu\n",
               len_def, *len_inf);
    } else { /* failure */
        fprintf(stderr,"mem_def failed. ret = %d.\n", ret);
    }
    return gp_buf_inf;
}




U8* CRCInput(U8* type, U8*p_data, U32 p_data_len){
    int i =0;
    U8* buffer = malloc(sizeof(char)*(p_data_len + sizeof(char)*4));
//    memcpy(buffer, type, 4);
    for (i = 0; i<4; ++i){
        buffer[i] = *(type+i);
    }
    
    for (i = 0; i<p_data_len; ++i){
        buffer[i+4] = *(p_data+i);
    }
    return buffer;
}



int main(int argc, char *argv[])
{
    data_IHDR_p wrong_di_IHDRbuf;
    U8* header_buf;
    U8* IHDR_data_buf_list[argc-1];
    U32 height;
    U32 IDAT_length;
    U32 IDAT_length_list[argc-1];
    U8* tmp_buffer;
    U64 len_def = 0;
    int i=0;
    simple_PNG_p PNG_buf;
    
    
    
    int ret = 0;
    U64 len_inf = 0;      /* uncompressed data length                   */

    tmp_buffer = malloc (sizeof(char)*25);
    
    
    if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }
    
    

    
//Initialization
    FILE * png_0 = fopen(argv[1], "rb");
    wrong_di_IHDRbuf = malloc(sizeof(char)*13);
    PNG_buf = malloc(sizeof(PNG_buf));
    PNG_buf->p_IHDR = malloc(sizeof(PNG_buf->p_IHDR));
    PNG_buf->p_IDAT = malloc(sizeof(PNG_buf->p_IDAT));
    PNG_buf->p_IEND = malloc(sizeof(PNG_buf->p_IEND));
    header_buf = header(png_0);
    
    


    
//    Setup IHDR and leave crc empty;
    PNG_buf->p_IHDR = malloc(sizeof(char)*13);
    tmp_buffer = ReadIHDR(png_0);
    fseek(png_0, 8, SEEK_SET);
    fread(PNG_buf->p_IHDR, sizeof(char)*8, 1, png_0);
    fread(wrong_di_IHDRbuf, sizeof(char)*13, 1, png_0);
    PNG_buf->p_IHDR->p_data = malloc(sizeof(char)*13);
    memcpy(PNG_buf->p_IHDR->p_data, wrong_di_IHDRbuf, sizeof(char)*13);
    fread(&PNG_buf->p_IHDR->crc, sizeof(char)*4, 1, png_0);
    
    
//Setup length and type of IDAT
    fseek(png_0, 33, SEEK_SET);
    fread(PNG_buf->p_IDAT, sizeof(char)*8, 1, png_0);
    

    
    
    height = 0;
    IDAT_length = 0;
    fclose(png_0);
    
//New data field in IDAT
    for (i=0; i<argc-1; ++i){
        FILE * png_n = fopen(argv[i+1], "rb");
        IHDR_data_buf_list[i] = IDATDataFieldUncom(png_n, &len_inf);
        IDAT_length +=len_inf;
        IDAT_length_list[i] = len_inf;
        height+=IHDRHieght(png_0);
        fclose(png_n);
    }

//set crc and dimension value in IHDR
    wrong_di_IHDRbuf->height = htonl(height);
    memcpy(PNG_buf->p_IHDR->p_data, wrong_di_IHDRbuf, sizeof(char)*13);
    
    U8* tmp_crc_buffer = malloc(sizeof(char)*17);
    memcpy(tmp_crc_buffer, PNG_buf->p_IHDR->type, sizeof(char)*4);
    memcpy(tmp_crc_buffer+4, PNG_buf->p_IHDR->p_data, sizeof(char)*13);
    for (i=0; i<17; ++i){
        printf("%x ", tmp_crc_buffer[i]);
    }
    
    PNG_buf->p_IHDR->crc = htonl(crc(tmp_crc_buffer, sizeof(char)*17));
    

//Compress new data

    U8* gp_buf_def = malloc(sizeof(char)*1000000);
    U8* tmp_IDATdata_buffer;
    tmp_IDATdata_buffer = malloc(sizeof(char)*IDAT_length);
    memcpy(tmp_IDATdata_buffer, IHDR_data_buf_list[0], sizeof(char)*IDAT_length_list[0]);
    for (i = 1; i<argc-1; ++i){
        memcpy(tmp_IDATdata_buffer+IDAT_length_list[i], IHDR_data_buf_list[i], sizeof(char)*IDAT_length_list[i]);
    }
     ret = mem_def(gp_buf_def, &len_def, tmp_IDATdata_buffer, IDAT_length, Z_DEFAULT_COMPRESSION);
    
//set IDAT data and length field
    PNG_buf->p_IDAT->p_data = gp_buf_def;
    PNG_buf->p_IDAT->length = htonl(len_def);
    
    
    tmp_crc_buffer = malloc(sizeof(char)*(4+len_def));
    memcpy(tmp_crc_buffer, PNG_buf->p_IDAT->type, sizeof(char)*4);
    memcpy(tmp_crc_buffer+4, PNG_buf->p_IDAT->p_data, sizeof(char)*len_def);
    PNG_buf->p_IDAT->crc = htonl(crc(tmp_crc_buffer, (unsigned long)sizeof(char)*(4+len_def)));
    
    
    FILE * fp;
    fp= fopen( "/Users/wangqiong/Desktop/images/finally!.png" , "wb" );
    
    
//write file
    for (i = 0; i<8; ++i){
        printf("%x ", header_buf[i]);
    }
    printf("\n");
    
    fwrite(header_buf, sizeof(char)*8, 1, fp);
    
    
    //setup IEND
    png_0 = fopen(argv[1], "rb");
    fseek(png_0, -12, SEEK_END);
    fread(PNG_buf->p_IEND, sizeof(char)*8, 1, png_0);
    fseek(png_0, -4, SEEK_END);
    fread(&(PNG_buf->p_IEND->crc), sizeof(char)*4, 1, png_0);
    fclose(png_0);
    
    
    fwrite(PNG_buf->p_IHDR, sizeof(char)*8 , 1 , fp );
    fwrite(PNG_buf->p_IHDR->p_data, sizeof(char)*13 , 1 , fp );
    fwrite(&PNG_buf->p_IHDR->crc, sizeof(char)*4 , 1 , fp );
    fwrite(PNG_buf->p_IDAT, sizeof(char)*8 , 1 , fp );
    fwrite(PNG_buf->p_IDAT->p_data, sizeof(char)*len_def, 1 , fp );
    fwrite(&PNG_buf->p_IDAT->crc, sizeof(char)*4 , 1 , fp );
    fwrite(PNG_buf->p_IEND, sizeof(char)*8 , 1 , fp );
    fwrite(&PNG_buf->p_IEND->crc, sizeof(char)*4 , 1 , fp );
//
    fclose(fp);
    
    
   
    return 0;
}



