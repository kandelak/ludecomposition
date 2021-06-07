#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.c"





int testOutput(size_t n, float* orgM, float* M) {
    int res = 0;
    for (size_t i=0; i < n*n; i++) {
        if (abs(orgM[i] - M[i]) >= 1e-9) res++;
    }
    return res;
}


void printErrorMatrix(size_t n, float* orgM, float* M) {
    for (size_t i=0; i < n*n; i++) {
        if (abs(orgM[i] - M[i]) >= 1e-9) printf("\x1B[31m%f (!=%f),", M[i], orgM[i]);
        else printf("\x1B[0m%f,", M[i]);
        if ((i+1) % n == 0) printf("\n");
    }
    printf("\x1B[0m\n");
}


void matrixMul(size_t n, float* A, float* B, float* Res){
    size_t i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            Res[i*n + j] = 0;
            for (k = 0; k < n; k++)
                Res[i*n + j] += A[i*n + k] * B[k*n + j];
        }
    }
}


void printResultWithoutSolution(size_t n, float* A, float* L, float* U, float* P) {
    printf("###### Fehleranalyse:\n");
    float LxU[n*n], LxUxP[n*n];
    matrixMul(n, L, U, LxU);
    matrixMul(n, P, LxU, LxUxP);
    int fehlerLxUxP = testOutput(n, A, LxUxP);
    printf("%d Fehler bei L*U*P\n", fehlerLxUxP);
    if (fehlerLxUxP != 0) {
        printf("Fehler in Erbenis von L*U*P:\n");
        printErrorMatrix(n, A, LxUxP);
    }
    printf("\n\n\n");
}




void printResultWithSolution(size_t n, float* orgL, float* L, float* orgU, float* U, float* orgP, float* P) {
    printf("###### Fehleranalyse:\n");
    int fehlerL = testOutput(n, orgL, L);
    int fehlerU = testOutput(n, orgU, U);
    int fehlerP = testOutput(n, orgP, P);
    printf("%d Fehler in L;   %d Fehler in U;   %d Fehler in P\n", fehlerL, fehlerU, fehlerP);
    if (fehlerL != 0) {
        printf("Fehler in L:\n");
        printErrorMatrix(n, orgL, L);
    }
    if (fehlerU != 0) {
        printf("Fehler in U:\n");
        printErrorMatrix(n, orgU, U);
    }
    if (fehlerP != 0) {
        printf("Fehler in P:\n");
        printErrorMatrix(n, orgP, P);
    }
    printf("\n\n\n");
}







int tests() {
    // Muss der rang wirklich immer >=1 sein??????????

    printf("############### Eingabe: 1x1-Matrix = 1 ###############\n");
    // input:
    size_t n = 1;
    float L[n*n];
    float U[n*n];
    float P[n*n];
    float A[] = {1}; 
    // Lösungen:
    float orgL[] = {1};
    float orgU[] = {1};
    float orgP[] = {1};
    printf("###### Eingabe:\n");
    printMatrix(n, A);
    luZerlegung(n,A,L,U,P);
    printResultWithSolution(n, orgL, L, orgU, U, orgP, P);


    printf("############### Eingabe: 1x1-Matrix = 5 ###############\n");
    // input:
    n = 1;
    float L1[n*n];
    float U1[n*n];
    float P1[n*n];
    float A1[] = {5};
    // Lösungen:
    float orgL1[] = {1};
    float orgU1[] = {5};
    float orgP1[] = {1};
    printf("###### Eingabe:\n");
    printMatrix(n, A1);
    luZerlegung(n,A1,L1,U1, P1);
    printResultWithSolution(n, orgL1, L1, orgU1, U1, orgP1, P1);


    printf("############### Eingabe: 1x1-Matrix = 5.4535192 ###############\n");
    // input:
    n = 1;
    float L2[n*n];
    float U2[n*n];
    float P2[n*n];
    float A2[] = {5.4535192}; 
    // Lösungen:
    float orgL2[] = {1};
    float orgU2[] = {5.4535192};
    float orgP2[] = {1};
    printf("###### Eingabe:\n");
    printMatrix(n, A2);
    luZerlegung(n,A2,L2,U2, P2);
    printResultWithSolution(n, orgL2, L2, orgU2, U2, orgP2, P2);


    printf("############### Eingabe: 2x2-Matrix = {1,0},{0,1} ###############\n");
    // input:
    n = 2;
    float L3[n*n];
    float U3[n*n];
    float P3[n*n];
    float A3[] = {1,0,0,1}; 
    // Lösungen:
    float orgL3[] = {1,0,0,1};
    float orgU3[] = {1,0,0,1};
    float orgP3[] = {1,0,0,1};
    printf("###### Eingabe:\n");
    printMatrix(n, A3);
    luZerlegung(n,A3,L3,U3, P3);
    printResultWithSolution(n, orgL3, L3, orgU3, U3, orgP3, P3);


    printf("############### Eingabe: 2x2-Matrix = {5,12},{-77,4.5} ###############\n");
    // input:
    n = 2;
    float L4[n*n];
    float U4[n*n];
    float P4[n*n];
    float A4[] = {5,12,-77,4.5}; 
    // Lösungen:
    float orgL4[] = {1, 0, -0.0649351, 1};
    float orgU4[] = {-77, 4.5, 0, 12.2922};
    float orgP4[] = {0,1,1,0};
    printf("###### Eingabe:\n");
    printMatrix(n, A4);
    luZerlegung(n,A4,L4,U4, P4);
    printResultWithSolution(n, orgL4, L4, orgU4, U4, orgP4, P4);


    printf("############### Eingabe: 2x2-Matrix = {0, 2},{-7.3, 8} ###############\n");
    // input:
    n = 2;
    float L5[n*n];
    float U5[n*n];
    float P5[n*n];
    float A5[] = {0, 2, -7.3, 8}; 
    // Lösungen:
    float orgL5[] = {1, 0, 0, 1};
    float orgU5[] = {-7.3, 8, 0, 2};
    float orgP5[] = {0,1,1,0};
    printf("###### Eingabe:\n");
    printMatrix(n, A5);
    luZerlegung(n,A5,L5,U5, P5);
    printResultWithSolution(n, orgL5, L5, orgU5, U5, orgP5, P5);





    printf("############### Eingabe: 2x2-Matrix = {0, 2},{-7.3, 8} ###############\n");
    // input:
    n = 2;
    float L6[n*n];
    float U6[n*n];
    float P6[n*n];
    float A6[] = {0, 2, -7.3, 8}; 
    printf("###### Eingabe:\n");
    printMatrix(n, A6);
    luZerlegung(n,A6,L6,U6, P6);
    printResultWithoutSolution(n, A6, L6, U6, P6);
}



//int testsWithoutSolutions() {
//}


int main(int argc, char** argv) {
    tests();
}
