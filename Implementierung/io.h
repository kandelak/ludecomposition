#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <sys/stat.h>

/**
 * TODO! With Examples and better Explanation
 */
void printHelp()
{
    printf("-h/--help: Display brief summaries of command line Options\n");

    printf("-f: Specify input stream\n");
    printf("-p: Deactivates pivoting Method."
           "No guarantee for correct answer");
    printf("-b: Activate Benchmarking Mode (Functional Mode is Default)\n");
    printf("-r: generate random Matrix \n"
           "Space seperated numbers : \n"
           "1st Number : Number of Matrices to be generated \n"
           "Other Numbers define sizes of particular Matrices\n"
           "Note : These Arguments are optional and their absence will cause randomizing\n"
           "Only one Matrix\n"
           "P.S : This Option works only with Benchmarking mode");

    printf("-o: Choose Output stream \n");
    printf("-v: Choose Version of the Implementation\n");
    printf("-i : Choose number of iterations\n");
}

void writeMatrix(FILE *f, size_t n, const float *M)
{
    for (size_t index = 0; index < n * n; index++)
    {
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
void read_matrix_from_stream(size_t n, FILE *fp, float *matrix)
{

    if (fp == NULL)
    {
        perror("Couldn't open the file");
        exit(EXIT_FAILURE);
    }

    size_t index = 0;

    size_t mat_size = n * n;

    while (mat_size > 0)
    {
        if (fscanf(fp, "%f", matrix + (index++)) == -1)
        {
            break;
        }
        mat_size--;
    }

    if (mat_size > (index - 1))
    {
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

void run(void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output)
{
    size_t num_of_matrices;
    fscanf(input, "%lu", &num_of_matrices);
    size_t size_of_matrices[num_of_matrices];
    for (size_t k = 0; k < num_of_matrices; k++)
    {
        fscanf(input, "%lu", size_of_matrices + k);
    }
    for (size_t i = 0; i < num_of_matrices; i++)
    {
        float matrix[size_of_matrices[i] * size_of_matrices[1]];
        read_matrix_from_stream(size_of_matrices[i], input, matrix);
        float L[size_of_matrices[i]], U[size_of_matrices[i]], P[size_of_matrices[i]];
        func(size_of_matrices[i], matrix, L, U, P);
        fprintf(output, "Operation %lu: \n\n", i + 1);
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

int ioFunction(void (*func)(size_t, const float *, float *, float *, float *), int argc, char **argv)
{
    int opt;
    char c;

    size_t n;
    int pivoting = 1;
    int randomizing_Matrix = 0;
    int benchmarking = 0;
    char *input = NULL;
    char *output = NULL;
    char *random = NULL;

    size_t iterations = 1;
    static struct option long_options[] = {
        {"help", optional_argument, NULL, 'h'}};

    while ((opt = getopt_long(argc, argv, "o:bhpr::f:", long_options, NULL)) !=
           -1)
    {

        switch (opt)
        {
        case 'i':
            iterations = strtoul(optarg, NULL, 0);
            break;
        case 'f':
            input = optarg;
            break;
        case 'r':
            randomizing_Matrix = 1;
            if (optarg != 0)
            {
                random = optarg;
            }
        case 'p':
            pivoting = 0;
            break;
        case 'h':
            printHelp();
            exit(EXIT_SUCCESS);
        case 'b':
            benchmarking = 1;
            break;
        case 'o':
            //if (optarg != 0)
            //{
            output = optarg;
            // }
            break;
        default: /* '?' oder 'h' */
            fprintf(stderr, "Hilfe mit -h oder --help anzeigen");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * Handles to Input and Output Streams 
     */

    FILE *in;
    FILE *out;

    if (input == NULL)
    {
        in = stdin;
    }
    else
    {
        in = fopen(input, "r");
    }
    if (output == NULL)
    {
        out = stdout;
    }
    else
    {
        out = fopen(output, "w+");
    }

    /**
     * Functional Mode is on 
    */
    if (!benchmarking)
    {
        if (randomizing_Matrix)
        {
            printf("Randomizing works only with Benchmarking Mode\n"
                   "Please refer to --help/-h");
            exit(EXIT_FAILURE);
        }
        // For NULL will come the specified function
        run(func, in, out);
    }

    /**
     * Benchmarking Mode is on
    */
    else
    {
        if (randomizing_Matrix)
        {
        }
    }

    fclose(in);
    fclose(out);

    return 0;
}
