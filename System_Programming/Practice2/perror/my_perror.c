#include <stdio.h>
#include <string.h>
#include <errno.h>
void my_perror(const char *s) {
if(s == NULL) {
printf("%s\n", strerror(errno));
}
printf("%s:%s\n", s, strerror(errno));
}

