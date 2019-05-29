#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>   /* for printf().  man 3 printf */
#include <stdlib.h>  /* for exit().    man 3 exit   */
#include <string.h>  /* for strcat().  man strcat   */
#include "lab_png.h"
#include "zutil.h"
#define max_file_in_dir 1000
#define max_search_depth 1000


int main(int argc, char *argv[])
{
    long lSize;
    int ret = 0;
    size_t result;
    U8* buffer;
    U8* tmp_buffer;
    U64 len_inf = 0;      /* uncompressed data length                   */
    U8* gp_buf_inf; /* output buffer for mem_inf() */
    data_IHDR_p png_IHDR_data = malloc (sizeof(data_IHDR_p));
    U64 len_def = 0;
    tmp_buffer = malloc (sizeof(char)*25);
    
    
    if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }
    
//Openfile
    FILE * png_0 = fopen(argv[1], "rb");
    fseek (png_0 ,0 , SEEK_END);
    lSize = ftell(png_0);
    fseek (png_0 ,0 , SEEK_SET);
    if (png_0==NULL) {
        fputs ("File error",stderr);
        exit (1);
    }
    

    rewind (png_0);
    if (&buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
    buffer = malloc (sizeof(char)*lSize);
    result = fread (buffer,1,lSize,png_0);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
    
//save data info to data_IHDR struct
    fseek(png_0, 16, SEEK_SET);
    fread(tmp_buffer, 1, 25, png_0);
    png_IHDR_data = (data_IHDR_p)tmp_buffer;
    
    

    
    simple_PNG_p png_p = malloc(sizeof(simple_PNG_p));
    png_p->p_IHDR = malloc(sizeof(chunk_p));
    png_p->p_IDAT = malloc(sizeof(chunk_p));
    png_p->p_IHDR = malloc(sizeof(chunk_p));
    png_p->p_IHDR->p_data = buffer + 8;
    png_p->p_IDAT->p_data = buffer + 34;
    
//save IDAT datafield to tmp buffer
    fseek(png_0, 33, SEEK_SET);
    tmp_buffer = malloc(sizeof(char)*(4));
    fread(tmp_buffer, sizeof(tmp_buffer), 1, png_0);
    int i=0;
    for (i=0; i<4; ++i){
        printf("%x", tmp_buffer[i]);
    }
    len_def = tmp_buffer[0] << 24 | tmp_buffer[1] << 16 | tmp_buffer[2] <<8 | tmp_buffer[3];
    len_def = (U32)len_def;
    fseek(png_0, 41, SEEK_SET);
    free(tmp_buffer);
    tmp_buffer = malloc(sizeof(char)*(len_def));
    result = fread(tmp_buffer,len_def, 1 , png_0);
    gp_buf_inf = malloc(sizeof(char)*10000000);
    ret = mem_inf(gp_buf_inf, &len_inf, tmp_buffer, len_def);
    for (i=0; i<1; ++i){
        printf("%x", gp_buf_inf[i]);
    }
    if (ret == 0) { /* success */
        printf("compressed len = %ld, len_inf = %lu\n",
               lSize, len_inf);
    } else { /* failure */
        fprintf(stderr,"mem_def failed. ret = %d.\n", ret);
    }
    
    
    
//    while(i<lSize){
//        ret = mem_inf(gp_buf_inf, &len_inf, buffer+34, lSize);
//
//        if (ret == 0) { /* success */
//            printf("original len = %d, len_inf = %lu\n", \
//                   BUF_LEN, len_inf);
//        } else { /* failure */
//            fprintf(stderr,"mem_def failed. ret = %d.\n", ret);
//        }
//    }
    fclose (png_0);
    free (buffer);
    return 0;
    return 0;
}



