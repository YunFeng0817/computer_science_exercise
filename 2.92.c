//
// Created by Fitz on 2017/9/17.
//

#include <stdlib.h>
#include <stdio.h>

typedef unsigned float_bits;

float_bits float_negate(float_bits f){
    unsigned sign = f>>31;
    unsigned exp = (f&(~INT_MIN))>>23;
    unsigned frac = f&0x7fffff;  //使前23位都为0

    if (exp==0xffff){
        if(frac!=0){
            return f;
        }
    }
    sign = ~sign;
    return sign<<31|exp<<23|frac;
}

int main(){
    float_bits flot = float_negate(0x4048f5c3);
    printf("%0x\n",flot);
    return 0;
}
