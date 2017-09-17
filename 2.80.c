//
// Created by Fitz on 2017/9/17.
//

#include <stdlib.h>

int threefourths(int x){
    int t = (x&INT_MIN)!=0;
    int temp = ((x+(x<<1))+t+(t<<1))>>2;
    //printf("%d\n",temp);
    return temp;
}


int main(){
    threefourths(-6);
    return 0;
}
