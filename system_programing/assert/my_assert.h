void myAssert(int exp){
if(exp == 0){
printf("%s : %s at %d\n", __FILE__, __func__, __LINE__ );
abort();
}
}
