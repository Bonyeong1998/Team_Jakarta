#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <setjmp.h>

#define BUFSIZE 256
sigjmp_buf jbuf;
pid_t pid;

void handler(int narg, char **argv);
int getargs(char *cmd, char **argv);
void launch(int narg, char **argv);
void redirection(int narg, char **argv);
void ls(int narg, char **argv);
void cd(int narg, char **argv);
void my_rmdir(int narg, char **argv);
void cp(int narg, char **argv);
void mv(int narg, char **argv);
int getargs(char *cmd, char **argv);
void SIGINT_Handler(int signo,pid_t pid);
void SIGQUIT_Handler(int signo);
void pipe_launch(int narg, char **argv);
char *substring(int start, int end, char * str);
void SIGQUIT_Handler(int signo) {
    printf("\n");
    exit(1);
}

void SIGINT_Handler(int signo,pid_t pid)
{   
    if (kill(pid, SIGTERM) != 0) {
        printf("\n");
    }



}


int main()
{
    signal(SIGINT, SIGINT_Handler);
    signal(SIGTSTP, SIGQUIT_Handler);
    char buf[256];
    int narg;
    int i = 0;
    while (1)
    {

        char *argv[50] = {'\0'} ;
        printf("shell> ");
	    gets(buf);
        //  Q1. exit 입력 시 프로그램 종료.
        if(!strcmp(buf,"exit") || !strcmp(buf,"exit;"))
            break;
        
        else if(!strcmp(buf,"") || !strcmp(buf,"\t")) 
            continue;
        

        narg = getargs(buf, argv);
        handler(narg, argv);
    }

}

void handler(int narg, char **argv) {
    
    int i = 0;
    int is_background = 0, is_redirection = 0, is_pipe = 0;
    for(i = 0; i < narg; i++) {
        if( (!strcmp(argv[i], ">")) || (!strcmp(argv[i], "<"))) {
            is_redirection = 1;
            break;
        }
        else if (!strcmp(argv[i], "|")) {
            is_pipe = 1;
            break;
        }
        else if (!strcmp(argv[i], "&")) {
            is_background = 1;
            break;
        }
    }    

    if(is_background){
        launch(narg, argv);
        is_background = 0;
    }
    else if(is_redirection){
        redirection(narg, argv);
        is_redirection = 0;
    }
    else if(is_pipe){
        pipe_launch(narg, argv);
        is_pipe = 0;
    }
    else if(!strcmp(argv[0], "ls")){
        ls(narg, argv);
    }
    else if(!strcmp(argv[0], "cd")){
        cd(narg, argv);
    }
    else if (!strcmp(argv[0], "rmdir")){
        my_rmdir(narg, argv);
    }
    else if (!strcmp(argv[0], "cp")){
        cp(narg, argv);
    }
    else if (!strcmp(argv[0], "mv")){
        mv(narg, argv);
    }
    else if (!strcmp(argv[0], "pwd")){
        pwd(narg, argv);
    }
    else if (!strcmp(argv[0], "rm")){
        rmdir_and_rm(narg, argv);
    }
    else if (!strcmp(argv[0], "mkdir")){
        my_mkdir(narg, argv);
    }
    else if (!strcmp(argv[0], "ln")){
        my_ln(narg, argv);
    }
    else if (!strcmp(argv[0], "cat")){
        cat(narg, argv);
    }
    else {
        launch(narg,argv);
    }

}

int getargs(char *cmd, char **argv)
{
    int narg = 0;
    while (*cmd)
    {
        if (*cmd == ' ' || *cmd == '\t')
            *cmd++ = '\0';
        else
        {
            argv[narg++] = cmd++;
            while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t')
                cmd++;
        }
    }
    argv[narg] = NULL;
    return narg;
}

void pipe_launch(int narg, char **argv) {
    int command_pos = 0;
    int count_pipe = 0;
    int i  = 0 ;
    int pd_idx = 0;
    int k = 0;
    int status;

    while(argv[command_pos] != NULL) {
        if(argv[command_pos][0] =='|')
            count_pipe ++;
        command_pos++;
    }

    int fd[count_pipe*2];
    for(i = 0; i < (count_pipe); i++){
        if(pipe(fd + i*2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    command_pos = 0;
    for( i = 0 ; i<=count_pipe; i++) {
        int j =0;
        char **command = (char **)malloc(narg * sizeof(char *));

        if(!strcmp(argv[command_pos], "|"))
            command_pos++;

        if(i <count_pipe) {
        while(strcmp(argv[command_pos], "|")  ) {
            command[j] = (char *)malloc(100 * sizeof(char));
            strcpy(command[j],argv[command_pos]);
            command_pos++;
            j++;
            }
        }
        
         else {
            while(argv[command_pos] != NULL) {
                command[j] = (char *)malloc(100 * sizeof(char));
                strcpy(command[j],argv[command_pos]);
                command_pos++;
                j++;

            }
        }

        command[j] = NULL;

        pid = fork();
        if(pid == 0) {
            if(i<count_pipe) {
            if(dup2(fd[pd_idx + 1], 1) < 0){
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            }

            if(pd_idx != 0 ){
                if(dup2(fd[pd_idx-2], 0) < 0){
                    perror(" dup2");
                    exit(EXIT_FAILURE);

                }
            }
            for(k = 0; k < 2*count_pipe; k++){
                close(fd[k]);
            }

            if( execvp(command[0], command) < 0 ){
                    perror(command);
                    exit(EXIT_FAILURE);
            }
        }
        pd_idx+=2;

        k=0;
        while(command[k] != NULL) {
            free(command[k]);
            k++;
        }
        free(command);
    }

    for(k = 0; k < 2 * count_pipe; k++){
        close(fd[k]);
    }

    for(k = 0; k < count_pipe + 1; k++)
        wait(&status);
    
}

void launch(int narg, char **argv)
{
    pid = 0;
    int i = 0;
    int is_background = 0;
    if(narg != 0 && !strcmp(argv[narg -1], "&") ){
        argv[narg-1] = NULL;
        is_background = 1;
    }
    pid = fork();
    if (pid == 0){
        if(is_background){
            printf("\nCREATE BACKGROUND PROCESS PID: %ld\n", (long)getpid());
        }

        if(execvp(argv[0], argv) < 0) {
            perror("[ERROR] CREATE BACKGROUND: ");
        }
    }
    else {
        if(is_background == 0){
            wait(pid);
        }
    }
}

char *substring(int start, int end, char * str) {
    char *new = (char *)malloc(sizeof(char)*(end-start+2));
    strncpy(new,str+start,end-start+1);
    new[end-start+1] = 0;
    return new;
}

void redirection(int narg, char **argv) {
    pid;
    int i = 0;
    int fd;
    int split_index = 0, is_write = 0;

    int write_flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    char *cmd[10] = {'\0'}; // redirection을 수행할 명령어 저장.


    for(i = 0; i < narg; i++){
        if(!strcmp(argv[i], ">")){
            split_index = i;
            is_write = 1;
        }
        else if(!strcmp(argv[i], "<")){
            split_index = i;
            is_write = 0;
        }
    }

    for(i = 0; i < split_index; i++){
        cmd[i] = argv[i];
    }

    pid = fork();
    if(pid == 0) {
        // > 연산자 
        if (is_write){
            if ((fd = open(argv[split_index + 1], write_flags, mode)) == -1) {
                perror("[ERROR] OPEN: ");
                exit(1);
            }
            if (dup2(fd, 1) == -1) {
                perror("[ERROR] DUP2: ");
                exit(1);
            }
        }
        // < 연산자
        else{
            if ((fd = open(argv[split_index + 1], O_RDONLY)) == -1) {
                perror("[ERROR] OPEN: ");
                exit(1);
            }
            if (dup2(fd, 0) == -1) {
                perror("[ERROR] DUP2: ");
                exit(1);
            }
        }
    
        if (close(fd) == -1) {
            perror("[ERROR] CLOSE: ");
            exit(1);
        }
        execvp(cmd[0], cmd);
    }

    else if (pid > 0) {
        wait(pid);
    }
}

void ls(int narg, char **argv){
    char temp[256];
    if(narg == 1) {
        getcwd(temp, 256);
        printf("%s", temp);
        argv[1] =  temp;

    }
    DIR *pdir;
    struct dirent *pde;
    int i = 0;
    if((pdir = opendir(argv[1])) < 0) {
        perror("[ERROR] OPENDIR: ");
    }
    printf("\n");
    while((pde = readdir(pdir)) != NULL) {
        printf("%-20s", pde->d_name);
        if(++i %3 == 0)
            printf("\n");
    }
    printf("\n");
    closedir(pdir);
}

void cd(int narg, char **argv) {
    if(narg == 1) {
       chdir("HOME");
    }
    else{
        if(chdir(argv[1]) == -1){
            printf("%s: No search file or directory\n", argv[1]);
        }
    }
}

void my_rmdir(int narg, char **argv){
    int i = 0;
    char temp[256];
    if(narg == 1){
        printf("rmdir: missing operand\n");
    }
    else{
        if (rmdir(argv[1]) == -1) {
            perror("rmdir");
        }
    }
}

void cp(int narg, char **argv) {
    int src_fd; 
    int dst_fd;
    char buf[256];
    ssize_t rcnt;
    ssize_t tot_cnt = 0;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 

    if (narg < 3){
        fprintf(stderr, "Usage: file_copy src_file dest_file\n");
        exit(1);
    }
    if ((src_fd = open(argv[1], O_RDONLY)) == -1){
        perror("[ERROR]SRC OPEN");
        exit(1);
    }

    if ((dst_fd = creat(argv[2], mode)) == -1){
        perror("[ERROR]DST OPEN");
        exit(1);
    }

    while ((rcnt = read(src_fd, buf, 256)) > 0){
        tot_cnt += write(dst_fd, buf, rcnt);
    }

    if (rcnt < 0){
        perror("[ERROR]READ");
        exit(1);
    }

    close(src_fd);
    close(dst_fd);
}

void mv(int narg, char **argv) {
    struct stat buf;
    char *target;
    char *src_file_name_only;
    if (narg < 3)
    {
        fprintf(stderr, "Usage: file_rename src target\n");
        exit(1);
    }
    // Check argv[1] (src) whether it has directory info or not.
    if (access(argv[1], F_OK) < 0)
    {
        fprintf(stderr, "%s not exists\n", argv[1]);
        exit(1);
    }
    else
    {
        char *slash = strrchr(argv[1], '/');
        src_file_name_only = argv[1];
        if (slash != NULL)
        { // argv[1] has directory info.
            src_file_name_only = slash + 1;
        }
    }
    // Make target into a file name if it is a directory
    target = (char *)calloc(strlen(argv[2]) + 1, sizeof(char));
    strcpy(target, argv[2]);
    if (access(argv[2], F_OK) == 0)
    {
        if (lstat(argv[2], &buf) < 0)
        {
            perror("lstat");
            exit(1);
        }
        else
        {
            if (S_ISDIR(buf.st_mode))
            {
                free(target);
                target = (char *)calloc(strlen(argv[1]) + strlen(argv[2]) + 2, sizeof(char));
                strcpy(target, argv[2]);
                strcat(target, "/");
                strcat(target, src_file_name_only);
            }
        }
    }
    printf("target = %s\n", target);
    if (rename(argv[1], target) < 0){
        perror("rename");
        exit(1);
    }
    free(target);
}

void pwd() {
    char *buf = (char *)malloc(sizeof(char)*(BUFSIZE));
    
    if(getcwd(buf,BUFSIZE) == NULL) {
        perror("[ERROR] PWD");
        exit(EXIT_FAILURE);
    }
    else
        printf("%s \n",buf);
    
    free(buf);
    
}

void rmdir_and_rm(int narg, char ** argv) {

    if(narg < 2)
        fprintf(stderr, "Path is not exists\n");
    else {
        if(remove(argv[1]) < 0) {
            perror("[ERROR] RM/RMDIR");
            exit(EXIT_FAILURE);
        }
    }
}

void my_mkdir(int narg, char ** argv) {
    umask(0);
    if(argv)

    if(narg < 2)
        fprintf(stderr, "PathName is not exists\n");
    else {
        if(mkdir(argv[1],umask(0)) < 0) {
            perror("[ERROR] MKDIR");
            exit(EXIT_FAILURE);
        }
    }
}

void my_ln(int narg, char ** argv) {
    char cmd;
    char *src;  
    char *target;
    if (narg < 2) {
        fprintf(stderr, "Usage: ln [u,s] ...\n");
        fprintf(stderr, " ln src target\n");
        fprintf(stderr, " ln u[nlink] filename\n");
        fprintf(stderr, " ln s[ymlink] src target\n");
        exit(1);
    }

    
    if (!strcmp(argv[1],"-s")) {
        if (narg < 4) {
            fprintf(stderr, "ln l src target [link]\n");
            exit(1);
        }
        src = argv[2];
        target = argv[3]; 
        if (symlink(src, target) < 0) {
            perror("symlink");
            exit(1);
            
        }
    }
    else if (!strcmp(argv[1],"-u")) {
        src = argv[2];
        if (unlink(src) < 0) {
            perror("unlink");
            exit(1);
        }
    }

    else if (narg == 3) {
        src = argv[1];
        target = argv[2];
        if (link(src, target) < 0) {
            perror("link");
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Unknown command...\n");
    }
} 

void cat(int narg, char ** argv) {
    FILE *file[narg-1];
    int loop;
    char buf;
    if (narg < 1) {
        fprintf(stderr, "Please Input Files \n");
        exit(1);
    }

    for(loop = 0; loop<narg-1; loop++) {
        file[loop] = fopen(argv[loop+1],"r");
        if(file[loop] == NULL) {
            printf("cat : %s : No such file or directory \n",argv[loop+1]);
        }
        else {
            while((buf = fgetc(file[loop])) != EOF) {
                printf("%c",buf);
            }
        
            if((fclose(file[loop])) != 0)
            {
                perror("CAT");
            }
        }

    }

    printf("\n");

}
