#include "ludecomp.h"
#include <stdio.h>
//#include "test.h"
#include "io.h"
int main(int argc,char** argv){

float A[16] = {6,5,3,-10,3,7,-3,5,12,4,4,4,0,12,0,-8};
float U[16],L[16],P[16];
/*ludecomp(4,A,L,U,P);
printMatrix(4,L);
printf("\n\n");
printMatrix(4,U);
printf("\n\n");
printMatrix(4,P);*/
ioFunction(ludecomp, argc, argv);
}

