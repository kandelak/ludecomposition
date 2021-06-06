#include <stddef.h>
#include <stdio.h>



void printMatrix(size_t n, const float* M) {
    for (int index = 0; index < n*n; index++) {
        printf("%f,", M[index]);
        if ((index+1) % n == 0) printf("\n");
    }
    printf("\n\n");
}



//void ludecomp(size_t n, const float A, float* L, float* U);


void luZerlegung(size_t n, const float* A, float* L, float* U) {
    //kopie von a in U
    for (int index = 0; index < n*n; index++) {
        U[index] = A[index];
    }
    printMatrix(n, U);


    //Einheitsmatrix in L
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {L[i*n + j] = 1;}
            else        {L[i*n + j] = 0;}
        }
    }
    printMatrix(n, L);


    //Gaus-Eliminierung
    //für jede Spalte ab führenden eintrag 0...0
    for (int i = 0; i < n; i++) {
        //für jede Zeile ab führenden eintrag
        for (int j = i; j < n-1; j++) {
            float faktor = U[i+((j+1)*n)] / U[i+(i*n)];
            //printf("%f für Zeile %d \n", faktor, (j+1));
            for (int x = 0; x < n; x++) {
                U[((j+1)*n)+x] -= U[(i*n)+x] * faktor;
            }
        }
    }
    printMatrix(n, U);
}






int main(int argc, char** argv) {
    float A[9] = {1,2,1, 2,2,3, 3,5,4};
    float L[9];
    float U[9];
    luZerlegung(3, A, L, U);
}
