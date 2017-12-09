#include<stdio.h>
#include<stdlib.h>
#include <string.h>
void copy_int(int val,void *buf,int maxbytes){
	if(maxbytes-(int)sizeof(val)>=0){     //此处的坑是：sizeof(val)返回一个size_t类型的数,size_t是无符号类型,maxbytes-sizeof(val)会把maxbytes强转为unsigned类型,相减结果永远大于0，导致if判断永远为真
		memcpy(buf,(void *)&val,sizeof(val));
	}
}

int main(){
	//int b=0;
	void *a = malloc(4);
	copy_int(55,a,4);
	return 0;
}
