//
// Created by Fitz on 2017/9/17.
//

#include <stdlib.h>
#include <stdio.h>

union tran{
    float f;
    unsigned u;
};

unsigned f2u(float x){
    union tran temp;
    temp.f = x;
    return temp.u;

}

int flaot_le(float x, float y){
    unsigned ux = f2u(x);
    unsigned uy = f2u(y);

    unsigned sx = ux>>31;
    unsigned sy = uy>>31;
    printf("%0x,%0x",ux,uy);
    return 0;
}

int main(){
    flaot_le(3.1,3.25564684);
    return 0;
}
