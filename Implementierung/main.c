#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>



void printMatrix(size_t n, const float* M) {
    for (int index = 0; index < n*n; index++) {
        printf("%f,", M[index]);
        if ((index+1) % n == 0) printf("\n");
    }
    printf("\n");
}


//tauscht Zeilen zl1 zl2 in Matrix M
void tausche_zeilen(size_t n,float* M,size_t zl1,size_t zl2,size_t bis_dieser_spalte){

         //kann vektorisiert werden
         for(int i=0;i<bis_dieser_spalte;i++){
             float temp = M[zl1*n+i];
             M[zl1*n+i] = M[zl2*n+i];
             M[zl2*n+i] = temp;
         }

    }

//tausch Spalten sp1 sp2 in Matrix M
void tausche_spalten(size_t n ,float* M,size_t sp1,size_t sp2){

         //kann vektorisiert werden         
         for(int i=0;i<n;i++ ){
             float temp = M[i*n+sp1];
             M[i*n+sp1] = M[i*n+sp2];
             M[i*n+sp2] = temp;
         }

    }


void pivotize(size_t n,float* L,float* U,float* P,size_t zeile_zu_tauschen,size_t zeile_mit_max){
    size_t bis_dieser_spalte = zeile_zu_tauschen;
    //tausche die Zeilen in U komplett
    tausche_zeilen(n,U,zeile_zu_tauschen,zeile_mit_max,n);

    //tausche Zeilen in L nicht komplett sonder nur bis i-te spalte
    tausche_zeilen(n,L,zeile_zu_tauschen,zeile_mit_max,bis_dieser_spalte);

    //printf("L nach dem Tauchen:  \n ");
    //printMatrix(n,L);
    tausche_spalten(n,P,zeile_zu_tauschen,zeile_mit_max);

}


//void ludecomp(size_t n, const float A, float* L, float* U);
void luZerlegung(size_t n, const float* A, float* L, float* U) {
    //kopie von a in U
    for (int index = 0; index < n*n; index++) {
        U[index] = A[index];
    }
    //printf("Kopie von A in U\n");
    //printMatrix(n, U);


    //for pivotizing
    float P[n*n];

    //Einheitsmatrix in L
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j) { 
                L[i*n + j] = 1;
                P[i*n+j] = 1;  
	       	}
            else{
		        L[i*n + j] = 0;
	            P[i*n+j] = 0;		
	        }
        }
    }
    //printf("Einheitsmatrix in L \n");
    //printMatrix(n, L);
    //printf("Einheitsmatrix in P \n");
    //printMatrix(n,P);


    //Gaus-Eliminierung
    //für jede Spalte ab führenden eintrag 0...0
    for (size_t i = 0; i < n; i++) {
        //finde betragsgroesstes Element in Spalte i
        float max = U[i+i*n];
        size_t zeile_mit_max = i;
        for(size_t k=i+1;k<n;k++){
            if(abs(U[i+k*n])>max){
                max = U[i+k*n];
                zeile_mit_max = k;
            }     
        }
       //printf("grosstes Element in Spalte %d : %f in Zeile %ld\n",i,max,zeile_mit_max);            
       pivotize(n,L,U,P,i,zeile_mit_max);
       
	   
	   //für jede Zeile ab führenden eintrag
        for (size_t j = i; j < n-1; j++) {
            float faktor = U[i+(j+1)*n] / U[i+(i*n)];
            //in L schreiben 
            L[i+(j+1)*n] = faktor;
            //kann vektorisiert werden`
            for (size_t x = 0; x < n; x++) {
                U[(j+1)*n+x] -= U[(i*n)+x] * faktor;
            }
        }
    }
    
    //printf("Ergebnis in P \n");
    //printMatrix(n,P);
    //printf("Ergebnis in U \n");
    //printMatrix(n, U);
    //printf("Ergebnis in L \n");
    //printMatrix(n,L);
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
        if (abs(orgM[i] - M[i]) >= 1e-9) printf("\x1B[31m%f,", M[i]);
        else printf("\x1B[0m%f,", M[i]);
        if ((i+1) % n == 0) printf("\n");
    }
    printf("\x1B[0m\n");
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
    printf("\n\n\n");
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
    float orgL5[] = {1, 0, 0, 1};
    float orgU5[] = {-7.3, 8, 0, 2};
    printf("###### Eingabe:\n");
    printMatrix(n, A5);
    luZerlegung(n,A5,L5,U5);
    printResult(n, orgA5, A5, orgL5, L5, orgU5, U5);

}














int main(int argc, char** argv) {
    //float A[16] = {1,2,1,1, 
//	           2,2,3,3, 
//	           3,5,4,8, 
//	           9,12,-3,0};
  
   // float A[16] = {2,4,3,5,
     //              -4,-7,-5,-8,
//	           6,8,2,9,
  //                 4,9,-2,14};	
    float A[16] = {6,5,3,-10,
                  3,7,-3,5,
                  12,4,4,4,
                  0,12,0,-8}; 
	
	
    float L[16];
    float U[16];
    //luZerlegung(4, A, L, U);
    tests();

}
