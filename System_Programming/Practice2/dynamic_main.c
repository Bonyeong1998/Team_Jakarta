#include <stdio.h>
#include <dlfcn.h>

void main(){
	void*handle;
	int (*add) (int,int), (*sub) (int, int), (*mul) (int, int);
	double (*div) (int, int);
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
	sub = dlsym(handle, "sub");
	if ((error = dlerror()) != NULL){
		fprintf (stderr, "%s", error);
		exit(1);
	}
	mul = dlsym(handle, "mul");
	if ((error = dlerror()) != NULL){
		fprintf (stderr, "%s", error);
		exit(1);
	}
	div = dlsym(handle, "div");
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
		add(a,b), sub(a,b), mul(a,b), div(a,b));
	return 0;
	dlclose(handle);
}
