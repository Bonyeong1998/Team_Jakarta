#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
int search_directory(char * path) {
    int i = 0;
    struct dirent *pde;
    DIR * pdir;
    char child_node[256] ="";
    if((pdir = opendir(path))<0) {
        perror("opendir");
        exit(1);
    }
    while ((pde = readdir(pdir)) != NULL) {
        printf("%20s ", pde->d_name);
        if (++i % 3 == 0)
        printf("\n");
        if (pde->d_name[0] == '.')
        continue;
        if(pde->d_type == DT_DIR) {
        strcpy(child_node,"");
        strcpy(child_node,path);
        strcat(child_node,"/");
        strcat(child_node,pde->d_name);
        search_directory(child_node);
        }
    }
    closedir(pdir);
}
int main(int argc, char * argv[]) {
    DIR *pdir;
    struct dirent *pde;
    if(argc<2) {
        fprintf(stderr, "Usage: file_dir dirname\n");
        exit(1);
    }
    search_directory(argv[1]);
    printf("\n");
}

