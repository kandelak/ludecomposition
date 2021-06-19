#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <sys/stat.h>

void printHelp() {
    printf("Hilfe zu benutzung des LU-Zerlegungs Programms:\n");
    printf("\"This Program decomposes quadratic Matrices\n"
           "  in 2 Triangular Matrices, which multiplied equal to \n"
           "  The given Matrix. Definitions of flags : ");
    printf("-h/--help: Use to  print this Text\n");
    printf("-m: Define Input Matrix\n");
    printf("-n: bestimmen der Grösse einer zufällig generierten Matrix bei nicht "
           "Spezifizierung oder ungültiger Eingabe wird eine zufällige größe "
           "gewählt \n");
    printf("-p: Deactivates pivoting Method. "
           "No guarantee for correct answer");
    printf("-t: Activate Testing\n");
    printf("-b: Activate Benchmarking \n");
    printf("-o: Choose Output file \n");
    printf("-v: Choose Version of the Implementation");
    printf("-i : Choose number of iterations");
}

void writeMatrix(FILE *f, size_t n, const float *M) {
    for (size_t index = 0; index < n * n; index++) {
        fprintf(f, "%f,", M[index]);
        if ((index + 1) % n == 0)
            fprintf(f, "\n");
    }
}

void matrixGenerator(size_t n, float *A) {
    srand((unsigned int) time(NULL));
    float a =
            7867865.34598; // Zufallsgenerator:
    // https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
    for (size_t i = 0; i < n * n; i++)
        A[i] = (((float) rand() / (float) (RAND_MAX)) - 0.5) * a;
}

/**
 *
 * @param path  : Path to the file , from which numbers will be read
 * @param matrix : Matrix where the numbers will be written
 */
void readFile(char *path, float *matrix) {
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        perror("Couldn't open the file");
        exit(EXIT_FAILURE);
    }
    size_t n;
    size_t index = 0;

    while (fscanf(fp, "%f", (matrix + (index++))) == 1);

    if (n * n != index) {
        printf("ERROR : Wrong number of floats in file for the given size \n"
               "Usage : \nfirst number is always the size of the Matrix row/column \n"
               "All of the other numbers are entries for the Matrix");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

int ioFunction(void (*ludecomp)(size_t, float *, float *, float *, float *),
               int argc, char **argv) {

    // IO Funktionen
    // Qenerierte Matrix benutzen: wenn kein -m
    // Übergebene Matrix Benutzen: -m matrix
    // pivot deaktivieren: -p
    // Hilfe Drucken: -h / --help
    char opt;
    char c;
    size_t n = 0;
    size_t Pivot = 0;
    size_t randomMatrix = 1;
    size_t runTests = 0;
    size_t toFile = 0;
    FILE *fcount;
    FILE *fwrite;
    char *input;
    char *output = NULL;
    size_t iterations = 1;
    static struct option long_options[] = {
            {"help", optional_argument, NULL, 'h'}};

    while ((opt = getopt_long(argc, argv, "o:tbhpm:n:", long_options, NULL)) !=
           -1) { // o hat gerade verpflichetenden parameter soll noch freiwillig
        // werden

        switch (opt) {

            case 'm':
                input = optarg;
                randomMatrix = 0;
                break;

            case 'n':
                if (n == 0) {
                    n = atoll(optarg);
                }

                break;
            case 'i' :
                iterations = strtoul(optarg, NULL, 0);
                break;
            case 'p':
                Pivot = 1; // Soll Pivot Funktion aussschalten
                break;

            case 'h':
                printHelp();
                return 0;
                break;

            case 't':
                //  Führt tests aus
                runTests = 1;
                break;

            case 'b':
                // Soll Benchmarking Aktivieren
                break;

            case 'o':
                // Soll output Spezifizieren
                // toFile = 1;
                // if(optarg != NULL){ // funst nicht
                toFile = 2;
                output = optarg;
                //  }
                break;

            default: /* '?' oder 'h' */
                fprintf(stderr, "Hilfe mit -h oder --help anzeigen");
                exit(EXIT_FAILURE);
        }
    }

    if (runTests == 1) {
        return tests(ludecomp);
    }

    if (randomMatrix == 0) {
        fcount = fopen(input, "r");

        if (fcount == NULL) {
            fprintf(stderr, "File Konnte nicht geöffnet werden");
            exit(EXIT_FAILURE);
        }

        while ((c = fgetc(fcount)) != EOF) {
            if (c == ' ') {
                n++;
                continue;
            }
            if (c == '\n') {
                n++;
                break;
            }
        }
        fclose(fcount); // close fehler muss noch abgefangen werden wenn == 0 kein
        // fehler

    } else {
        srandom(time(NULL));
        if (n <= 0) {
            n = (3 + rand() % 17);
        } // wenn n ungültig generate random size
    }

    float A[n * n];

    if (randomMatrix == 0) {
        readFile(input, A);
    } else {
        matrixGenerator(n, A);
    }
    // printf("%f\n",A[0]);
    // for(int i = 0 ; i<n*n ; i++){
    // printf("%f \n", A[i]);}

    float L[n * n];
    float U[n * n];
    float P[n * n];
    while (iterations-- > 0) ludecomp(n, A, L, U, P);
    // if (Pivot == 0){}
    // else{//luZerlegungOhnePivot(n, A, L, U);
    //}
    if (toFile == 1) {
        strcat(input, "Result.txt");
        fwrite = fopen(input, 'w');
        if (fwrite == NULL) {
            fprintf(stderr, "Outputfile konnte nicht erstellt werden");
            exit(EXIT_FAILURE);
        }
        fprintf(fwrite, " Matrix A: \n\n");
        writeMatrix(fwrite, n, A);
        fprintf(fwrite, " ############################################### \n\n");
        fprintf(fwrite, " Matrix L: \n\n");
        writeMatrix(fwrite, n, L);
        fprintf(fwrite, " ############################################### \n\n");
        fprintf(fwrite, " Matrix U: \n\n");
        writeMatrix(fwrite, n, U);
        fprintf(fwrite, " ############################################### \n\n");
        fprintf(fwrite, " Matrix P: \n\n");
        writeMatrix(fwrite, n, P);
        fprintf(fwrite, " ############################################### \n\n");
        fclose(fwrite);
    }
    if (toFile == 2) {

        fwrite = fopen(output, 'w');
        if (fwrite == NULL) {
            fprintf(stderr, "Outputfile konnte nicht erstellt werden");
            exit(EXIT_FAILURE);
        }
        fprintf(fwrite, " Matrix A: \n\n");
        writeMatrix(fwrite, n, A);
        fprintf(fwrite, " ############################################### \n\n");
        fprintf(fwrite, " Matrix L: \n\n");
        writeMatrix(fwrite, n, L);
        fprintf(fwrite, " ############################################### \n\n");
        fprintf(fwrite, " Matrix U: \n\n");
        writeMatrix(fwrite, n, U);
        fprintf(fwrite, " ############################################### \n\n");
        fprintf(fwrite, " Matrix P: \n\n");
        writeMatrix(fwrite, n, P);
        fprintf(fwrite, " ############################################### \n\n");
        fclose(fwrite);
    }

    printf(" Matrix A: \n\n");
    printMatrix(n, A);
    printf(" ############################################### \n\n");
    printf(" Matrix L: \n\n");
    printMatrix(n, L);
    printf(" ############################################### \n\n");
    printf(" Matrix U: \n\n");
    printMatrix(n, U);
    printf(" ############################################### \n\n");
    printf(" Matrix P: \n\n");
    printMatrix(n, P);
    printf(" ############################################### \n\n");

    return 0;
}
