#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include "ludecomp.h"
#include "test.h"
#include <unistd.h>
#include "ludecomp_intrinsics.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <sys/stat.h>
#include <float.h>
#include <alloca.h>

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
 * Checks validity of the input
 * 
 */
int check_validity(FILE *input)
{
    char buf[50];
    size_t is_long = 1;
    char *endptr;
    size_t cnt = 0;
    long val_long;
    float val_float;
    while (fgets(buf, 50, input) != NULL)
    {
        int len = strlen(buf);
        // Max Length of the number representation
        if (len > 22)
        {
            return 0;
        }
        //Check if it is a number
        errno = 0;
        // If Number Must be Integer
        if (is_long == 1)
        {
            val_long = strtol(buf, &endptr, 10);
            if ((errno == ERANGE && (val_long == LONG_MAX || val_long == LONG_MIN)) || (errno != 0 && val_long == 0))
            {
                return 0;
            }
        }
        //If Number Must be Float
        else
        {
            val_float = strtod(buf, &endptr);
            if ((errno == ERANGE && (val_float == HUGE_VALF || val_float == HUGE_VALL)) || (errno != 0 && val_long == 0))
            {
                return 0;
            }
        }

        if (endptr == buf)
        {
            fprintf(stderr, "Conversion Error Occurred on %s: ", buf);
            return 0;
        }
    }

    return 1;
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
    printf("--bench-all : Activates Benchmarking for pre-defined inputs");

    printf("--input: Specify input stream\n (use --input alternatively)");
    printf("--no-print: Tell Program not to Print results of the LU decomposition");
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

    printf("--output: Choose Output stream \n (use -o alternatively)");
    printf("-v: Choose Version of the Implementation\n");
    printf("-i : Choose number of iterations\n");
}

void ludecomp_asm2(size_t n, float *, float *, float *, float *);

/**
 *
 * Warning  : Exception Handling & Heap Allocation not yet implemented !!!
 *
 *@param out : pointer to output of the calculations : command line  / output file
 * This function uses @read_matrix_from_file(size_t,FILE,float) function to get multiple Matrices
 * from the specified @param input stream
 */

#define LINE_SEPARATOR " \n############################################### \n\n"
#define MAX_STACK_MEM 8000000
int run(char *name, void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, int benchmarking, int print, int iterations)
{

    size_t num_of_matrices;
    fscanf(input, "%ld", &num_of_matrices);
    size_t size_of_matr_line;
    size_t size_of_matr;
    for (size_t i = 0; i < num_of_matrices; i++)
    {
        fscanf(input, "%ld", &size_of_matr_line);
        size_of_matr = size_of_matr_line * size_of_matr_line;

        /**
         * Handling the Heap Allocation if needed
         */
        int allocated = 0;
        float *A;
        float *L;
        float *U;
        float *P;
        if (size_of_matr * 4 * 4 >= MAX_STACK_MEM)
        {
            allocated = 1;
            A = malloc(sizeof(float) * size_of_matr);
            L = malloc(sizeof(float) * size_of_matr);
            U = malloc(sizeof(float) * size_of_matr);
            P = malloc(sizeof(float) * size_of_matr);
            if (!A || !L || !U || !P)
            {
                fprintf(stderr, "Could not allocate Memory for the Operation %d : ", i + 1);
            }
        }
        else
        {

            A = alloca(size_of_matr * sizeof(float));
            L = alloca(size_of_matr * sizeof(float));
            U = alloca(size_of_matr * sizeof(float));
            P = alloca(size_of_matr * sizeof(float));
        }

        read_matrix_from_stream(size_of_matr_line, input, A);

        if (benchmarking)
        {
            double start, end;
            fprintf(output, "%s Implementation on Operation %ld took (in Seconds) : \n", name, i + 1);
            for (int k = 0; k < iterations; k++)
            {
                start = curtime();
                func(size_of_matr_line, A, L, U, P);
                end = curtime();
                fprintf(output, "%f\n", end - start);
            }
        }
        else
        {

            func(size_of_matr_line, A, L, U, P);
        }

        if (print)
        {
            fprintf(output, "\nOperation %ld: \n\n", i + 1);
            fprintf(output, " Matrix A: \n\n");
            writeMatrix(output, size_of_matr_line, A);
            fprintf(output, " Matrix L: \n\n");
            writeMatrix(output, size_of_matr_line, L);

            fprintf(output, " Matrix U: \n\n");
            writeMatrix(output, size_of_matr_line, U);

            fprintf(output, " Matrix P: \n\n");
            writeMatrix(output, size_of_matr_line, P);
            fprintf(output, LINE_SEPARATOR);

            printResultWithoutSolution(size_of_matr_line, A, L, U, P, output);
        }
    }

    return 1;
}

struct implementation_version
{
    const char *name;

    void (*func)(size_t, const float *, float *, float *, float *);
};

/**
 *  Tool to Choose different implementations
 */
typedef struct implementation_version implementation_version;

const implementation_version implementations[] = {
    {"c", ludecomp},
    {"c_intrinsics", ludecomp_intrinsics},
    {"asm", ludecomp_asm2}};

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
    int bench_predefined = 0;
    int version = 0; //ludecomp is the default Version
    char *input = NULL;
    char *output = NULL;
    char *random = NULL;
    int print = 1;
    int testing = 0;
    size_t iterations = 1;

    static struct option long_options[] = {
        {"help", optional_argument, 0, 'h'},
        {"input", required_argument, 0, 'f'},
        {"output", required_argument, 0, 'o'},
        {"bench-all", no_argument, 0, 'a'},
        {"no-print", no_argument, 0, 'n'}};

    int opt;

    while ((opt = getopt_long(argc, argv, "o:bhpv:r:f:i:n", long_options, NULL)) !=
           -1)
    {

        switch (opt)
        {
        case 'i':
            iterations = strtol(optarg, NULL, 10);
            break;
        case 'r':
            randomizing_Matrix = 1;
        case 'p':
            pivoting = 0;
            break;
        case 'v':
            version = strtol(optarg, NULL, 10);
            break;
        case 'h':
            printHelp();
            exit(EXIT_SUCCESS);
        case 'b':
            benchmarking = 1;
            break;
        case 'o':
            output = optarg;
            break;
        case 'f':
            input = optarg;
            break;
        case 'a':
            input = "bench.txt";
            bench_predefined = 1;
            benchmarking = 1;
            break;
        case 'n':
            print = 0;
            break;

        default: /* '?' oder 'h' */
            fprintf(stderr, "Refer to --help/-h\n\n");
            exit(EXIT_FAILURE);
        }
    }

    if (version >= sizeof(implementations) / sizeof(implementation_version) || version < 0)
    {
        fprintf(stderr, "%s: Invalid Implementation Specifier %u\n");
        exit(EXIT_FAILURE);
    }

    /**
     * Handling to Input and Output Streams 
     */

    FILE *in;
    FILE *out;

    if (input == NULL)
    {
        if (bench_predefined)
        {
            in = fopen("bench.txt", "r");
            if (!in)
            {
                perror("Error opening file for reading");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            in = stdin;
        }
    }
    else
    {
        in = fopen(input, "r");
        if (!in)
        {
            perror("Error opening file for reading");
            exit(EXIT_FAILURE);
        }
    }
    if (output == NULL)
    {
        out = stdout;
    }
    else
    {
        out = fopen(output, "w");
        if (!out)
        {
            perror("Error opening file for writing");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * Setting the chosen Implementation
     */

    const implementation_version *impl = &implementations[version];

    if (bench_predefined)
    {
    }

    if (optind < argc)
    {
        if (optind < argc - 1)
            fprintf(stderr, "%s: ignoring extra arguments\n", argv[0]);
    }

    if (randomizing_Matrix && (benchmarking || !testing))
    {
        printf("You are currently in Benchmarking/Functional Mode of the Program\n"
               "Randomizing works only with Testing\n"
               "Please refer to --help/-h\n\n");
        exit(EXIT_FAILURE);
    }

    run(impl->name, impl->func, in, out, benchmarking, print, iterations);

    fclose(in);
    fclose(out);

    return 0;
}
