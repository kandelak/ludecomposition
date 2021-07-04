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
                fprintf(out, "%ld\n", 1);
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
                float intervals[4][4] = {{0, 1}, {-789, -2}, {10000, 200000}, {400000, 400002}};

                int i = 0;
                // while (i < 4)
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

void run_tests(char *name, void (*func)(size_t, const float *, float *, float *, float *), char *input, char *output, float tolerate)
{

        FILE *in;
        {
                in = fopen(input, "r");
                if (!in)
                {
                        perror("Error occurred while opening file for generating the randomized inputs for Testing");
                        exit(EXIT_FAILURE);
                }
        }
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
        fprintf(out, "Implementation : %s\n", name);
        size_t num_of_matrices;
        fscanf(in, "%ld", &num_of_matrices);
        size_t n;

        int tests_passed = 1;
        for (size_t i = 0; i < num_of_matrices; i++)
        {
                fscanf(in, "%ld", &n);
                size_t size_of_matr = n * n;

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

                read_matrix_from_stream(n, in, A);
                func(n, A, L, U, P);
                if (!print_result_without_solution(n, A, L, U, P, out, tolerate))
                {
                        fprintf(out, "Test %ld Failed.\n", i + 1);
                        tests_passed = 0;
                }
                else
                {
                        fprintf(out, "Test %ld Passed.\n", i + 1);
                }
                free(A);
                free(L);
                free(U);
                free(P);
        }

        if (tests_passed)
        {
                fprintf(out, "\n<<All Tests Passsed>>\n");
        }

        fprintf(out, "\n");

        fclose(in);
        fclose(out);
}
