#include <limits.h>
#include "ludecomp.h"
#include "test.h"
#include <unistd.h>
#include <sys/resource.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <float.h>

void pivot_vs_no_pivot()
{
    FILE *example = fopen("pivot_example.txt", "r");
    if (!example)
    {
        perror("Unable to open file for reading.\n");
        exit(EXIT_FAILURE);
    }
    printf("Decomposing with pivoting..(See read-only file pivoting_example.txt)\n\n");
    size_t num_of_matrices;
    fscanf(example, "%ld", &num_of_matrices);

    size_t size_of_matr_row;

    for (size_t i = 0; i < num_of_matrices; i++)
    {
        fscanf(example, "%ld", &size_of_matr_row);
        run_on_heap("C", ludecomp, example, stdout, 0, 0, 1, 1, 1, size_of_matr_row);
    }
    printf("\n        As you can see this particular input is successfully decomposed using implementation with pivoting.\n\n"
           "        But if we use non-pivoting Implementation, this will happen:\n\n");
    freopen("pivot_example.txt", "r", example);
    if (!example)
    {
        perror("Unable to open file for reading.\n");
        exit(EXIT_FAILURE);
    }

    fscanf(example, "%ld", &num_of_matrices);
    for (size_t i = 0; i < num_of_matrices; i++)
    {

        fscanf(example, "%ld", &size_of_matr_row);
        run_on_heap("C without P", ludecomp_without_P, example, stdout, 0, 0, 1, 1, 1, size_of_matr_row);
    }

    printf("\n        Due to limited computation resources of the floating-point numbers larger rounding errors will occur while not pivoting.\n\n");
    fclose(example);
}

void printHelp()
{
    printf("--help/-h : Display brief summaries of command line Options\n\n");
    printf("--bench-all : Activates Benchmarking for pre-defined inputs\n\n"
           "       Example: ./main --bench-all\n\n");

    printf("--input/-f: Specify input stream\n\n"
           "       Example: ./main --input=input_file.txt\n"
           "       Example: ./main --input input_file.txt\n"
           "       Example: ./main -f=input_file.txt\n\n");
    printf("--no-print: Deactivates user-friendly printing. For instance for testing/benchmarking purposes or during computations on larger matrices.\n\n"
           "       Example: ./main --input=input_file.txt --no-print\n\n");
    printf("-b: Activates Benchmarking\n\n"
           "       Example: ./main --input=input_file.txt -b\n"
           "       Example: ./main --input=input_file.txt -b --no-print\n\n");
    printf("--output/-o: Chooses output stream for storing the results\n"
           "       Note: Standart output stream is the default output stream\n\n"
           "       Example: ./main --input=input_file.txt -b --output=results.txt\n\n");
    printf("-v: Chooses the implementation for the decomposition\n\n"
           "   Implementations:\n\n"
           "       0 : C\n"
           "       1 : C with intrinsics\n"
           "       2 : Assembler with SIMD\n"
           "       3 : Assembler\n"
           "       4 : C without Pivoting\n"
           "       Note: C is the default implementation\n\n"
           "       Example: ./main --input=input_file.txt --output=results.txt -v 2\n"
           "       Example: ./main -f input_file.txt -v 1 -b");
    printf("-i : Choose number of iterations for decomposition\n\n"
           "       Note: If benchmarking the average time is computed automatically\n\n"
           "       Example: ./main --input input_file.txt -o result.txt -b -v 3 -i 10\n\n");
    printf("-t/--test-all : Test pre-defined input from predefined file\n\n");
    printf("--random-test : Generates single random input with specified row/column size of the matrix\n\n"
           "       Example: ./main --random-test 500\n"
           "       This will generate 500x500 randomized matrix\n\n");
    printf("--random-tests : Generates multiple random inputs with specified specified parameter\n\n"
           "       Example: ./main --random-tests 500\n"
           "       This will generate multiple matrices with size from 1 till 500\n\n");
    printf("--bench-all : Generates temporary input for benchmarking and deletes it after completion of the benchmarking cycle\n\n"
           "       Example: ./main --bench-all\n"
           "       This will generate and benchmark multiple inputs with matrix row/column size varying from 1 till 1000\n\n");
    printf("--pivot/-p : Shows comparision example for decomposition with and without pivoting\n\n"
           "       Example: ./main --pivot\n\n");
}

int ludecomp_asm_simd(size_t n, float *, float *, float *, float *);

int ludecomp_asm(size_t n, float *, float *, float *, float *);

/**
 * Runs LU-Decomposition on the Stack
 */

void run_on_stack(char *name, int (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, int benchmarking, int print, size_t iterations, int testing, size_t i, size_t size_of_matr_row)
{

    size_t size_of_matr = size_of_matr_row * size_of_matr_row;

    float A[size_of_matr];

    float L[size_of_matr];

    float U[size_of_matr];

    float P[size_of_matr];

    int decomposed = 1;

    read_matrix_from_stream(size_of_matr_row, input, A);

    if (benchmarking && !testing)
    {
        if (!run_bench(func, output, A, L, U, P, iterations, name, i, size_of_matr_row, print))
        {
            decomposed = 0;
        };
    }
    else
    {
        for (int k = 0; k < iterations; k++)
        {
            if (!func(size_of_matr_row, A, L, U, P))
            {
                fprintf(output, "This Matrix can not be decomposed.\n");
                decomposed = 0;
            };
        }
    }

    if (testing & decomposed)
    {

        fprintf(output, "Testing...\n");
        if (!test_ludecomp(size_of_matr_row, A, L, U, P, TOLERATE_ERROR))
        {
            fprintf(output, "Test %ld failed.\n", i);
        }
        else
        {
            fprintf(output, "Test %ld succeeded.\n", i);
        }
    }

    if (print && decomposed)
    {
        print_pretty(output, A, L, U, P, size_of_matr_row, i);
    }
}

/**
 * Runs LU-decomposition on the heap
 */
void run_on_heap(char *name, int (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, int benchmarking, int print, size_t iterations, int testing, size_t i, size_t size_of_matr_row)
{
    size_t size_of_matr = size_of_matr_row * size_of_matr_row;

    float *A = NULL;
    float *L = NULL;
    float *P = NULL;
    float *U = NULL;
    int decomposed = 1;
    ;
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
        if (!run_bench(func, output, A, L, U, P, iterations, name, i, size_of_matr_row, print))
        {
            decomposed = 0;
        }
    }
    else
    {
        for (int k = 0; k < iterations; k++)
        {
            if (!func(size_of_matr_row, A, L, U, P))
            {
                decomposed = 0;
            };
        }
    }

    if (testing && decomposed)
    {
        fprintf(output, "Testing...\n");
        if (!test_ludecomp(size_of_matr_row, A, L, U, P, TOLERATE_ERROR))
        {
            fprintf(output, "Test %ld Failed.\n", i + 1);
        }
        else
        {
            fprintf(output, "Test %ld succeeded.\n", i + 1);
        }
    }

    if (print && decomposed)
    {
        print_pretty(output, A, L, U, P, size_of_matr_row, i);
    }
    free(P);
    free(A);
    free(L);
    free(U);
}

/**
 *  To choose different implementations
 */

struct implementation_version
{
    const char *name;

    int (*func)(size_t, const float *, float *, float *, float *);
};

typedef struct implementation_version implementation_version;

const implementation_version implementations[] = {
    {"c", ludecomp},
    {"c_intrinsics", ludecomp_intrinsics},
    {"asm_simd", ludecomp_asm_simd},
    {"asm", ludecomp_asm},
    {"c_no_P", ludecomp_without_P}};

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
    int version = 0;
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
        {"pivoting", no_argument, 0, 'p'},
    };

    int opt;

    while ((opt = getopt_long(argc, argv, "o:bphv:r:f:i:nts:", long_options, NULL)) !=
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
            bench_predefined = 1;
            benchmarking = 1;
            break;
        case 'p':
            pivot_vs_no_pivot();
            exit(EXIT_SUCCESS);
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
     * Benchmarking with predefined configuration
     */
    if (bench_predefined)
    {
        max_size_random_tests = 1000;
        print = 0;
        printf("Generating inputs for Benchmarking in temporary file (bench_tmp.txt)...\n");
        generate_random_tests(max_size_random_tests, 0, "bench_tmp.txt");
        printf("Generating done. (In file bench_tmp.txt)\n");
        input = "bench_tmp.txt";
    }

    /**
     * Predefined Testing
     */
    if (test_predefined)
    {
        //input = "testing/test.txt";
        testing = 1;
        print = 0;
    }

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
     * Handling Input and Output Streams 
     */

    FILE *in;
    FILE *out;

    if (input == NULL)
    {
        printf("Please specify the input file. Refer to --help/-h\n");
        exit(EXIT_FAILURE);
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

    printf("Calculating using %s Implementation...\n", impl->name);

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

    printf("Calculating done.\n");

    // Delete Temporary file
    if (bench_predefined)
    {
        if (remove("bench_tmp.txt") == -1)
        {
            perror("Could not remove temporary file.");
            fclose(in);
            fclose(out);
            exit(EXIT_FAILURE);
        }
    }

    fclose(in);
    fclose(out);

    return 0;
}
