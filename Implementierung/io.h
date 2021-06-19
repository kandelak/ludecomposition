#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <sys/stat.h>

void printHelp() {
    printf("Program Usage explanation :\n");
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


/**
 * @param path  : Path to the file , from which numbers will be read
 * @param matrix : Matrix where the numbers will be written
 * @param n : row/column size of this particular Matrix
 * @return -1 if no More Matrices in this file. Every @int value besides -1
 * represents there are one or more matrices still in the file and
 * @return is the row/column length of the next matrix
 */


void read_matrix_from_file(size_t n, FILE *fp, float *matrix) {

    if (fp == NULL) {
        perror("Couldn't open the file");
        exit(EXIT_FAILURE);
    }

    size_t index = 0;

    size_t mat_size = n * n;

    while (mat_size > 0) {
        if (fscanf(fp, "%f", matrix + (index++)) == -1) {
            break;
        }
        mat_size--;
    }

    if (mat_size > (index - 1)) {
        printf("ERROR : Wrong number of entries for the Matrix for the specified size  \n"
               "Usage : \nfirst number is always the number of Matrices \n "
               "Then size of the each Matrix row/column \n"
               "All the other numbers are entries for the Matrices");
        exit(EXIT_FAILURE);
    }
}

/**
 *@param out : pointer to output of the calculations : command line (NULL) / output file
 */
void input_from_file(FILE *input, FILE *output, float *matrix) {
    size_t num_of_matrices;
    fscanf(input, "%ld", &num_of_matrices);
    size_t size_of_matrices[num_of_matrices];
    for (size_t k = 0; k < num_of_matrices; k++) {
        fscanf(input, "%ld", size_of_matrices + k);
    }
    for (int i = 0; i < num_of_matrices; i++) {
        read_matrix_from_file(size_of_matrices[i], input, matrix);
        printMatrix(size_of_matrices[i], matrix);
        printf("\n");
    }
}


int ioFunction(int argc, char **argv) {
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
                exit(EXIT_SUCCESS);
            case 't':
                runTests = 1;
                break;
            case 'b':
                break;
            case 'o':
                toFile = 2;
                output = optarg;
                break;

            default: /* '?' oder 'h' */
                fprintf(stderr, "Hilfe mit -h oder --help anzeigen");
                exit(EXIT_FAILURE);
        }
    }

    if (runTests == 1) {
        return tests();
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
        read_matrix_from_file(n, input, A);
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
