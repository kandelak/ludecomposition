#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "matr_utilities.h"

void test_unit(void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output)
{
        size_t num_of_matrices;
        fscanf(input, "%ld", &num_of_matrices);
        size_t n;
        size_t size;

        int tests_passed = 1;

        for (size_t i = 0; i < num_of_matrices; i++)
        {
                fscanf(input, "%ld", &n);
                size = n * n;
                float matrix[size];
                read_matrix_from_stream(n, input, matrix);
                float L[size], U[size], P[size];
                func(n, matrix, L, U, P);
                if (!printResultWithoutSolution(n, matrix, L, U, P, output))
                {
                        tests_passed = 0;
                }
        }
        if (tests_passed)
        {
                fprintf(output, "\033[0;32mAll Tests Passsed\n\n\n");
        }
}

#define NUM_OF_TEST_CLASSES 5

struct test_classes
{
        char *name;
        char *file;
};

typedef struct test_classes test_classes;

const test_classes tests[] = {
    {"Boundary Conditions", "testing/boundary_testing.txt"},
    {"Input/Output Correctness", "testing/io_testing.txt"},
    {"Memory Handling", "testing/memory_testing.txt"},
    {"Randomized Inputs", "testing/random_testing.txt"},
    {"Rounding Errors", "testing/rounding_error_testing.txt"},
};

void run_tests(void (*func)(size_t, const float *, float *, float *, float *), FILE *output)
{
        FILE *input;
        int cnt = NUM_OF_TEST_CLASSES;

        while (cnt-- > 0)
        {
                input = fopen(tests[cnt].file, "r");
                if (!input)
                {
                        perror("Could't open text file for testing input");
                        exit(EXIT_FAILURE);
                }
                if (!output)
                {
                        perror("Couldn't open text file for writing the testing results");
                        exit(EXIT_FAILURE);
                }
                fprintf(output, "Testing the program for %s...\n\n", tests[cnt].name);
                test_unit(func, input, output);
                fclose(input);

                //Resetting the Color
                printf("\033[0m");
        }
}
