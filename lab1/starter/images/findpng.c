#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>   /* for printf().  man 3 printf */
#include <stdlib.h>  /* for exit().    man 3 exit   */
#include <string.h>  /* for strcat().  man strcat   */

int validate_png(unsigned char arr[]);

int main(int argc, char *argv[]) 
{

	DIR *p_dir;
    struct dirent *p_dirent;
    char str[64];
    char *ptr;
    struct stat buf;

    /* no dir specified */
    if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }

    /* if dir not accessed */
    if ((p_dir = opendir(argv[1])) == NULL) {
        sprintf(str, "opendir(%s)", argv[1]);
        perror(str);
        exit(2);
    }

    /* loop entries in dir */
    while ((p_dirent = readdir(p_dir)) != NULL) {

        char *str_name = p_dirent->d_name;
        FILE *f = fopen(p_dirent->d_name, "rb");

        if (lstat(p_dirent->d_name, &buf) < 0) {
            perror("lstat error");
            continue;
        }

        /* if regular file */
        else if (S_ISREG(buf.st_mode)) {

            unsigned char arr[7];
            fread(arr,8,1,f);

            if (validate_png(arr) == 0){
                printf("%s\n", str_name);
            }
        }

        else if (S_ISDIR(buf.st_mode)) {
            printf("dir\n");
        }
        else {
            printf("unknown file type \n");
            return -1;
        }

    }

    /*

    open dir 

    for each file in pdir
    	if regular
    		validate png
    	if dir
    		chdir(./dir)
    		recursive call

	while loop to print p_dir

	close dir

	*/

	return 0;
}

   int validate_png(unsigned char arr[]) {

        unsigned char png_header[] = {137, 80, 78, 71, 13, 10, 26, 10};

        for (int i=0 ; i< sizeof(*arr) / sizeof(arr[0]); i++) {
                if (arr[i] != png_header[i]) {
                    return -1;
                }
            }

        return 0;

    }









