#include "ludecomp.h"
#include "test.h"
#include "io.h"
#include <time.h>
#include <unistd.h>
int main(int argc, char **argv) {
  size_t n = 30;
  int cnt = 0;
  
  double time;
  while(n<1000){
 float* L =  malloc(32*n*n);
 float* U =  malloc(32*n*n);
 float* P =  malloc(32*n*n);
 float* genA =  malloc(32*n*n);
  matrixGenerator(n, genA);
  ludecomp(n, genA, L, U, P);
  time = ludecomp_without_P(n,genA,L,U);
  printf("%f\n",time);
  n+=1;
  free(L);free(U);free(P);free(genA);
  }
  fclose(fp);
}
