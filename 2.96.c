//
// Created by Fitz on 2017/9/17.
//

#include <stdlib.h>
#include <stdio.h>

typedef unsigned float_bits;

//此用来测试，没有用在函数中float_f2i()
union tran{
    float f;
    unsigned u;
};

//此用来测试，没有用在函数中float_f2i()
float_bits f2u(float x){
    union tran temp;
    temp.f = x;
    return temp.u;

}

//此用来测试，没有用在函数中float_f2i()
float u2f(float_bits x){
    union tran temp;
    temp.u = x;
    return temp.f;

}

int float_f2i(float_bits f){
    unsigned sign = f>>31;
    int exp = (f&(~INT_MIN))>>23;
    unsigned frac = f&0x7fffff;  //使前9位都为0

    if (exp==0xff){
        if(frac!=0){
            return 0x80000000;
        }
    }

    if(exp==0)
        exp = (unsigned)1-((1<<7)-1);
    else{
        exp = exp-((1<<7)-1);
        frac = (frac|0x800000);
    }
    if(exp<=23)
        frac = (frac&((-1<<(23-exp))))>>(23-exp);
    else
        frac = frac<<(exp-23);
    if(sign==1)
        return (~frac)+1;
    return sign<<31|frac;
}

int main(){
    //以下用来测试
    float aa=-1122135456.211235;
    float_bits flow = f2u(aa);
    printf("system:%d\n",(int)aa);
    //以上是测试

    int integer = float_f2i(flow);
    printf("my    :%d\n",integer);
    return 0;
}

//0x7fffffff
//-1122135456.211235