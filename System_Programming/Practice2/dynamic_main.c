#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(){
	void*handle;
	int (*add) (int,int), (*subtract) (int, int), (*multiply) (int, int);
	double (*divide) (int, int);
	char *error;
	handle = dlopen("./lib/libshare.so", RTLD_LAZY);
	if(!handle) {
		fputs (dlerror(),stderr);
		exit(1);
	}
	add = dlsym(handle, "add");
	if ((error = dlerror()) != NULL){
		fprintf (stderr, "%s", error);
		exit(1);
	}
	subtract = dlsym(handle, "subtract");
	if ((error = dlerror()) != NULL){
		fprintf (stderr, "%s", error);
		exit(1);
	}
	multiply = dlsym(handle, "multiply");
	if ((error = dlerror()) != NULL){
		fprintf (stderr, "%s", error);
		exit(1);
	}
	divide = dlsym(handle, "divide");
	if ((error = dlerror()) != NULL){
		fprintf (stderr, "%s", error);
		exit(1);
	}
	int a = 0, b = 0;
	printf("a : ");
	scanf("%d", &a);
	printf("b : ");
	scanf("%d", &b);
	
	printf("add result : %d\n sub result : %d\n mul result : %d\n div result : %lf\n",
		add(a,b), subtract(a,b), multiply(a,b), divide(a,b));
	return 0;
	dlclose(handle);
}
