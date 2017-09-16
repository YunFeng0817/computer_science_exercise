#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int lower_one_mask(int n){
	 int a = ~(-1<<(n-1)<<1);      //此处有坑：当n = 32时，编译器会自动对n取模32运算 
	 return a; 
}

int main(int argc, char *argv[]) {
	int a = lower_one_mask(32);
	printf("0x%0X\n",a);
	return 0;
}
