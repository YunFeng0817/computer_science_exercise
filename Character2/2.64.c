#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int any_odd_one(unsigned x){
	unsigned temp = 0x55555555;
	x = x&temp;
	return x!=0;
}



int main(int argc, char *argv[]) {
	int a = any_odd_one(0xf);
	printf("%0x",a);
	return 0;
}
