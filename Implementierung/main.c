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
#include <sys/stat.h>
#include <sys/time.h>
#include <float.h>

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

void run_on_stack(char *name, void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, int benchmarking, int print, size_t iterations, int testing, size_t i, size_t size_of_matr_row)
{

    size_t size_of_matr = size_of_matr_row * size_of_matr_row;

    float A[size_of_matr];

    float L[size_of_matr];

    float U[size_of_matr];

    float P[size_of_matr];

    read_matrix_from_stream(size_of_matr_row, input, A);

    if (benchmarking && !testing)
    {
        run_bench(func, output, A, L, U, P, iterations, name, i, size_of_matr_row);
    }
    else
    {
        for (int k = 0; k < iterations; k++)
            func(size_of_matr_row, A, L, U, P);
    }

    if (testing)
    {

        fprintf(output, "Testing...\n");
        if (!print_result_without_solution(size_of_matr_row, A, L, U, P, output, print, TOLERATE_ERROR))
        {
            fprintf(output, "Test on Operation %ld failed.\n", i);
        }
        else
        {
            fprintf(output, "Test on Operation %ld succeeded.\n", i);
        }
    }

    if (print)
    {
        print_pretty(output, A, L, U, P, size_of_matr_row, i);
    }
}

void run_on_heap(char *name, void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, int benchmarking, int print, size_t iterations, int testing, size_t i, size_t size_of_matr_row)
{
    size_t size_of_matr = size_of_matr_row * size_of_matr_row;

    float *A = NULL;
    float *L = NULL;
    float *P = NULL;
    float *U = NULL;

    A = malloc(sizeof(float) * size_of_matr);
    if (!A)
    {
        perror("Could not allocate Memory");
        exit(EXIT_FAILURE);
    }
    L = malloc(sizeof(float) * size_of_matr);
    if (!L)
    {
        perror("Could not allocate Memory");
        free(A);
        exit(EXIT_FAILURE);
    }
    U = malloc(sizeof(float) * size_of_matr);
    if (!U)
    {
        perror("Could not allocate Memory");
        free(L);
        free(A);
        exit(EXIT_FAILURE);
    }
    P = malloc(sizeof(float) * size_of_matr);
    if (!P)
    {
        perror("Could not allocate Memory");
        free(A);
        free(L);
        free(U);
        exit(EXIT_FAILURE);
    }

    /**
     *  Read and compute next Matrix
    */

    read_matrix_from_stream(size_of_matr_row, input, A);

    if (benchmarking && !testing)
    {
        run_bench(func, output, A, L, U, P, iterations, name, i, size_of_matr_row);
    }
    else
    {
        for (int k = 0; k < iterations; k++)
        {
            func(size_of_matr_row, A, L, U, P);
        }
    }

    if (testing)
    {

        if (!print_result_without_solution(size_of_matr_row, A, L, U, P, output, print, TOLERATE_ERROR))
        {
            fprintf(output, "Test %ld Failed.\n", i + 1);
        }
        else
        {
            fprintf(output, "Test %ld Passed.\n", i + 1);
        }
    }

    if (print)
    {
        print_pretty(output, A, L, U, P, size_of_matr_row, i);
    }
    free(P);
    free(A);
    free(L);
    free(U);
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

    int test_predefined = 0;
    int generate_multiple_input = 0;
    int generate_single_input = 0;
    int benchmarking = 0;
    int bench_predefined = 0;
    int version = 0; //ludecomp is the default Version
    char *input = NULL;
    char *output = NULL;
    char *random = NULL;
    size_t max_size_random_tests = 0;
    int testing = 0;

    int print = 1;

    size_t iterations = 1;

    static struct option long_options[] = {
        {"help", optional_argument, 0, 'h'},
        {"input", required_argument, 0, 'f'},
        {"output", required_argument, 0, 'o'},
        {"bench-all", no_argument, 0, 'a'},
        {"no-print", no_argument, 0, 'n'},
        {"test-all", no_argument, 0, 't'},
        {"random-tests", required_argument, 0, 'r'},
        {"random-test", required_argument, 0, 's'},
    };

    int opt;

    while ((opt = getopt_long(argc, argv, "o:bhv:r:f:i:nts:", long_options, NULL)) !=
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
            testing = 1;
            break;

        default: /* '?' oder 'h' */
            fprintf(stderr, "Refer to --help/-h\n\n");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * Setting the chosen Implementation
     */
    if (version >= sizeof(implementations) / sizeof(implementation_version) || version < 0)
    {
        if (!output && max_size_random_tests > 20)
        {
            fprintf(stderr, "Please specify output file while generating big number of matrices\n\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "Invalid Implementation Specifier \nExiting..\n");
        exit(EXIT_FAILURE);
    }

    const implementation_version *impl = &implementations[version];

    const char *gen_file = "gen_file.txt";

    /**
     * Generating the random Inputs for Testing
     */
    if (generate_multiple_input)
    {
        testing = 1;
        printf("Generating randomized Inputs for Testing...\n");
        generate_random_tests(max_size_random_tests, 0, gen_file);
        printf("Generating done. (In file gen_file.txt)\n");
        exit(EXIT_SUCCESS);
    }
    else if (generate_single_input)
    {
        testing = 1;
        printf("Generating randomized Inputs for Testing...\n");
        generate_random_tests(max_size_random_tests, 1, gen_file);
        printf("Generating done. (In file gen_file.txt)\n");
        exit(EXIT_SUCCESS);
    }

    /**
     * Handling to Input and Output Streams 
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
     * Calculate and print pre-defined Benches
     */

    if (bench_predefined)
    {
        in = fopen("bench.txt", "r");
        if (!in)
        {
            perror("Error opening file for reading");
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc)
    {
        if (optind < argc - 1)
            fprintf(stderr, "%s: ignoring extra arguments\n", argv[0]);
    }

    /**
     * Checking Validity Of the Input
     */
    printf("Checking Validity of the Input...\n");
    if (!check_validity(in))
    {
        printf("Validity Check Failed.\n");
        fclose(in);
        fclose(out);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Input is Valid.\n");
        in = freopen(input, "r", in);
        if (!in)
        {
            perror("Error opening file for reading");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * Calculating Decompositions
     */
    size_t num_of_matrices;
    fscanf(in, "%ld", &num_of_matrices);

    size_t size_of_matr_row;

    for (size_t i = 0; i < num_of_matrices; i++)
    {
        fscanf(in, "%ld", &size_of_matr_row);

        if (size_of_matr_row > STACK_LIMIT)
        {
            run_on_heap(impl->name, impl->func, in, out, benchmarking, print, iterations, testing, i, size_of_matr_row);
        }
        else
        {
            run_on_stack(impl->name, impl->func, in, out, benchmarking, print, iterations, testing, i, size_of_matr_row);
        }
    }

    fclose(in);
    fclose(out);

    return 0;
}
