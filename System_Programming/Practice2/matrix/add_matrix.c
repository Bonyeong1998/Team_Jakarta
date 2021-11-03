#include <stdio.h>
#include <stdlib.h>
int main(void) {
int a = 0, b = 0;
int i = 0;
int *ptr_matrix1, *ptr_matrix2, *ptr_result;
printf("Please enter the size of the matrix.\n");
scanf("%d", &a);
scanf("%d", &b);
ptr_matrix1 = (int*)malloc(a*b*sizeof(int));
if (ptr_matrix1 == NULL) {
printf("Failed to allocate memory.\n");
exit(-1);
}
ptr_matrix2 = (int*)malloc(a*b*sizeof(int));
if (ptr_matrix2 == NULL) {
printf("Failed to allocate memory.\n");
exit(-1);
}
printf("Please enter a matrix A.\n");
for(i = 0; i < a*b; i++){
scanf("%d", &ptr_matrix1[i]);
}
printf("Please enter a matrix B.\n");
for(i = 0; i < a*b; i++){
scanf("%d", &ptr_matrix2[i]);
}
ptr_result = (int*)malloc(a*b*sizeof(int));
if (ptr_result == NULL) {
printf("Failed to allocate memory.\n");
exit(-1);
}
for(i = 0; i < a*b; i++) {
ptr_result[i] = ptr_matrix1[i] + ptr_matrix2[i];
}
printf("RESULT:\n");
for(i = 0; i < a*b; i++) {
printf("%d ", ptr_result[i]);
if (i % b == b - 1)
printf("\n");
}
free((int*)ptr_matrix1);
free((int*)ptr_matrix2);
free((int*)ptr_result);
return 0;
}
