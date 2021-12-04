#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE 1024
#define SHMKEY (key_t)0111
int getargs(char*, char**);
struct sigaction act;

 
int changeDirectory(char *args[]) //CD Function
{
	

	if (args[1] == NULL)
	{
		chdir(getenv("HOME"));
		return 1;
	}

	else
	{
		if (chdir(args[1]) == -1)
		{
			printf(" %s: 해당 디렉토리가 존재하지 않음.\n", args[1]);
			return -1;
		}
	}
	return 0;
}

int Signal_Interrupt(struct sigaction *def, sigset_t *mask, void(*handler)(int)) { //시그널 설정, ctrl + c / ctrl +z 시그널 설정
   struct sigaction catch;

   catch.sa_handler = handler;
   catch.sa_flags = 0;
   def->sa_handler = SIG_DFL;
   def->sa_flags = 0;



   if ((sigemptyset(&(def->sa_mask)) == -1) || (sigemptyset(&(catch.sa_mask)) == -1) || (sigaddset(&(catch.sa_mask), SIGINT) == -1) || (sigaddset(&(catch.sa_mask), SIGQUIT) == -1) || (sigaction(SIGINT, &catch, NULL) == -1) || (sigaction(SIGQUIT, &catch, NULL) == -1) || (sigemptyset(mask) == -1) || (sigaddset(mask, SIGINT) == -1) || (sigaddset(mask, SIGQUIT) == -1))


      return -1;


   return 0;


}

void sig_handler(int signo)
{//자식 프로세스를 종료 시 부모 프로세스도 같이 종료시킨다.

   pid_t pid ;
   int stat ;

   while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
      printf("child %d terminated normaly\n", pid) ;

}


int main() {
    int shmid, len, narg, status, fd;
    void *shmaddr;
    char buf[1024], *argv[50];
	char *get_current_dir_name();
    pid_t pid;
    DIR *pdir;
    struct dirent *pde;
    int i = 0;
    //ln
    char cmd, *src, *target, ch;
   
    FILE* fp;
    int cp_src, cp_dst, r_size, w_size;
   
    char* msg[2], dir_path[50], dir_name[50];
    int p[2];
    //dir
 
    act.sa_flags = SA_RESTART;
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_handler; //act.sa_handler에 sig_handler등록 
 
    if ((shmid = shmget(SHMKEY, SHMSIZE,IPC_CREAT|0666)) == -1) {
        perror ("shmget failed");
        exit (1);
    }
    if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror ("shmat failed");
        exit (1);
    }
    strcpy((char *)shmaddr, getcwd(buf, 1024));
	

  
    while(1) {
        printf( "[%s] $ ", get_current_dir_name() );  //  쉘 시작시현재 디렉터리 위치를 출력
        fgets(buf, sizeof(buf), stdin);  // 명령어 입력
        pid = fork();
        narg = getargs(buf, argv);
     
        if(pid == 0) {
            if(!strcmp(argv[0], "ls")) {  // "ls" 
                strcpy(dir_path, (char*)shmaddr); 
                pdir = opendir(dir_path);
    
                while ((pde = readdir(pdir)) != NULL) {
                    if(strncmp(".", pde->d_name,1) == 0 || strncmp("..",pde->d_name,2) == 0)
                        continue;
                    printf("%s  ", pde->d_name);
                }
                printf("\n");
                closedir(pdir);
            }
           
            else if(!strcmp(argv[0], "pwd")) { // "pwd"
        
                getcwd(buf, 1024);
                printf("%s\n", buf);
            }
          
            else if(!strcmp(argv[0], "mkdir")) { // "mkdir"
          
                strcpy(dir_name, argv[1]);
                strcpy(dir_path, getcwd(buf, 1024));
                strcat(dir_path, "/");
                strcat(dir_path, dir_name);
                if(mkdir(dir_path, 0755) > 0)
                    printf("디렉토리를 생성할 수 없습니다.");
            }
            else if(!strcmp(argv[0], "rmdir")) { //" rmdir" 
              
                strcpy(dir_name, argv[1]);
                strcpy(dir_path, getcwd(buf, 1024));
                strcat(dir_path, "/");
                strcat(dir_path, dir_name);
                if(rmdir(dir_path) > 0)
                    printf("디렉토리를 삭제할 수 없습니다.");
            }
            
            else if(!strcmp(argv[0], "ln")) { // "ln"
                
                cmd = (char) *argv[1];
                printf("cmd : %c\n", cmd);
                if (cmd == '|' ) {
                    if (narg < 4) {
                        fprintf(stderr, "file_link l src target [link]\n");
                        exit(1);
                    }
                    src = argv[2];
                    target = argv[3];
                    if (link(src, target) < 0) {
                        perror("link");
                        exit(1);
                    }
                }
                else if (cmd == 's') {
                    if (narg < 4) {
                        fprintf(stderr, "file_link l src target [link]\n"); 
                        exit(1);
                    }
                    src = argv[2];
                    target = argv[3];
                    if (symlink(src, target) < 0) {
                        perror("symlink");
                        exit(1);
                    }
                }
                else if (cmd == 'u') {
                    src = argv[2];
                    if (unlink(src) < 0) {
                        perror("unlink");
                        exit(1);
                    }
                } else {
                    fprintf(stderr, "Unknown command...\n");
                }
            }
            else if(!strcmp(argv[0], "cp")) { // "cp"
            
                cp_src = open(argv[1], O_RDONLY);
                cp_dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644); 
                // O_WRONLY : 쓰기 전용 모드로 파일을 연다, O_TRUNC : 존재하던 데이터를 모두 삭제한다. 
                while(read(cp_src, &ch, 1))
                    write(cp_dst, &ch, 1);
                close(cp_src);
                close(cp_dst);
            }
            else if(!strcmp(argv[0], "rm")) { // "rm"
                
                unlink(argv[1]);
            }
            else if(!strcmp(argv[0], "mv")) { // "mv"
              
                char file_name[50];
                strcpy(file_name, argv[1]);
                cp_src = open(argv[1], O_RDONLY);
                cp_dst = open(argv[2], O_RDWR|O_CREAT|O_EXCL, 0664);
                // O_RDWR : 읽기/쓰기 혼용, O_EXCL: 파일이 존재하는지 확인, O_CREAT : 파일이 없을 때 생성 모드, 세 번째 인자를 요구함

                r_size = read(cp_src,buf, 1024);
                w_size = write(cp_dst,buf,r_size);
                while(r_size == 1024) {
                    r_size = read(cp_src,buf,1024);
                    w_size = write(cp_dst,buf,r_size);
                }
                unlink(file_name);
                close(cp_src);
                close(cp_dst);
            }
            else if (strcmp(argv[0], "clear") == 0) // "clear"
		system("clear");
            else if(!strcmp(argv[0], "cat")) { // "cat"
                
                cp_src = open(argv[1], O_RDONLY);
                while(read(cp_src, &ch, 1))
                    write(1, &ch, 1);
                close(cp_src);
            }
            if(!strcmp(argv[narg-3], "<")) { // "<"
                printf("< 실행\n");
                strcpy(dir_name, argv[0]);
                for(i=0; argv[i] != NULL; i++) {
                    if(!strcmp(argv[i], "<")) {
                        break;
                    }
                }
                fp = fopen(argv[i+2], "r");
                fscanf(fp, "%[^\n]s", buf);
                fclose(fp);
                fp = fopen(dir_name, "w");
                fprintf(fp, "%s\n", buf);
                fclose(fp);
            }
            else if(!strcmp(argv[narg-2], ">")) { // ">"
                printf("> 실행\n");
                for(i=0; argv[i] != NULL; i++) {
                    if(!strcmp(argv[i], ">")) {
                        break;
                    }
                }
                fp = fopen(argv[i-1], "r");
                fscanf(fp, "%[^\n]s", buf);
                fclose(fp);
                fp = fopen(argv[i+1], "w");
                fprintf(fp, "%s\n", buf);
                fclose(fp);
            }
            else if(!strcmp(argv[narg-2], ">>")) { // ">>"
                printf(">> 실행\n");
                for(i=0; argv[i] != NULL; i++) {
                    if(!strcmp(argv[i], ">>")) {
                        break;
                    }
                }
                fp = fopen(argv[i-1], "r");
                fscanf(fp, "%[^\n]s", buf);
                fclose(fp);
                fp = fopen(argv[i+1], "a");
                fprintf(fp, "%s\n", buf);
                fclose(fp);
            }
            else if(!strcmp(argv[narg-2], "|")) {  // "|"
                printf("| 실행\n");
                for(i=0; argv[i] != NULL; i++) {
                    if(!strcmp(argv[i], "|")) {
                        break;
                    }
                }
                if(pipe(p) == -1) {
                    perror("pipe call failed");
                    exit(1);
                }
                write(p[1], argv[i-1], 16);
                write(p[1], argv[i+1], 16);
            }
            if(!strcmp(argv[narg-1], "&")) {  //"&"
                printf("& 실행\n");
            }
            else {
                exit(0); 
            }
        }
        else if(pid > 0) {
            wait((int*)0);
	    if(!strcmp(argv[0], "exit")) {  // "exit"
                exit(0);
            }
 	    else if(!strcmp(argv[0], "cd")) {  // "cd"
                changeDirectory(argv);
            }
        }
        else
            perror("fork failed");
    }
}int getargs(char *cmd, char **argv) {
    int narg = 0;
    while(*cmd) {
        if (*cmd == ' ' || *cmd == '\t' || *cmd == '\n')
            *cmd++ = '\0';
        else {
            argv[narg++] = cmd++;
            while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n')
                cmd++;
        }
    }
    argv[narg] = NULL;
    return narg;
}
