#include "testlib.h"
#include <stdio.h>

int main(){
	int a, b;
	int i;
	while(1){
		printf("Choose the calculation you want.\n 1.add 2.sub 3.mul 4.div if you want exit press Ctrl+C key:  ");
		scanf("%d", &i);
		printf("a : ");
		scanf("%d", &a);
		printf("b : ");
		scanf("%d", &b);
		
		switch(i){
			case 1:
				printf("%d + %d = %d\n", a, b, add(a,b));
				break;
			case 2:
				printf("%d - %d = %d\n", a, b, subtract(a,b));
				break;
			case 3:
				printf("%d * %d = %d\n", a, b, multiply(a,b));
				break;
			case 4:
				printf("%d / %d = %lf\n", a, b, divide(a,b));
				break;
		}
	}
	
	return 0;
}
