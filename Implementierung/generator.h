#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "IO.h"
/**
 * Worst Case input for Pivoting
 */
void must_pivotize(size_t n, float *A)
{
        size_t offset = n;
        A[0] = 1;
        for (size_t i = 1; i < n * n; i++)
        {
                if (i % offset == 0)
                {
                        A[i] = 5.0;
                        offset += (n + 1);
                }
                else
                {
                        A[i] = 1.0;
                }
        }
}

/**
 * Generates Matrix with entries from 0 to range
 */
void matrix_generator(size_t n, float *A, float range)
{
        srand((unsigned int)time(NULL));
        for (size_t i = 0; i < n * n; i++)
                A[i] = ((float)rand() / (RAND_MAX)) * range;
}

/**
 * Generates Matrix with entries from 0 to range (negative numbers)
 */
void matrix_generator2(size_t n, float *A, float range)
{
        srand((unsigned int)time(NULL));
        for (size_t i = 0; i < n * n; i++)
        {
                A[i] = ((float)rand() / (RAND_MAX)) * range;
                int num = (int)A[i];
                A[i] = (num % 7 > 4 ? 1 : -1) * A[i];
        }
}

/**
 * Generates Matrix with entries from 0 to range (no fraction part & negatives)
 */
void matrix_generator3(size_t n, float *A, float range)
{
        srand((unsigned int)time(NULL));
        for (size_t i = 0; i < n * n; i++)
        {
                int num = (int)(((float)rand() / (RAND_MAX)) * range);
                A[i] = (num % 7 > 4 ? 1 : -1) * num;
        }
}

/**
 * Generates non-decomposable Matrices
 */

void not_decomposable1(size_t n, float *A)
{
        for (size_t i = 0; i < n * n; i++)
        {
                A[i] = 0;
        }
}

void not_decomposable2(size_t n, float *A)
{
        for (size_t i = 0; i < n * n; i++)
        {
                A[i] = 2.0;
        }
}

void not_decomposable3(size_t n, float *A)
{
        for (size_t i = 0; i < (n * n) / 2; i++)
        {
                A[i] = 3.0;
        }
        for (size_t i = (n * n) / 2; i < (n * n); i++)
        {
                A[i] = 0;
        }
}

void not_decomposable4(size_t n, float *A)
{
        for (size_t i = 0; i < (n * n) - 1; i++)
        {
                A[i] = i + 1;
        }
        for (size_t i = (n * n) - 1; i < (n * n); i++)
        {
                A[i] = i;
        }
}
/**
 * Generates multiple inputs for testing purposes
 */

#define TOLERATE_ERROR 2e-1
#define MUST_ALIGN_HEAP 800

void random_multiple_input(char *output)
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

        fprintf(out, "%ld\n", (long int) 65);

        float range = 1000.0;

        //Tests if the Heap Allocation happens accordingly
        float *C;

        C = malloc(sizeof(float) * MUST_ALIGN_HEAP * MUST_ALIGN_HEAP);

        if (!C)
        {
                perror("Could not allocate Memory");
                exit(EXIT_FAILURE);
        }
        fprintf(out, "%ld\n", (long int )MUST_ALIGN_HEAP);
        matrix_generator(MUST_ALIGN_HEAP, C, range);
        write_matrix_in_stream(out, MUST_ALIGN_HEAP, C);

        free(C);

        size_t next_size = 10;

        fprintf(out, "%ld\n", next_size);
        float B[next_size * next_size];
        not_decomposable1(next_size, B);
        write_matrix_in_stream(out, next_size, B);

        fprintf(out, "%ld\n", next_size);
        not_decomposable2(next_size, B);
        write_matrix_in_stream(out, next_size, B);

        fprintf(out, "%ld\n", next_size);
        not_decomposable3(next_size, B);
        write_matrix_in_stream(out, next_size, B);

        fprintf(out, "%ld\n", next_size);
        not_decomposable4(next_size, B);
        write_matrix_in_stream(out, next_size, B);

        range = 10000.0;
        for (size_t k = 1; k <= 600; k += 20)
        {
                float *D = malloc(sizeof(float) * k * k);

                if (!D)
                {
                        perror("Could not allocate Memory");
                        exit(EXIT_FAILURE);
                }
                fprintf(out, "%ld\n", k);

                matrix_generator2(k, D, range);

                write_matrix_in_stream(out, k, D);

                free(D);
        }

        for (size_t k = 1; k <= 600; k += 20)
        {
                float *D = malloc(sizeof(float) * k * k);

                if (!D)
                {
                        perror("Could not allocate Memory");
                        exit(EXIT_FAILURE);
                }

                fprintf(out, "%ld\n", k);
                matrix_generator3(k, D, range);
                write_matrix_in_stream(out, k, D);
                free(D);
        }

        fclose(out);
}

/**
 * Generates single random input
 */
void random_single_input(char *output, size_t n)
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

        float *A = malloc(sizeof(float) * n * n);

        if (!A)
        {
                perror("Could not allocate Memory");
                exit(EXIT_FAILURE);
        }
        float range = 10000.0;
        fprintf(out, "%ld\n", (long int) 1);
        fprintf(out, "%ld\n", n);
        matrix_generator(n, A, range);
        write_matrix_in_stream(out, n, A);
        fclose(out);
}

void generate_bench(char *bench_file)
{
        FILE *bench;
        {
                bench = fopen(bench_file, "w");
                if (!bench)
                {
                        perror("Error occurred while opening file for generating the randomized inputs for Testing");
                        exit(EXIT_FAILURE);
                }
        }
        fprintf(bench, "%ld\n",(long int) 1000 / 25);
        for (size_t i = 1; i < 1000; i += 50)
        {
                float *A;

                A = malloc(sizeof(float) * i * i);

                if (!A)
                {
                        perror("Could not allocate Memory");
                        exit(EXIT_FAILURE);
                }

                matrix_generator(i, A, 1000.0);
                fprintf(bench, "%ld\n", i);
                write_matrix_in_stream(bench, i, A);

                fprintf(bench, "%ld\n", i);
                must_pivotize(i, A);
                write_matrix_in_stream(bench, i, A);
                free(A);
        }

        fclose(bench);
}
