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

int float_f2i(float_bits uf){
    unsigned sign = uf>>31;
    int exp = uf>>23 &0xff;  //移到最低位的一端，然后取出相应的数字
    unsigned frac = uf&0x7fffff;  //使前9位都为0

    if (exp==0xff){  //判断是否为  NAN
        return 0x80000000;
    }

    if(exp==0)//处理非规格类型
        exp = 1-((1<<7)-1);
    else{
        exp = exp-((1<<7)-1);
        frac = (frac|0x800000);//在第9位加上默认存在的1
    }
    if(exp<-8)    //由于变量移位大于32位，会自动取余，所以要单独处理
        frac=0;
    else if(exp<=23)
        frac = (frac&((-1<<(23-exp))))>>(23-exp);
    else if(exp<54)
        frac = frac<<(exp-23);
    else
        return 0x80000000;  //超出了int的最大范围，于是返回 0x80000000u
    if(sign==1)   //如果是负数，返回补码
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