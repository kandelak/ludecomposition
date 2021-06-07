#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.c"





int testOutput(size_t n, float* orgM, float* M) {
    int res = 0;
    for (size_t i=0; i < n*n; i++) {
        if (orgM[i] != M[i]) res++;
    }
    return res;
}


void printErrorMatrix(size_t n, float* orgM, float* M) {
    for (size_t i=0; i < n*n; i++) {
        if (orgM[i] != M[i]) printf("\x1B[31m%f,", M[i]);
        else printf("\x1B[0m%f,", M[i]);
        if ((i+1) % n == 0) printf("\n");
    }
    printf("\n");
}


void printResult(size_t n, float* orgA, float* A, float* orgL, float* L, float* orgU, float* U) {
    printf("###### Fehleranalyse:\n");
    int fehlerA = testOutput(n, orgA, A);
    int fehlerL = testOutput(n, orgL, L);
    int fehlerU = testOutput(n, orgU, U);
    printf("%d Fehler in A;   %d Fehler in L;   %d Fehler in U\n", fehlerA, fehlerL, fehlerU);
    if (fehlerA != 0) {
        printf("Fehler in A:\n");
        printErrorMatrix(n, orgA, A);
    }
    if (fehlerL != 0) {
        printf("Fehler in L:\n");
        printErrorMatrix(n, orgL, L);
    }
    if (fehlerU != 0) {
        printf("Fehler in U:\n");
        printErrorMatrix(n, orgU, U);
    }
    printf("\n\n");
}





int tests() {
    // Muss der rang wirklich immer >=1 sein??????????

    printf("############### Eingabe: 1x1-Matrix = 1 ###############\n");
    // input:
    size_t n = 1;
    float L[n*n];
    float U[n*n];
    float A[] = {1}; 
    // Lösungen:
    float orgA[] = {1}; 
    float orgL[] = {1};
    float orgU[] = {1};
    printf("###### Eingabe:\n");
    printMatrix(n, A);
    luZerlegung(n,A,L,U);
    //printf("#### Ausgabe:\n");
    //printMatrix(n, U);
    //printMatrix(n, L);
    printResult(n, orgA, A, orgL, L, orgU, U);
    



    printf("############### Eingabe: 1x1-Matrix = 5 ###############\n");
    // input:
    n = 1;
    float L1[n*n];
    float U1[n*n];
    float A1[] = {5}; 
    // Lösungen:
    float orgA1[] = {5}; 
    float orgL1[] = {1};
    float orgU1[] = {5};
    printf("###### Eingabe:\n");
    printMatrix(n, A1);
    luZerlegung(n,A1,L1,U1);
    printResult(n, orgA1, A1, orgL1, L1, orgU1, U1);




    printf("############### Eingabe: 1x1-Matrix = 5.4535192 ###############\n");
    // input:
    n = 1;
    float L2[n*n];
    float U2[n*n];
    float A2[] = {5.4535192}; 
    // Lösungen:
    float orgA2[] = {5.4535192}; 
    float orgL2[] = {1};
    float orgU2[] = {5.4535192};
    printf("###### Eingabe:\n");
    printMatrix(n, A2);
    luZerlegung(n,A2,L2,U2);
    printResult(n, orgA2, A2, orgL2, L2, orgU2, U2);




    printf("############### Eingabe: 2x2-Matrix = {1,0},{0,1} ###############\n");
    // input:
    n = 2;
    float L3[n*n];
    float U3[n*n];
    float A3[] = {1,0,0,1}; 
    // Lösungen:
    float orgA3[] = {1,0,0,1}; 
    float orgL3[] = {1,0,0,1};
    float orgU3[] = {1,0,0,1};
    printf("###### Eingabe:\n");
    printMatrix(n, A3);
    luZerlegung(n,A3,L3,U3);
    printResult(n, orgA3, A3, orgL3, L3, orgU3, U3);



    printf("############### Eingabe: 2x2-Matrix = {5,12},{-77,4.5} ###############\n");
    // input:
    n = 2;
    float L4[n*n];
    float U4[n*n];
    float A4[] = {5,12,-77,4.5}; 
    // Lösungen:
    float orgA4[] = {5, 12, -77, 4.5}; 
    float orgL4[] = {1, 0, -0.0649351, 1};
    float orgU4[] = {-77, 4.5, 0, 12.2922};
    printf("###### Eingabe:\n");
    printMatrix(n, A4);
    luZerlegung(n,A4,L4,U4);
    printResult(n, orgA4, A4, orgL4, L4, orgU4, U4);




    printf("############### Eingabe: 2x2-Matrix = {0, 2},{-7.3, 8} ###############\n");
    // input:
    n = 2;
    float L5[n*n];
    float U5[n*n];
    float A5[] = {0, 2, -7.3, 8}; 
    // Lösungen:
    float orgA5[] = {0, 2, -7.3, 8}; 
    float orgL5[] = {1, 0, 1, 0};
    float orgU5[] = {-7.3, 8, 0, 2};
    printf("###### Eingabe:\n");
    printMatrix(n, A5);
    luZerlegung(n,A5,L5,U5);
    printResult(n, orgA5, A5, orgL5, L5, orgU5, U5);

}



