#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <sys/stat.h>


/**
 * TODO!
 */
void printHelp() {
    printf("-h/--help: Display brief summaries of command line Options\n");

    printf("-f: Specify input file\n");
    printf("-p: Deactivates pivoting Method."
           "No guarantee for correct answer");
    printf("-b: Activate Benchmarking \n");
    printf("-r: generate random Matrix \n"
           "Space seperated numbers : 1st Number -> Number of Matrices to be generated \n"
           "\n Other Numbers define sizes of particular Matrices"
           "\n Note : These Arguments are optional and their absence will cause randomizing "
           "\n only one Matrix");
    printf("-o: Choose Output stream \n");
    printf("-v: Choose Version of the Implementation\n");
    printf("-i : Choose number of iterations\n");
}

void writeMatrix(FILE *f, size_t n, const float *M) {
    for (size_t index = 0; index < n * n; index++) {
        fprintf(f, "%f,", M[index]);
        if ((index + 1) % n == 0)
            fprintf(f, "\n");
    }
}


/**
 *
 * Warning : Exception handling not implemented yet !!!
 *
 * @param n : size
 * @param fp
 * @param matrix
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
 *
 * Warning  : Exception Handling & Heap Allocation not yet implemented !!!
 *
 *@param out : pointer to output of the calculations : command line  / output file
 * This function uses @read_matrix_from_file(size_t,FILE,float) function to get multiple Matrices
 * from the specified @param input stream.
 */

#define LINE_SEPARATOR " ############################################### \n\n"

void multiple_input_from_file(FILE *input, FILE *output) {
    size_t num_of_matrices;
    fscanf(input, "%ld", &num_of_matrices);
    size_t size_of_matrices[num_of_matrices];
    for (size_t k = 0; k < num_of_matrices; k++) {
        fscanf(input, "%ld", size_of_matrices + k);
    }
    for (int i = 0; i < num_of_matrices; i++) {
        float matrix[size_of_matrices[i] * size_of_matrices[1]];
        read_matrix_from_file(size_of_matrices[i], input, matrix);
        float L[size_of_matrices[i]], U[size_of_matrices[i]], P[size_of_matrices[i]];
        ludecomp(size_of_matrices[i], matrix, L, U, P);
        fprintf(output, " Matrix A: \n\n");
        writeMatrix(output, size_of_matrices[i], matrix);
        fprintf(output, LINE_SEPARATOR);
        fprintf(output, " Matrix L: \n\n");
        writeMatrix(output, size_of_matrices[i], L);
        fprintf(output, LINE_SEPARATOR);
        fprintf(output, " Matrix U: \n\n");
        writeMatrix(output, size_of_matrices[i], U);
        fprintf(output, LINE_SEPARATOR);
        fprintf(output, " Matrix P: \n\n");
        writeMatrix(output, size_of_matrices[i], P);
        fprintf(output, LINE_SEPARATOR);
    }
}


int ioFunction(int argc, char **argv) {
    int opt;
    char c;

    size_t n;
    size_t pivot = 1;
    size_t randomMatrix = 0;


    char *input = NULL;
    char *output = NULL;
    char *random = NULL;

    size_t iterations = 1;
    static struct option long_options[] = {
            {"help", optional_argument, NULL, 'h'}};

    while ((opt = getopt_long(argc, argv, "o::bhpr::f:", long_options, NULL)) !=
           -1) {

        switch (opt) {
            case 'i' :
                iterations = strtoul(optarg, NULL, 0);
                break;
            case 'f':
                input = optarg;
                randomMatrix = 0;
                break;
            case 'r':
                randomMatrix = 1;
                if (optarg != 0) {
                    random = optarg;
                }
            case 'p':
                pivot = 0;
                break;
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
            case 'b':
                //TODO
                break;
            case 'o':
                if (optarg != 0) {
                    output = optarg;
                }
                break;
            default: /* '?' oder 'h' */
                fprintf(stderr, "Hilfe mit -h oder --help anzeigen");
                exit(EXIT_FAILURE);
        }
    }


    if (randomMatrix == 0) {
        if (input != NULL) {
            FILE *in = fopen(input, "r");
            if (output != NULL) {
                FILE *out = fopen(output, "w");
                multiple_input_from_file(in, out);
                fclose(out);
            } else {
                multiple_input_from_file(in, stdout);
            }
            fclose(in);

        } else {
            if (output != NULL) {
                FILE *out = fopen(output, "w");
                multiple_input_from_file(stdin, out);
                fclose(out);
            } else {
                multiple_input_from_file(stdin, stdout);
            }
        }
    } else {
        //TODO
        srandom(time(NULL));
        if (random == NULL) {
            n = (3 + rand() % 17);
        } else {
            FILE *in = fopen("random_input.txt", "w");
            fprintf(in, "%s", random);
        }
        if (output != NULL) {
            FILE *out = fopen(output, "w");
            float A[n * n];
            //matrixGenerator(n, A);
            multiple_input_from_file(stdin, out);
            fclose(out);
        } else {

        }
    }


    return 0;
}
