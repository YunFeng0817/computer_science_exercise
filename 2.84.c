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
    printf("%0x,%0x\n",sx,sy);
    printf("%0x,%0x\n",ux&(~INT_MIN),uy&(~INT_MIN));
    return ((sx|sy==0)&&(ux<=uy)) || ((sx>sy)&&((ux&~INT_MIN)!=0)&&((ux&~INT_MIN)!=0)) || ((ux&~INT_MIN)==0&&(uy&~INT_MIN)==0) || ((sx&sy==1)&&(ux>=uy));
}

int main(){
    printf("%d",flaot_le(3.1415926,0.141592));
    return 0;
}
