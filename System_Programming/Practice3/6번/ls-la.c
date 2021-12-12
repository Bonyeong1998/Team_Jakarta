#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char* argv[]){
    execl("/bin/ls", argv[1], argv[2], (char *)0);
}

