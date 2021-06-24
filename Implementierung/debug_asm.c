#include "matr_utilities.h"

void ludecomp_asm(size_t n, const float* A, float* L, float* U, float *P);


int main(int argc, char** argv) {
    size_t n = 3;
    float L[n * n];
    float U[n * n];
    float P[n * n];
    float A[] = {1,2,4,2,3,8,-1,-3,-1};
    ludecomp_asm(n,A,L,U,P);
    printf("Matrix A\n");
    printMatrix(n,A);
    printf("Matrix L\n");
    printMatrix(n,L);
    printf("Matrix U\n");
    printMatrix(n,U);
    printf("Matrix P\n");
    printMatrix(n,P);
}