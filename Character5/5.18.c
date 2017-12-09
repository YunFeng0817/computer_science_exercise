//
// Created by Fitz on 2017/12/9.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* calculate a0 + a1*x + a2*x^2 + ... + an*x^n */
double poly(double a[], double x, long degree) {
  long i;
  double result = a[0];
  double xpwr = x;
  for (i = 1; i <= degree; i++) {
    result += a[i] * xpwr;
    xpwr = x * xpwr;
  }
  return result;
}

/* apply horner's method */
double polyh(double a[], double x, long degree) {
  long i;
  double result = a[degree];
  for (i = degree-1; i >= 0; i--) {
    result = a[i] + x*result;
  }
  return result;
}

double polyMy(double a[],double x,long degree){
  long i;
  double result=a[0];
  double xpwr=x;
  double sum[5];
  double temp[4]={x*x,x*x*x*x,x*x*x*x*x};
  for(i=0;i<degree-5;i+=5)
  {
    sum[0]=result*xpwr+result*xpwr*x;
    sum[1]=result*(xpwr*temp[2]);
    sum[2]=result*(xpwr*temp[0]);
    sum[3]=result*(xpwr*temp[1]);
    result=result+(sum[0]+(sum[1]+(sum[2]+sum[3])));
  }
  for(;i<degree-1;i++)
  {
      result += a[i] * xpwr;
      xpwr = x * xpwr;
  }
  return result;
}


#define LOOP 100000
#define LEN 100000

int main(int argc, char* argv[]) {
  int time;
  double x;
  long degree;

  x = 1;
  degree = LEN;
  double b[LEN + 1];

    time=clock();
    for (int c = 0; c < LOOP; c++) {
        polyMy(b, x, degree);
    }
    time=clock()-time;
    printf("%d\n",time);

  time=clock();
  for (int c = 0; c < LOOP; c++) {
    poly(b, x, degree);
  }
  time=clock()-time;  //输出运行的时间 单位是毫秒
  printf("%d\n",time);
  time=clock();
  for (int c = 0; c < LOOP; c++) {
    polyh(b, x, degree);
  }
  time=clock()-time;
  printf("%d\n",time);

  return 0;
}
