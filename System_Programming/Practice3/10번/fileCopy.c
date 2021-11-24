#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>

#define SHMSIZE 4096
#define SHMNAME "/my_shm"
#define SEMNAME "/my_sem"

void p(sem_t *semd);
void v(sem_t *semd);

void main(int argc, char* argv[])
{
  FILE *fp, *copy_fp;   // file ptr
  char *path = argv[1]; 
  char buf[SHMSIZE]; 
  int status, shmd, count;
  void *shmaddr;
  pid_t pid;
  sem_t *semd;
  int i, val;

  if ((semd = sem_open(SEMNAME, O_CREAT, 0600, 1)) == SEM_FAILED) {
    perror ("sem_open failed");
    exit (1);
  }    // semaphore Open

  sem_getvalue(semd, &val);
  memset(buf, 0, SHMSIZE); // memory init

  if((pid = fork()) == 0){
    /* chile shm write*/

    if ((shmd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666)) == -1) {
      perror ("shm_open failed");
      exit (1);
    }

    if (ftruncate(shmd, SHMSIZE) != 0) {
      perror ("ftruncate failed");
      exit (1);
    }

    if ((shmaddr = mmap(0, SHMSIZE, PROT_WRITE,	MAP_SHARED, shmd, 0)) == MAP_FAILED) {
      perror ("mmap failed");
      exit (1);
    }

    if((fp = fopen(path, "r")) == NULL){ // file exception handle
      perror("fopen");
      exit(1);
    }

    if (mlock(shmaddr, SHMSIZE) != 0) {
      perror ("mlock failed");
      exit (1);
    }
    
    p(semd);
    printf("Child Process %d : semaphore in use\n", getpid());

    while(feof(fp) != 0){

      count = fread(buf, sizeof(char), SHMSIZE, fp);
      strcpy((char *)shmaddr, buf);      

    }
    v(semd);
    printf("Child Process %d : putting semaphore\n", getpid());
    
    if (munmap(shmaddr, SHMSIZE) == -1) {
      perror ("munmap failed");
      exit (1);
    }

    fclose(fp);
    close(shmd);

    exit(0);
  }else if (pid > 0){
    /* parents read */
    pid = wait(&status);
    
    if ((shmd = shm_open(SHMNAME, O_RDWR, 0666)) == -1) {
      perror ("shm_open failed");
      exit (1);
    }
  
    if ((shmaddr = mmap(0, SHMSIZE, PROT_READ, MAP_SHARED, shmd, 0)) == MAP_FAILED) {
      perror ("mmap failed");
      exit (1);
    }
  
    if (mlock(shmaddr, SHMSIZE) != 0) {
      perror ("mlock failed");
      exit (1);
    }

    strcat(path,"_copy"); // append path + "_copy"

    if((copy_fp = fopen(path, "w")) == NULL){ // file exception handle
      perror("fopen");
      exit(1);
    }

    p(semd);
    printf("Parent Process %d : semaphore in use\n", getpid());

    // shm data copy to copy_fp
    fwrite(shmaddr, sizeof(char), SHMSIZE, copy_fp);
    fclose(copy_fp);
    
    if (munmap(shmaddr, SHMSIZE) == -1) {
      perror ("munmap failed");
      exit (1);
    }

    printf("Parents Process %d : putting semaphore\n", getpid());
    v(semd);

    close(shmd);
    
    if (shm_unlink(SHMNAME) == -1) {
      perror ("shm_unlink failed");
      exit (1);
    }

    if (sem_close(semd) == -1) {
      perror ("sem_close failed");
      exit (1);
    }

  }else{
    perror("fork Error");
    exit(1);
  }
}


void p(sem_t *semd) {
  int ret;
  if ((ret = sem_trywait(semd)) != 0 && errno == EAGAIN)
    sem_wait(semd);
  else if (ret != 0) {
    perror ("sem_trywait failed");
    exit (1);
  }
}
void v(sem_t *semd) {
  if (sem_post(semd) != 0) {
    perror ("sem_post failed");
    exit (1);
  }
}
