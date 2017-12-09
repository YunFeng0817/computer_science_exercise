#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

unsigned replace_byte(unsigned x,int i,unsigned char b){
	unsigned temp = 0xff<<(i*8);
	x = temp|x;
	unsigned ub =~((unsigned char)(~b)<<(i*sizeof(int)*2));
	x = x&ub;
	return x;
}

int main(int argc, char *argv[]) {
	unsigned a = replace_byte(0x12345678,1,0xAB);
	printf("0x%0x",a);
	return 0;
}

