#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "matr_utilities.h"

/**
 *  @param max_size_of_row :  Maximum size of the Matrix to be tested
 *  @param only_this_size!=0 :  Tests randomized Matrix only of this size
 *  @paran only_this_size==0 : Generates randomized input Matrices of size o @param max_size_of_row 
 */

void generate_random_tests(size_t max_size_of_row, int only_this_size, char *output)
{

        FILE *out;
        if (output == NULL)
        {

                out = stdout;
        }
        else
        {
                out = fopen(output, "w");
                if (!out)
                {
                        perror("Error occurred while opening file for generating the randomized inputs for Testing");
                        exit(EXIT_FAILURE);
                }
        }
        size_t start = 1;
        if (only_this_size)
        {
                start = max_size_of_row;
                fprintf(out, "%ld\n", 4);
        }
        else
        {
                fprintf(out, "%ld\n", max_size_of_row * 4);
        }

        for (size_t k = start; k <= max_size_of_row; k++)
        {
                float *A;

                size_t size_of_matr = k * k;

                A = malloc(sizeof(float) * size_of_matr);

                if (!A)
                {
                        perror("Could not allocate Memory");
                        exit(EXIT_FAILURE);
                }
                float a;
                float b;
                float intervals[4][4] = {{0, 1}, {0, 10000}, {10000, 200000}, {400000, 400002}};

                int i = 0;
                while (i < 4)
                {
                        a = intervals[i][0];
                        b = intervals[i][1];
                        i++;
                        fprintf(out, "%ld\n", k);
                        matrix_generator_intervals(k, A, a, b);
                        writeMatrix(out, k, A);
                }

                free(A);
        }
        fclose(out);
}

void test_unit(void (*func)(size_t, const float *, float *, float *, float *), FILE *input, FILE *output, float tolerate)
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
                if (!print_result_without_solution(n, matrix, L, U, P, output, tolerate))
                {
                        tests_passed = 0;
                }
        }
        if (tests_passed)
        {
                fprintf(output, "<<All Tests Passsed>>\n\n\n");
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

void run_tests(void (*func)(size_t, const float *, float *, float *, float *), FILE *output, float test)
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
                test_unit(func, input, output, test);
                fclose(input);
        }
}
