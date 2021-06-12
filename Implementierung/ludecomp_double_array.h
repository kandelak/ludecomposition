#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

double ludecomp_double_array(size_t n, const float A[n][n], float L[n][n], float U[n][n],float P[n][n]) {

  double start = curtime();
  // Copying A in U
  for (size_t index = 0; index < n; index++) {
    for(size_t i = 0;i<n;i++)
      U[index][i] = A[index][i];
  }

  // Writing Identity matrices in P and L
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (i == j) {
        L[i][j] = 1;
        P[i][j] = 1;
      } else {
        L[i][j] = 0;
        P[i][j] = 0;
      }
    }
  }

  for (size_t i = 0; i < n; i++) {

    // Searching for maximal absolute value in Column i
    float max = U[i][i];
    size_t row_with_max = i;
    for (size_t k = i + 1; k < n; k++) {
      if (abs(U[k][i]) > max) {
        max = U[k][i];
        row_with_max = k;
      }
    }

    // Pivotizing

    // Swapping rows i and row_with_max in U and L
    for (size_t k = 0; k < n; k++) {
      float temp = U[i][k];
      U[i][k] = U[row_with_max][k];
      U[row_with_max][k] = temp;
      // swapping in L (Only before i-th column)
      if (k < i) {
        temp = L[i][k];
        L[i][k] = L[row_with_max][k];
        L[row_with_max][k] = temp;
      }
    }

    // Swapping Columns in P
    for (size_t k = 0; k < n; k++) {
      float temp = P[k][i];
      P[k][i] = P[k][row_with_max];
      P[k][row_with_max] = temp;
    }

    // pivotizing done
      
	  
	  
for (size_t j = i; j < n - 1; j++) {



      float faktor = U[j+1][i] / U[i][i];

      // Writing factors in L
      L[j+1][i] = faktor;

      for (size_t x = 0; x < n; x++) {
        U[j+1][x] -= U[i][x] * faktor;
      }
    }
  }
  double end = curtime();
  return end-start;
}
double ludecomp_double_array_withoutP(size_t n, const float A[n][n], float L[n][n], float U[n][n]) {

  double start = curtime();
  // Copying A in U
  for (size_t index = 0; index < n; index++) {
    for(size_t i = 0;i<n;i++)
      U[index][i] = A[index][i];
  }

  // Writing Identity matrices in P and L
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (i == j) {
        L[i][j] = 1;
      } else {
        L[i][j] = 0;
      }
    }
  }

  for (size_t i = 0; i < n; i++) {

      for (size_t j = i; j < n - 1; j++) {

      float faktor = U[j+1][i] / U[i][i];

      // Writing factors in L
      L[j+1][i] = faktor;

      for (size_t x = 0; x < n; x++) {
        U[j+1][x] -= U[i][x] * faktor;
      }
    }
  }
  double end = curtime();
  return end-start;
}

void matrixMul_double_array(size_t n,float A[n][n],float B[n][n],float res[n][n]){

        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                for(size_t k=0;k<n;k++)
                    res[i][j] += A[i][k] * B[k][j];
            }
        }
}

void printMatrix_double_array(size_t n, const float M[n][n]) {
  for (size_t index = 0; index < n; index++) {
	  for(int j=0;j<n;j++){
    printf("%f ", M[index][j]);
	  }
      printf("\n");
   }

      printf("\n\n");
}
void matrixGenerator_double_array(size_t n, float A[n][n]) {
  srand((unsigned int)time(NULL));
  float a =
      7867865.34598; // Zufallsgenerator:
                     // https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
  for (size_t i = 0; i < n; i++)
	  for(int j=0;j<n;j++){
    A[i][j] = (((float)rand() / (float)(RAND_MAX)) - 0.5) * a;
	  }
} 
