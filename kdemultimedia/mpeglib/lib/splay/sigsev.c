/*
  No SegFault:

  g++ -o sigsev.exe sigsevTest.cpp 

  SegFault:

  g++ -o sigsev.exe -O6 sigsevTest.cpp 
 */

#include <stdio.h>
#undef __NO_MATH_INLINES                  // <<<< Add this line
#define __NO_MATH_INLINES 1               // <<<<   and this.
#include <math.h>


int main() {
  printf("hello Martin test->main\n");

  //pow(6.0,3.0);
  float value;
  value=cos(double(0));
  printf("Wert: %f\n",value);
  pow(6.0,3.0);

  printf("hi:\n");
  exit(0);
}

