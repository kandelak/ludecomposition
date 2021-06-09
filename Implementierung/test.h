#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>



void printMatrix(size_t n, const float* M) {
    for (size_t index = 0; index < n*n; index++) {
        printf("%f,", M[index]);
        if ((index+1) % n == 0) printf("\n");
    }
    printf("\n");
}

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
    float LxU[n*n], PxLxU[n*n];
    matrixMul(n, L, U, LxU);
    matrixMul(n, P, LxU, PxLxU);
    printMatrix(n, P);
    int fehlerPxLxU = testOutput(n, A, PxLxU);
    printf("%d Fehler bei L*U*P\n", fehlerPxLxU);
    if (fehlerPxLxU != 0) {
        printf("Fehler in Erbenis von L*U*P:\n");
        printErrorMatrix(n, A, PxLxU);
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







int tests(void (*ludecomp)(size_t,float*,float*,float*,float*)) {
    // Muss der rang wirklich immer = n sein????????????????????????????????????????????????????????????

    printf("############### Eingabe: 1x1-Matrix (Einheitsmatrix) ###############\n");
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
    ludecomp(n,A,L,U,P);
    printResultWithSolution(n, orgL, L, orgU, U, orgP, P);


    printf("############### Eingabe: 1x1-Matrix (N) ###############\n");
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
    ludecomp(n,A1,L1,U1, P1);
    printResultWithSolution(n, orgL1, L1, orgU1, U1, orgP1, P1);


    printf("############### Eingabe: 1x1-Matrix = (Q+) ###############\n");
    // input:
    n = 1;
    float L2[n*n];
    float U2[n*n];
    float P2[n*n];
    float A2[] = {5.453519}; 
    // Lösungen:
    float orgL2[] = {1};
    float orgU2[] = {5.453519};
    float orgP2[] = {1};
    printf("###### Eingabe:\n");
    printMatrix(n, A2);
    ludecomp(n,A2,L2,U2, P2);
    printResultWithSolution(n, orgL2, L2, orgU2, U2, orgP2, P2);


    printf("############### Eingabe: 1x1-Matrix = (Q-) ###############\n");
    // input:
    n = 1;
    float L3[n*n];
    float U3[n*n];
    float P3[n*n];
    float A3[] = {-453.519}; 
    // Lösungen:
    float orgL3[] = {1};
    float orgU3[] = {-453.519};
    float orgP3[] = {1};
    printf("###### Eingabe:\n");
    printMatrix(n, A3);
    ludecomp(n,A3,L3,U3, P3);
    printResultWithSolution(n, orgL3, L3, orgU3, U3, orgP3, P3);


    printf("############### Eingabe: 2x2-Matrix (Einheitsmatrix) ###############\n");
    // input:
    n = 2;
    float L4[n*n];
    float U4[n*n];
    float P4[n*n];
    float A4[] = {1,0,0,1}; 
    // Lösungen:
    float orgL4[] = {1,0,0,1};
    float orgU4[] = {1,0,0,1};
    float orgP4[] = {1,0,0,1};
    printf("###### Eingabe:\n");
    printMatrix(n, A4);
    ludecomp(n,A4,L4,U4, P4);
    printResultWithSolution(n, orgL4, L4, orgU4, U4, orgP4, P4);


    printf("############### Eingabe: 2x2-Matrix (Q) ###############\n");
    // input:
    n = 2;
    float L5[n*n];
    float U5[n*n];
    float P5[n*n];
    float A5[] = {5,12,-77,4.5}; 
    // Lösungen:
    float orgL5[] = {1, 0, -0.0649351, 1};
    float orgU5[] = {-77, 4.5, 0, 12.2922};
    float orgP5[] = {1,0,0,1};
    printf("###### Eingabe:\n");
    printMatrix(n, A5);
    ludecomp(n,A5,L5,U5, P5);
    printResultWithSolution(n, orgL5, L5, orgU5, U5, orgP5, P5);








// ############################ ab hier kommen die Fehler
printf("############### Eingabe: 6x6-Matrix (Q mit Zahlen mit max 6 Stellen) ###############\n");
    // input:
    n = 6;
    float L6[n*n];
    float U6[n*n];
    float P6[n*n];
    float A6[] = {655.08, 132.35, -444.69, -293.09, -33.3688, 480.9, 
                -153.38, -338.36, 403.94, -611.8, 43545, -3.698, 
                455.2, -532.68, 858.73, -794.67, -444.9, -3.0944, 
                -409.17, -583.37, -202.31, -547.79, -3581.37, -337.7, 
                55.4283, 727.074, 751.8, -36.649, -342.610, 5452.07, 
                -3.8745, -127.561, -641.719, 31.6518, -10065, 0.2249}; 
    float orgL6[] = {1, 0, 0, 0, 0, 0, 
                    -0.234, 1, 0, 0, 0, 0, 
                    0.695, 2.032, 1, 0, 0, 0, 
                    -0.625, 1.629, -1.734, 1, 0, 0, 
                    0.085, -2.329, 2.664, -2.117, 1, 0, 
                    -0.006, 0.412, -1.375, 0.799, -0.222, 1};
    float orgU6[] = {655.08, 132.35, -444.69, -293.09, -33.369, 480.9, 
                    0, -307.372, 299.821, -680.424, 43537.187, 108.9, 
                    0, 0, 558.433, 791.763, -88898.874, -558.569, 
                    0, 0, 0, 1750.666, -228697.528, -1183.425, 
                    0, 0, 0, 0, -146224.76, 4647.951, 
                    0, 0, 0, 0, 0, 1170.146};
    float orgP6[] = {1, 0, 0, 0, 0, 0, 0,
                    0, 1, 0, 0, 0, 0, 0,
                    0, 0, 1, 0, 0, 0, 0,
                    0, 0, 0, 1, 0, 0, 0,
                    0, 0, 0, 0, 1, 0, 0,
                    0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 1};
    printf("###### Eingabe:\n");
    printMatrix(n, A6);
    ludecomp(n, A6, L6, U6, P6);
    printResultWithSolution(n, orgL6, L6, orgU6, U6, orgP6, P6);


printf("############### Eingabe: 7x7-Matrix (Q mit Zahlen über 8 Stellen) ###############\n");
    // input:
    n = 7;
    float L7[n*n];
    float U7[n*n];
    float P7[n*n];
    float A7[] = {2, 2, 76, 1200, -134535, -1543, 24,
                -45.423, 8, 1, 45098, 4.9876, 768.234, -3,
                65, 0, 5, 0, 124, 345.75, -214,
                8961234, 1, -2, -423, -1.346345, -5, -430.4569,
                537808, 76.966359, -829386, 933768, 3372.22177, -664105.0054, -3.55908,
                645, 262161.9, -8924, -198.47847, 77405, -298598.539, 4.43,
                -635.39543, -8763, 470150.09, -31685.3, -51, -180299, 73}; 
    float orgL7[] = {1, 0, 0, 0, 0, 0, 0, 
                    -22.7115, 1, 0, 0, 0, 0, 0, 
                    32.5, -1.216704416, 1, 0, 0, 0, 0, 
                    4480617, -167741.104018868, 139760.747807597, 1, 0, 0, 0, 
                    268904, -10065.534201393, 10675.22475429, 1.257796787, 1, 0, 0, 
                    322.5, 4895.211800161, 23339.914928084, 16.127164343, 1460.623735926, 1, 0, 
                    -317.697715, -152.136805683, -2081.734709432, -1.220356425, -49.637605157, -0.033588922, 1};
    float orgU7[] = {2, 2, 76, 1200, -134535, -1543, 24, 
                    0, 53.423, 1727.074, 72351.8, -3055486.6649, -34275.6105, 542.076, 
                    0, 0, -363.661437957, 49030.754543923, 654887.382700335, 8789.963353799, -334.453737154, 
                    0, 0, 0, -92944933.87707996, -1258448727.561142, -64328571.71902847, 30136692.656977855, 
                    0, 0, 0, 0, 13700251.799901249, 56329873.66854997, -35332881.42138688, 
                    0, 0, 0, 0, 0, -81276484678.85268, 51127170676.85111, 
                    0, 0, 0, 0, 0, 0, -361625.935642242};
    float orgP7[] = {1, 0, 0, 0, 0, 0, 0,
                    0, 1, 0, 0, 0, 0, 0,
                    0, 0, 1, 0, 0, 0, 0,
                    0, 0, 0, 1, 0, 0, 0,
                    0, 0, 0, 0, 1, 0, 0,
                    0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 0, 0, 0, 1};
    printf("###### Eingabe:\n");
    printMatrix(n, A7);
    ludecomp(n, A7, L7, U7, P7);
    printResultWithSolution(n, orgL7, L7, orgU7, U7, orgP7, P7);

}





/*
############### Eingabe: 7x7-Matrix (Q mit Zahlen über 8 Stellen) ###############
2 2 76 1200 -134535 -1543 24
-45.423 8 1 45098 4.9876 768.234 -3
65 0 5 0 124 345.75 -214
8961234 1 -2 -423 -1.346345 -5 -430.4569
537808 76.966359 -829386 933768 3372.22177 -664105.0054 -3.55908
645 262161.9 -8924 -198.47847 77405 -298598.539 4.43
-635.39543 -8763 470150.09 -31685.3 -51 -180299 73
//P
I
//L
1 0 0 0 0 0 0 
-22.7115 1 0 0 0 0 0 
32.5 -1.216704416 1 0 0 0 0 
4480617 -167741.104018868 139760.747807597 1 0 0 0 
268904 -10065.534201393 10675.22475429 1.257796787 1 0 0 
322.5 4895.211800161 23339.914928084 16.127164343 1460.623735926 1 0 
-317.697715 -152.136805683 -2081.734709432 -1.220356425 -49.637605157 -0.033588922 1 
//U
2 2 76 1200 -134535 -1543 24 
0 53.423 1727.074 72351.8 -3055486.6649 -34275.6105 542.076 
0 0 -363.661437957 49030.754543923 654887.382700335 8789.963353799 -334.453737154 
0 0 0 -92944933.87707996 -1258448727.561142 -64328571.71902847 30136692.656977855 
0 0 0 0 13700251.799901249 56329873.66854997 -35332881.42138688 
0 0 0 0 0 -81276484678.85268 51127170676.85111 
0 0 0 0 0 0 -361625.935642242 




############### Eingabe: 6x6-Matrix (Q mit Zahlen mit max 6 Stellen) ###############
655.08 132.35 -444.69 -293.09 -33.3688 480.9
-153.38 -338.36 403.94 -611.8 43545 -3.698
455.2 -532.68 858.73 -794.67 -444.9 -3.0944
-409.17 -583.37 -202.31 -547.79 -3581.37 -337.7
55.4283 727.074 751.8 -36.649 -342.610 5452.07
-3.8745 -127.561 -641.719 31.6518 -10065 0.2249
//P 
I
//L
1 0 0 0 0 0 
-0.234 1 0 0 0 0 
0.695 2.032 1 0 0 0 
-0.625 1.629 -1.734 1 0 0 
0.085 -2.329 2.664 -2.117 1 0 
-0.006 0.412 -1.375 0.799 -0.222 1
//U
655.08 132.35 -444.69 -293.09 -33.369 480.9 
0 -307.372 299.821 -680.424 43537.187 108.9 
0 0 558.433 791.763 -88898.874 -558.569 
0 0 0 1750.666 -228697.528 -1183.425 
0 0 0 0 -146224.76 4647.951 
0 0 0 0 0 1170.146 














{53, 71, -73, -31},
{0, 0, 0, 0},
{84, 53, 62, -66}, 
{50, -19, 53, 68}
ERROR: matrix is singular (auch wenn eine splate nur 0er)

Matrix muss laut kit Satz 2.11 regular sein damit P*A eine L*R hat
(Sei A∈RN,N regulär. Dann existiert eine Permutationsmatrix P, 
so dass PA eine LR-Zerlegung PA=LR besitzt und für die Einträge|L[m,n]| ≤ 1gilt.)



Beste möglichen Laufzeiten: Kit S.49
Auswertung von P benötigt O(N2)Operationen, 
während das Lösen von mit der LR-Zerlegung O(N3) Operationen benötigt.



Pseudo-Code
https://de.wikipedia.org/wiki/Gau%C3%9Fsches_Eliminationsverfahren#LR-Zerlegung
http://www.am.uni-duesseldorf.de/~helzel/Lehre/WiSe1920/CompLinA/vorl/VL8.pdf




Matrix-Calc (auch LR-Zerlegung mit P)
https://matrixcalc.org/de/


*/


//int testsWithoutSolutions() {
//}


