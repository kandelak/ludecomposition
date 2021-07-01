#include <limits.h>
#include "ludecomp.h"
#include "test.h"
#include <unistd.h>
#include "ludecomp_intrinsics.h"
#include <sys/resource.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <float.h>

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
    printf("--output: Choose Output stream \n (use -o alternatively)");
    printf("-v: Choose Version of the Implementation\n");
    printf("-i : Choose number of iterations\n");
    printf("-t/--test-all : Test pre-defined input from predefined file\n");
    printf("-r/--generate : Test randomly matrices with specified max_size\n");
}

void ludecomp_asm2(size_t n, float *, float *, float *, float *);

void ludecomp_asm(size_t n, float *, float *, float *, float *);

/**
 *
 * Warning  : Exception Handling & Heap Allocation not yet implemented !!!
 *
 *@param out : pointer to output of the calculations : command line  / output file
 * This function uses @read_matrix_from_file(size_t,FILE,float) function to get multiple Matrices
 * from the specified @param input stream
 */

void run_on_stack(char *name, void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, int benchmarking, int print, int iterations, size_t i, size_t size_of_matr_row)
{

    size_t size_of_matr = size_of_matr_row * size_of_matr_row;

    float A[size_of_matr];

    float L[size_of_matr];

    float U[size_of_matr];

    float P[size_of_matr];

    read_matrix_from_stream(size_of_matr_row, input, A);

    if (benchmarking)
    {
        run_bench(func, output, A, L, U, P, iterations, name, i, size_of_matr_row);
    }
    else
    {
        func(size_of_matr_row, A, L, U, P);
    }
    
    if (print)
    {
        print_pretty(output, A, L, U, P, size_of_matr_row, i);
    }
}

void run_on_heap(char *name, void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, int benchmarking, int print, int iterations, size_t i, size_t size_of_matr_row)
{
    size_t size_of_matr = size_of_matr_row * size_of_matr_row;

    float *A = malloc(sizeof(float) * size_of_matr);
    float *L = malloc(sizeof(float) * size_of_matr);
    float *U = malloc(sizeof(float) * size_of_matr);
    float *P = malloc(sizeof(float) * size_of_matr);

    if (!A || !L || !U || !P)
    {
        perror("Could not allocate Memory");
        exit(EXIT_FAILURE);
    }

    /**
     *  Read and compute next Matrix
    */

    read_matrix_from_stream(size_of_matr_row, input, A);

    if (benchmarking)
    {
        run_bench(func, output, A, L, U, P, iterations, name, i, size_of_matr_row);
    }
    else
    {
        func(size_of_matr_row, A, L, U, P);
    }

    if (print)
    {
        print_pretty(output, A, L, U, P, size_of_matr_row, i);
    }

    free(A);
    free(L);
    free(U);
    free(P);
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
    {"asm_simd", ludecomp_asm2},
    {"asm", ludecomp_asm}};

int main(int argc, char **argv)
{

    if (argc <= 1)
    {
        printf("Invalid size of operands. Please Refer to --help/-h\n\n");
        exit(EXIT_FAILURE);
    }

    int pivoting = 1;
    int test_predefined = 0;
    int generate_multiple_input = 0;
    int generate_single_input = 0;
    int a = 0;
    int benchmarking = 0;
    int bench_predefined = 0;
    int version = 0; //ludecomp is the default Version
    char *input = NULL;
    char *output = NULL;
    char *random = NULL;
    size_t max_size_random_tests = 0;

    int print = 1;

    size_t iterations = 1;

    static struct option long_options[] = {
        {"help", optional_argument, 0, 'h'},
        {"input", required_argument, 0, 'f'},
        {"output", required_argument, 0, 'o'},
        {"bench-all", no_argument, 0, 'a'},
        {"no-print", no_argument, 0, 'n'},
        {"test-all", no_argument, 0, 't'},
        {"generate", required_argument, 0, 'r'},
        {"generate-single-size", required_argument, 0, 's'},
    };

    int opt;

    while ((opt = getopt_long(argc, argv, "o:bhpv:r:f:i:nts:", long_options, NULL)) !=
           -1)
    {

        switch (opt)
        {
        case 'i':
            iterations = strtol(optarg, NULL, 10);
            break;
        case 'r':
            generate_multiple_input = 1;
            max_size_random_tests = strtol(optarg, NULL, 10);
            break;
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
        case 's':
            max_size_random_tests = strtol(optarg, NULL, 10);
            generate_single_input = 1;
            break;
        case 't':
            test_predefined = 1;
            break;

        default: /* '?' oder 'h' */
            fprintf(stderr, "Refer to --help/-h\n\n");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * If Generating the random Inputs for Testing
     */
    if (generate_multiple_input)
    {
        if (!output && max_size_random_tests > 20)
        {
            fprintf(stderr, "Please specify output file while generating big number of matrices\n\n");
            exit(EXIT_FAILURE);
        }
        printf("Generating randomized Inputs for Testing...\n");
        generate_random_tests(max_size_random_tests, 0, output);
        printf("Generating done.\n");
        exit(EXIT_SUCCESS);
    }
    else if (generate_single_input)
    {
        printf("Generating randomized Inputs for Testing...\n");
        generate_random_tests(max_size_random_tests, 1, output);
        printf("Generating done.\n");
        exit(EXIT_SUCCESS);
    }

    if (version >= sizeof(implementations) / sizeof(implementation_version) || version < 0)
    {
        if (!output && max_size_random_tests > 20)
        {
            fprintf(stderr, "Please specify output file while generating big number of matrices\n\n");
            exit(EXIT_FAILURE);
        }
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

    /**
     * Calculate and print pre-defined Benches
     */

    if (bench_predefined)
    {
    }

    if (optind < argc)
    {
        if (optind < argc - 1)
            fprintf(stderr, "%s: ignoring extra arguments\n", argv[0]);
    }

    /**
     * Calculating Decompositions
     */
    size_t num_of_matrices;
    fscanf(in, "%ld", &num_of_matrices);

    size_t size_of_matr_row;
    size_t size_of_matr;
    for (size_t i = 0; i < num_of_matrices; i++)
    {
        fscanf(in, "%ld", &size_of_matr_row);

        size_of_matr = size_of_matr_row * size_of_matr_row;

        if (size_of_matr_row > STACK_LIMIT)
        {
            printf("da\n");
            run_on_heap(impl->name, impl->func, in, out, benchmarking, print, iterations, i, size_of_matr_row);
        }
        else
        {
            run_on_stack(impl->name, impl->func, in, out, benchmarking, print, iterations, i, size_of_matr_row);
        }
    }

    //run(impl->name, impl->func, in, out, benchmarking, print, iterations);

    fclose(in);
    fclose(out);

    return 0;
}
