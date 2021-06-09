#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>


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

	if(randomMatrix == 1){A = matrixGenerator(n);}
	
    float L[n*n];
    float U[n*n];
    if (Pivot == 0){lu_zerlegung(n, A, L, U);} 
    else{//luZerlegungOhnePivot(n, A, L, U);
    }
    return 0;
    
}
