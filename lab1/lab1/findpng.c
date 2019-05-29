#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>   /* for printf().  man 3 printf */
#include <stdlib.h>  /* for exit().    man 3 exit   */
#include <string.h>  /* for strcat().  man strcat   */

#define max_file_in_dir 1000
#define max_search_depth 1000


int FindPNG(char* path, int depth, int found){
    DIR *p_dir;
    struct dirent *p_dirent;
    char str[64];
    char* str_list[max_file_in_dir];
    int dir_f_num = 0;
    
    
    
    if ((p_dir = opendir(path)) == NULL) {
        sprintf(str, "opendir(%s)", path);
        perror(str);
        exit(2);
    }
    
    int i = 0;
    while ((p_dirent = readdir(p_dir)) != NULL) {
        char *str_path = p_dirent->d_name;  /* relative path name! */
        
        if (str_path == NULL) {
            fprintf(stderr,"Null pointer found!");
            exit(3);
        } else {
            str_list[i] = str_path;
        }
        ++i;
    }
    dir_f_num = i;
    
    
    char *ptr;
    struct stat buf;
    
    for (i = 1; i < dir_f_num; i++) {
        unsigned long j = strlen(path);
        unsigned long k = strlen(str_list[i]);
        char* abs_path;
        abs_path =malloc((j+k+2)*sizeof(char)); //need to free
        //        strcpy(abs_path, argv[1]);
        int n = 0;
        for (n = 0; n<j+k+2; n++){
            if (n <j)   abs_path[n] = path[n];
            else if (n == j) abs_path[n] = '/';
            else if (n == j+k+1) abs_path[n] = '\0';
            else abs_path[n] = str_list[i][n-j-1];
        }
        if (lstat(abs_path, &buf) < 0) {
            perror("lstat error");
            continue;
        }
        
        if      (S_ISREG(buf.st_mode))  ptr = "regular";
        else if (S_ISDIR(buf.st_mode))  ptr = "directory";
        else if (S_ISCHR(buf.st_mode))  ptr = "character special";
        else if (S_ISBLK(buf.st_mode))  ptr = "block special";
        else if (S_ISFIFO(buf.st_mode)) ptr = "fifo";
#ifdef S_ISLNK
        else if (S_ISLNK(buf.st_mode))  ptr = "symbolic link";
#endif
#ifdef S_ISSOCK
        else if (S_ISSOCK(buf.st_mode)) ptr = "socket";
#endif
        else                            ptr = "**unknown mode**";
        
        if (strncmp(ptr, "regular", 7)==0){
            FILE *fileStream; char fileText [5];
            fileStream = fopen (abs_path, "r");
            fgets (fileText, 5, fileStream);
            char* header = malloc(4*sizeof(char));
            int tmp_i = 1;
            for (tmp_i = 1; tmp_i<4; ++tmp_i) header[tmp_i-1] = fileText[tmp_i];
            header[4] = '\0';
            if (strncmp(header, "PNG", 3) == 0){
                printf("%s\n", abs_path);
                if (found ==-1) found = 0;
            }
            free(header);
        }
        else if (strncmp(ptr, "directory", 9)==0){
            ++depth;
            if (depth <=max_search_depth && strncmp(str_list[i], ".", 1)!=0 && strncmp(str_list[i], "..", 1)!=0) found = FindPNG(abs_path, depth, found);
        }
        free(abs_path);
        
    }
    
    
    if ( closedir(p_dir) != 0 ) {
        perror("closedir");
        exit(3);
    }
    return found;
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }
    if (FindPNG(argv[1], 0, -1) == -1) printf("findpng: No PNG file found\n");
    return 0;
}


