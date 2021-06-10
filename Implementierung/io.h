#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "test.h"
#include <sys/random.h>

void printHelp(){
    printf("Hilfe zu benutzung des LU-Zerlegungs Programms:\n");
    printf("\n Das Programm zerlegt eine quadratische Matrix in 2 Dreiecksmatrizen welche multipliziert wieder die ursprüngliche Matrix ergeben.\n Flags und ihre Bedeutung:\n");
    printf("-h/--help: diese Nachicht Anzeigen\n");
    printf("-m: Eingabematrix bestimmen\n");
    printf("-n: bestimmen der Grösse einer zufällig generierten Matrix bei nicht Spezifizierung oder ungültiger Eingabe wird eine zufällige größe gewählt \n");
    printf("-p: Wenn gesetzt wird die Pivot Funktion der Berechnung abgeschaltet dies resultiert in einer besseren Performance kann aber kein richtigen Ergebnis Garantieren\n");
    printf("-t: Starte Test Protokoll\n");
    printf("-b: Aktiviert Benchmarking \n");
}


float* matrixGenerator(size_t n){
    srandom(time(NULL));
    if( n <= 0 ){ n = (3 + rand() % 17);} // wenn n ungültig generate random size
    
    float A[n*n];
    for( size_t i = 0; i < n*n; i++){
       
         A[i] = (float)(random()* (random()))/ (float)(random()* (random()- random()));
    }
printf("generatormatrix \n");
 printMatrix(n,A);
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
    float matrix[size*size];

       while((c = fgetc(f)) != EOF) {

           if(c == ' '|| c == '\n' ){       // Achtung wenn file nicht mit Leerzeichen oder Absatz Endet dann wird das letzte wort nicht abgespeichert.(ggf Ändern)
               
               matrix[ffill] = atof(number);  // ggf Überprüfen ob chars nummern
               printf("%f\n",matrix[ffill]);
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


int ioFunction(void (*ludecomp)(size_t,float*,float*,float*,float*),int argc, char** argv) {
    printf("start\n");
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


static struct option long_options[] =
{
    {"help", optional_argument, NULL, 'h'}
};
printf("startwhile\n");
   while ((opt = getopt_long(argc, argv, "tbhpm:n:",long_options,NULL)) != -1) {

       switch (opt) {

       case 'm':
       printf("");
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

           case 't':
            // Soll Tests Ausführen
           break;

           case 'b':
             // Soll Benchmarking Aktivieren
           break;

       default: /* '?' oder 'h' */
           fprintf(stderr, "Hilfe mit -h oder --help anzeigen");
           exit(EXIT_FAILURE);
       }
   }
  
	if(randomMatrix == 1){
        A = matrixGenerator(n);}
    printf("%f\n",A[0]);
	 for(int i = 0 ; i<n*n ; i++){
         printf("%f \n", A[i]);
     }
    float L[n*n];
    float U[n*n];
    float P[n*n];
    printf("Vor MEthodenaufruf\n");
    if (Pivot == 0){ludecomp(n, A, L, U, P);} 
    else{//luZerlegungOhnePivot(n, A, L, U);
    }
    return 0;
    
}
