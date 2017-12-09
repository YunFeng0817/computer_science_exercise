//
// Created by Fitz on 2017/9/17.
//

#include<stdio.h>
#include <stdlib.h>
#include <string.h>

void *callocc(size_t nmemb,size_t size){
    if((size*nmemb==0)||(INT_MAX/nmemb<size)){  //此处判断是否乘法溢出和是否有0的存在
        return NULL;
    }
    void *s = malloc(nmemb*size);
    memset(s,0,size*nmemb);
    return s;
}

int main(){
    void *s = callocc((unsigned)-1,(unsigned)-1);
    printf("%p",s);
    return 0;
}
