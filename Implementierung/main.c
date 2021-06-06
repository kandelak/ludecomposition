#include <stddef.h>
#include <stdio.h>



void printMatrix(size_t n, const float* M) {
    for (int index = 0; index < n*n; index++) {
        printf("%f,", M[index]);
        if ((index+1) % n == 0) printf("\n");
    }
    printf("\n\n");
}


void pivotize(size_t n,float* L,float* U,float* P){}


//void ludecomp(size_t n, const float A, float* L, float* U);


void luZerlegung(size_t n, const float* A, float* L, float* U) {
    //kopie von a in U
    for (int index = 0; index < n*n; index++) {
        U[index] = A[index];
    }
    printMatrix(n, U);


    //for pivotizing
    float P[n*n];

    //Einheitsmatrix in L
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
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
    printMatrix(n, L);
    printMatrix(n,P);

    
    //Gaus-Eliminierung
    //für jede Spalte ab führenden eintrag 0...0
    for (int i = 0; i < n; i++) {
        //für jede Zeile ab führenden eintrag
        for (int j = i; j < n-1; j++) {

    	    //to be implemented
	    if(U[i+((j+1)*n)]==0) 
		    pivotize(n,L,U,P);

	    
	    float faktor = U[i+((j+1)*n)] / U[i+(i*n)];
            //printf("%f für Zeile %d \n", faktor, (j+1));
<<<<<<< HEAD
           L[i+(j+1)*n] = faktor;
	    
	    
=======
            L[i+(j+1)*n] = faktor;
>>>>>>> c175183e256073c85035022d7dc5657b5e2059eb
	    for (int x = 0; x < n; x++) {
                U[((j+1)*n)+x] -= U[(i*n)+x] * faktor;
            }
        }
    }
    printMatrix(n, U);
    printMatrix(n,L);
}






int main(int argc, char** argv) {
    float A[16] = {1,2,1,1, 
	           2,2,3,3, 
	           3,5,4,8, 
	           9,12,-3,0};
    float L[16];
    float U[16];
    luZerlegung(4, A, L, U);
}
