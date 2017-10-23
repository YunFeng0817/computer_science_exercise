//
// Created by Fitz on 2017/9/17.
//

#include <stdlib.h>
#include <stdio.h>

typedef unsigned float_bits;

float_bits float_negate(float_bits f){
    unsigned exp = f>>23 &0xff;
    unsigned frac = f&0x7fffff;  //使前8位都为0
    if (exp==0xff){
        if(frac!=0){
            return uf;
        }
    }
    return exp<<23|frac;
}

int main(){
    float_bits flot = float_negate(0x4048f5c3);
    printf("%0x\n",flot);
    return 0;
}
