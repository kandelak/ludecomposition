#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>


void printMatrix(size_t n, const float* M) {
    for (size_t index = 0; index < n*n; index++) {
        printf("%f,", M[index]);
        if ((index+1) % n == 0) printf("\n");
    }
    printf("\n");
}


//tauscht Zeilen zl1 zl2 in Matrix M
void tausche_zeilen(size_t n,float* M,size_t zl1,size_t zl2,size_t bis_dieser_spalte){

         //kann vektorisiert werden
         for(size_t i=0;i<bis_dieser_spalte;i++){
             float temp = M[zl1*n+i];
             M[zl1*n+i] = M[zl2*n+i];
             M[zl2*n+i] = temp;
         }

    }

//tausch Spalten sp1 sp2 in Matrix M
void tausche_spalten(size_t n ,float* M,size_t sp1,size_t sp2){

         //kann vektorisiert werden         
         for(size_t i=0;i<n;i++ ){
             float temp = M[i*n+sp1];
             M[i*n+sp1] = M[i*n+sp2];
             M[i*n+sp2] = temp;
         }

    }


void pivotize(size_t n,float* L,float* U,float* P,size_t zeile_zu_tauschen,size_t zeile_mit_max){
    size_t bis_dieser_spalte = zeile_zu_tauschen;
    
    tausche_zeilen(n,U,zeile_zu_tauschen,zeile_mit_max,n);

    tausche_zeilen(n,L,zeile_zu_tauschen,zeile_mit_max,bis_dieser_spalte);

    tausche_spalten(n,P,zeile_zu_tauschen,zeile_mit_max);

}



void ludecomp(size_t n, const float* A, float* L, float* U, float* P) {
    //kopie von a in U
    for (size_t index = 0; index < n*n; index++) {
        U[index] = A[index];
    }


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
    
}


int main(int argc,char** argv){
}
