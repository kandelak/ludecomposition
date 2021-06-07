#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

void printMatrix(size_t n, const float* M) {
    for (int index = 0; index < n*n; index++) {
        printf("%f,", M[index]);
        if ((index+1) % n == 0) printf("\n");
    }
    printf("\n\n");
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

                  printf("L nach dem Tauschen:  \n ");
		  printMatrix(n,L);
		  tausche_spalten(n,P,zeile_zu_tauschen,zeile_mit_max);

}


//void ludecomp(size_t n, const float A, float* L, float* U);


void luZerlegung(size_t n, const float* A, float* L, float* U) {
    //kopie von a in U
    for (int index = 0; index < n*n; index++) {
        U[index] = A[index];
    }
    printf("Kopie von A in U\n");
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
    printf("Einheitsmatrix in L \n");
    printMatrix(n, L);
    printf("Einheitsmatrix in P \n");
    printMatrix(n,P);

    //Gaus-Eliminierung
    //für jede Spalte ab führenden eintrag 0...0
    for (int i = 0; i < n; i++) {
       
      //finde betragsgroesstes Element in Spalte i
	          float max = U[i+i*n];
	          size_t zeile_mit_max = i;
	          for(int k=i+1;k<n;k++){
	             if(abs(U[i+k*n])>max){
		        max = U[i+k*n];
			zeile_mit_max = k;
		     }     
                  }
       printf("grosstes Element in Spalte %d : %f in Zeile %ld\n",i,max,zeile_mit_max);          
		  
       
       pivotize(n,L,U,P,i,zeile_mit_max);
       
       
 //      printf("Ergebnis in P \n");
      // printMatrix(n,P);
   //   printf("Ergebnis in U \n");
    //   printMatrix(n, U);
       printf("Ergebnis in L \n");
       printMatrix(n,L);
	   
	   //für jede Zeile ab führenden eintrag
        for (int j = i; j < n-1; j++) {

	   float faktor = U[i+(j+1)*n] / U[i+(i*n)];
            //printf("%f für Zeile %d \n", faktor, (j+1));
            
	   //in L schreiben 
	    L[i+(j+1)*n] = faktor;
	    printf("Ergebnis in P \n");
     //  printMatrix(n,P);
     //  printf("Ergebnis in U \n");
    //   printMatrix(n, U);
       printf("Ergebnis in L \n");
       printMatrix(n,L);


	    //kann vektorisiert werden`
	    for (int x = 0; x < n; x++) {
                U[(j+1)*n+x] -= U[(i*n)+x] * faktor;
            }
        }
    }
    
    printf("Ergebnis in P \n");
    printMatrix(n,P);
    printf("Ergebnis in U \n");
    printMatrix(n, U);
    printf("Ergebnis in L \n");
    printMatrix(n,L);
}

void printHelp(){
printf("Hilfe zu benutzung des LU-Zerlegungs Programms:\n");
printf("\n Das Programm zerlegt eine quadratische Matrix in 2 Dreiecksmatrizen welche multipliziert wieder die ursprüngliche Matrix ergeben.\n Flags und ihre Bedeutung:\n");
printf("-h/ --help: diese NAchicht Anzeigen\n");
printf("-m: Eingabematrix bestimmen\n");
printf("-n: bestimmen der Grösse einer zufällig generierten Matrix bei nicht Spezifizierung oder ungültiger Eingabe wird eine zufällige größe gewählt \n");
printf("-p: Wenn gesetzt wird die Pivot Funktion der Berechnung abgeschaltet dies resultiert in einer besseren Performance kann aber kein Ergebnis Garantieren\n");
}
float* matrixGenerator(size_t n){
    if( n <= 0 ){ n = (3 + rand() % 17);}
    
    float A[(int)pow(n,2)];
    for( int i = 0; i < pow(n,2); i++){
         A[i] = ((float)(rand()%100)/(float)(rand()%100));
    }
return A;
// wenn n ==  -1 generate random size
}

float* readFile(char * path){
  FILE * f = fopen(path,"r");
  FILE * fcount = fopen(path,"r");
    if(f == NULL || fcount == NULL){fprintf(stderr, "File Konnte nicht geöffnet werden");
           exit(EXIT_FAILURE);}
    char c;
    char number[40];
    int cfill = 0;
    int ffill = 0;
    int size = 0;

        while((c = fgetc(fcount)) != EOF) {
           if(c == ' '|| c == '\n' ){size++;}
           }
          fclose(fcount); 
float matrix[size];

       while((c = fgetc(f)) != EOF) {
           if(c == ' '|| c == '\n' ){       // Achtung wenn file nicht mit Leerzeichen oder Absatz Endet dann wird das letzte wort nicht abgespeichert.(ggf Ändern)
               matrix[ffill] = atof(number);  // ggf Überprüfen ob chars nummern
               char number[40];  
               cfill = 0;
           }     
           else{
            number[cfill++] = c;

           }
    }
    fclose(f);

    return matrix;

}


int main(int argc, char** argv) {
// IO Funktionen
// Qenerierte Matrix benutzen: wenn kein -m
// Übergebene Matrix Benutzen: -m matrix 
// pivot deaktivieren: -p
// Hilfe Drucken: -h / --help
char opt;
int Pivot = 0;
float* A;
int n = -1;
   while ((opt = getopt(argc, argv, "hpm:n:")) != -1) {
       switch (opt) {
       case 'm':
            A = readFile(optarg);
          break;

           case 'n':
           n = atoll(optarg);
           break;
       case 'p':
           Pivot = 1;  // Soll Pivot Fonktion aussschalten
           break;
        case 'h':
           printHelp();
           break;
       default: /* '?' oder 'h' */
           fprintf(stderr, "Hilfe mit -h oder --help anzeigen",argv[0]);
           exit(EXIT_FAILURE);
       }
   }

    //float A[16] = {1,2,1,1, 
//	           2,2,3,3, 
//	           3,5,4,8, 
//	           9,12,-3,0};
  
   // float A[16] = {2,4,3,5,
     //              -4,-7,-5,-8,
//	           6,8,2,9,
  //                 4,9,-2,14};	
   // float A[16] = {6,5,3,-10,
     //             3,7,-3,5,
    ///              12,4,4,4,
     //             0,12,0,-8}; 
	if(A == NULL){A = matrixGenerator(n);}
	
    float L[16];
    float U[16];
    if (Pivot == 0){luZerlegung(4, A, L, U);} 
    else{//luZerlegungOhnePivot(4, A, L, U);
    }
    
}
