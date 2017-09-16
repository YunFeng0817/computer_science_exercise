#include<stdio.h>
#include<stdlib.h>
#include <string.h>
void copy_int(int val,void *buf,int maxbytes){
	if(maxbytes-(int)sizeof(val)>=0){
		printf("%d\n", (int)sizeof(val));
		memcpy(buf,(void *)&val,sizeof(val));
		int *temp;
		temp = buf;
		printf("success!\n");
		printf("%d\n",*temp);
	}
}

int main(){
	//int b=0;
	void *a = malloc(4);
	copy_int(55,a,4);
	return 0;
}
