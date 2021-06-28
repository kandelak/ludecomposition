#include "ludecomp.h"
#include "test.h"
#include <unistd.h>
#include "ludecomp_intrinsics.h"
#include "ludecomp_2.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <sys/stat.h>
/**
 * For Benchmarking the program
 */
static inline double curtime(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

/**
 * For choosing Implementation 
 */

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

/**
 *
 * Warning  : Exception Handling & Heap Allocation not yet implemented !!!
 *
 *@param out : pointer to output of the calculations : command line  / output file
 * This function uses @read_matrix_from_file(size_t,FILE,float) function to get multiple Matrices
 * from the specified @param input stream.
 */

#define LINE_SEPARATOR " \n############################################### \n\n"

void run(char *name, void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output)
{
    size_t num_of_matrices;
    fscanf(input, "%ld", &num_of_matrices);
    size_t size_of_current_matr;

    for (size_t i = 0; i < num_of_matrices; i++)
    {
        fscanf(input, "%ld", &size_of_current_matr);
        float matrix[size_of_current_matr * size_of_current_matr];
        read_matrix_from_stream(size_of_current_matr, input, matrix);
        float L[size_of_current_matr * size_of_current_matr], U[size_of_current_matr * size_of_current_matr];
        
        float P[size_of_current_matr * size_of_current_matr];
    
        func(size_of_current_matr, matrix, L, U, P);
        fprintf(output, "Operation %lu: \n\n", i + 1);
        fprintf(output, " Matrix A: \n\n");
        writeMatrix(output, size_of_current_matr, matrix);

        fprintf(output, " Matrix L: \n\n");
        writeMatrix(output, size_of_current_matr, L);

        fprintf(output, " Matrix U: \n\n");
        writeMatrix(output, size_of_current_matr, U);

        fprintf(output, " Matrix P: \n\n");
        writeMatrix(output, size_of_current_matr, P);
        fprintf(output, LINE_SEPARATOR);
    }
}

struct implementation_version
{
    const char *name;

    void (*func)(size_t, const float *, float *, float *, float *);
    void (*func_without_P)(size_t, const float *, float *, float *);
};

typedef struct implementation_version implementation_version;

const implementation_version implementations[] = {
    {"c", ludecomp, ludecomp_without_P},
    {"c_intrinsics", ludecomp_intrinsics, ludecomp_without_P_intrinsics},
    {"c_dolittle", ludecomp_2, NULL},
};

int main(int argc, char **argv)
{

    if (argc <= 1)
    {
        printf("Invalid size of operands. Please Refer to --help/-h\n\n");
        exit(EXIT_FAILURE);
    }

    int pivoting = 1;
    int randomizing_Matrix = 0;
    int benchmarking = 0;
    int version = 0; //ludecomp is the default Version
    char *input = NULL;
    char *output = NULL;
    char *random = NULL;

    size_t iterations = 1;

    static struct option long_options[] = {
        {"help", optional_argument, NULL, 'h'}};

    int opt;

    while ((opt = getopt_long(argc, argv, "o:bhpv:r::f:", long_options, NULL)) !=
           -1)
    {

        switch (opt)
        {
        case 'i':
            iterations = atoi(optarg);
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
        case 'v':
            version = atoi(optarg);
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
            fprintf(stderr, "Refer to --help/-h\n\n");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * Checks and sets the path to the Input File 
     */
    if (optind < argc)
    {
        input = argv[optind];
        if (optind < argc - 1)
            fprintf(stderr, "%s: ignoring extra arguments\n", argv[0]);
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
        out = fopen(output, "w");
    }

    const implementation_version *impl = &implementations[version];

    /**
     * Functional Mode is on 
    */
    if (!benchmarking)
    {
        if (randomizing_Matrix)
        {
            printf("You are currently in Functional Mode of the Program\n"
                   "Randomizing works only with Benchmarking Mode\n"
                   "Please refer to --help/-h");
            exit(EXIT_FAILURE);
        }

        while (iterations > 0)
        {
            if (!pivoting)
                run(impl->name, impl->func_without_P, in, out);
            else
                run(impl->name, impl->func, in, out);
            iterations--;
        }
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
