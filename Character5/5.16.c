//
// Created by Fitz on 2017/12/9.
//
//
// Created by Fitz on 2017/12/9.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    long len;
    long *data;
} vec_rec,*vec_ptr;

typedef long data_t;

void inner4(vec_ptr u,vec_ptr v,data_t *data)
{
    long i;
    long length=u->len;
    data_t *udata=u->data,*vdata=v->data;
    data_t sum=(data_t)0;

    for(i=0;i<length-6;i+=6)
    {
        sum = sum + (udata[i] * vdata[i] + (udata[i+1] * vdata[i+1] +
                (udata[i+2] * vdata[i+2] +
                (udata[i+3] * vdata[i+3] +
                (udata[i+4] * vdata[i+4] +
              udata[i+5] * vdata[i+5])))));
    }

    for(;i<length-1;i++)
    {
        sum+=udata[i]*vdata[i];
    }

    *data=sum;
}

int main()
{
    long len=100;
    data_t *array=(data_t*)malloc(sizeof(data_t)*len);
    memset(array,1, sizeof(data_t)*len);
    data_t result=1,*ret=NULL;
    ret=&result;
    vec_ptr u=malloc(sizeof(vec_rec)),v=malloc(sizeof(vec_rec));
    u->data=array;
    u->len=len;
    v->data=array;
    inner4(u,v,ret);
    printf("%ld\n",*ret);
    return 0;
}

