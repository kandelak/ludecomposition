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
    printf("\n\n");
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




void lu_zerlegung(size_t n, const float* A, float* L, float* U) {
    //kopie von a in U
    for (size_t index = 0; index < n*n; index++) {
        U[index] = A[index];
    }


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

void printHelp(){
    printf("Hilfe zu benutzung des LU-Zerlegungs Programms:\n");
    printf("\n Das Programm zerlegt eine quadratische Matrix in 2 Dreiecksmatrizen welche multipliziert wieder die ursprüngliche Matrix ergeben.\n Flags und ihre Bedeutung:\n");
    printf("-h/--help: diese Nachicht Anzeigen\n");
    printf("-m: Eingabematrix bestimmen\n");
    printf("-n: bestimmen der Grösse einer zufällig generierten Matrix bei nicht Spezifizierung oder ungültiger Eingabe wird eine zufällige größe gewählt \n");
    printf("-p: Wenn gesetzt wird die Pivot Funktion der Berechnung abgeschaltet dies resultiert in einer besseren Performance kann aber kein richtigen Ergebnis Garantieren\n");
}


float* matrixGenerator(size_t n){
    if( n <= 0 ){ n = (3 + rand() % 17);} // wenn n ungültig generate random size
    
    float A[n*n];

    for( size_t i = 0; i < n*n; i++){

         A[i] = (float)(rand()%100) / (float)(rand()%100);
    }

return A;

}

float* readFile(char * path, size_t size){
  FILE * f = fopen(path,"r");
  

    if(f == NULL){fprintf(stderr, "File Konnte nicht geöffnet werden");
           exit(EXIT_FAILURE);}

    char c;
    char number[40];
    size_t cfill = 0;
    size_t ffill = 0;
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
char c;
size_t Pivot = 0;
float* A;
size_t n = 0;
size_t randomMatrix = 1;

FILE *fcount;


   while ((opt = getopt(argc, argv, "hpm:n:")) != -1) {
       switch (opt) {
       case 'm':
       fcount = fopen(optarg,"r");
       if(fcount == NULL){fprintf(stderr, "File Konnte nicht geöffnet werden");
           exit(EXIT_FAILURE);}
       while((c = fgetc(fcount)) != EOF) {
           if(c == ' '){n++;continue;}
           if(c == '\n'){n++;break;}
           }

        fclose(fcount); 

            A = readFile(optarg,n);
            randomMatrix = 0;
          break;

           case 'n':
           if( n == 0){
                n = atoll(optarg);
           }
           
           break;

       case 'p':
           Pivot = 1;  // Soll Pivot Funktion aussschalten
           break;

        case 'h':
           printHelp();
           return 0;
           break;

       default: /* '?' oder 'h' */
           fprintf(stderr, "Hilfe mit -h oder --help anzeigen");
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


	if(randomMatrix == 1){A = matrixGenerator(n);}
	
    float L[n*n];
    float U[n*n];
    if (Pivot == 0){lu_zerlegung(n, A, L, U);} 
    else{//luZerlegungOhnePivot(n, A, L, U);
    }
    return 0;
    
}
