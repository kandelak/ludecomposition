#define _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <alloca.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>

/**
 * For Benchmarking the program
 */
static inline double curtime(void)
{
    struct timespec t;
    clock_gettime(1, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}



/**
 * Matrix multiplication
 */
void matrix_mul(size_t n, float *A, float *B, float *Res)
{
    size_t i, j, k;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            Res[i * n + j] = 0;
            for (k = 0; k < n; k++)
                Res[i * n + j] += A[i * n + k] * B[k * n + j];
        }
    }
}

/**
 * Tests if two matrices are equal based on the given error toleration
 */
int test_matrix_eq(size_t n, float *orgM, float *M, float tolerate)
{
    int res = 1;
    size_t matr_size = n * n;
    int print_max = 5;
    for (size_t i = 0; i < matr_size; i++)
    {
        if (fabsf(orgM[i] - M[i]) >= tolerate)
        {
            if (print_max-- > 0)
            {
                printf("Matrix entry: %ld,%ld\n", i / matr_size, i % n);
                printf("Expected: %f, Got: %f\n", orgM[i], M[i]);
            }
            res = 0;
        }
    }
    return res;
}

/**
 *  * Runs Benchmark using given function
 */
int run_bench(int (*func)(size_t, const float *, float *, float *, float *), FILE *output, float *A, float *L, float *U, float *P, size_t iterations, char *name, size_t i, size_t size_of_matr_row, int print)
{
    int decomposed = 1;

    fprintf(output, "%ldx%ld took (in Seconds) : ", size_of_matr_row, size_of_matr_row, i + 1);

    double start, end, time = 0;
    for (size_t k = 0; k < iterations; k++)
    {
        start = curtime();
        if (!func(size_of_matr_row, A, L, U, P))
        {
            decomposed = 0;
        };
        end = curtime();
        time += (end - start);
    }
    iterations = (double)iterations;
    time /= iterations;
    if (decomposed)
        fprintf(output, "%f\n", time);
    else
    {
        fprintf(output, "Can not be decomposed (using this implementation)\n");
    }
}


/**
 * Tests correctness of the decomposition
 */
int test_ludecomp(size_t n, float *A, float *L, float *U,
                  float *P, float tolerate)
{
    int res = 1;
    size_t size_of_matr = n * n;
    float *LxU = NULL;
    float *PxLxU = NULL;

    LxU = malloc(sizeof(float) * size_of_matr);
    if (!LxU)
    {
        perror("Could not allocate Memory for Testing\n\n");
        exit(EXIT_FAILURE);
    }
    PxLxU = malloc(sizeof(float) * size_of_matr);
    if (!PxLxU)
    {
        perror("Could not allocate Memory for Testing\n\n");
        free(LxU);
        exit(EXIT_FAILURE);
    }

    matrix_mul(n, L, U, LxU);
    matrix_mul(n, P, LxU, PxLxU);

    int test_PxLxU = test_matrix_eq(n, A, PxLxU, tolerate);

    if (!test_PxLxU)
    {
        res = 0;
    }

    free(LxU);
    free(PxLxU);
    return res;
}
