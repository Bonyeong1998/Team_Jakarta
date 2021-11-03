#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int fd;
	char *buf = "저수준 파일 처리 함수를 이용한 파일생성";
	ssize_t cnt;
	int flags = O_WRONLY | O_CREAT | O_TRUNC;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // ==0644
	
	if(argc < 2){
		fprintf(stderr, "Usage: file_creat filename\n");
		exit(1);
	} 
	
	if ((fd = open(argv[1], flags, mode)) == -1){
		perror("open");
		exit(1);
	}
	
	cnt = write(fd, buf, strlen(buf));
	printf("write count = %ld\n", cnt);
	close(fd);

	return 0;
}
