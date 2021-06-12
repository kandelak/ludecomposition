#include "ludecomp.h"
#include "test.h"
#include "io.h"
#include <time.h>
#include <unistd.h>
#include "ludecomp_double_array.h"




int main(int argc, char **argv) {
   int n = 5 ;
   float genA[n][n];
   matrixGenerator_double_array(n,genA);
   printMatrix_double_array(n,genA);
   float L[n][n],U[n][n],P[n][n];
   ludecomp_double_array(n,genA,L,U,P);
   
   printMatrix_double_array(n,L);
   printMatrix_double_array(n,U);

   printMatrix_double_array(n,P);

}
