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
 * Checks validity of the input
 */

int check_validity(FILE *input)
{
    char buf[50];

    size_t num_of_matr;

    if (!fgets(buf, 50, input))
    {
        perror("Reading Input Failed.\n");
        return 0;
    }

    errno = 0;

    char *endptr;

    num_of_matr = strtol(buf, &endptr, 10);

    if (errno == ERANGE)
    {
        perror("Integer Number read is too small or too large.\n");
        return 0;
    }
    else if (endptr == buf)
    {
        printf("Nothing Read (No input for the number of the matrices)\n");
        return 0;
    }
    else if (*endptr && *endptr != '\n')
    {
        printf("Wrong Input Format (Number of matrices). Please refer to --help/-h.\n");
        return 0;
    }
    else if ((int)num_of_matr <= 0)
    {
        printf("Number of Matrices can not be less than or equal 0.\n");
        return 0;
    }

    for (size_t k = 0; k < num_of_matr; k++)
    {
        if (!fgets(buf, 50, input))
        {
            perror("Reading Input Failed.\n");
            return 0;
        }

        size_t size_of_matr_row = strtol(buf, &endptr, 10);

        if (errno == ERANGE)
        {
            perror("Integer number read was too small or too large.\n");
            return 0;
        }
        else if (endptr == buf)
        {
            printf("Nothing Read (No input for the Size of the Matrix)\n");
            return 0;
        }
        else if (*endptr && *endptr != '\n')
        {
            printf("Wrong Input Format (Size of the matrix row). Please refer to --help/-h.\n");
            return 0;
        }
        else if ((int)size_of_matr_row <= 0)
        {
            printf("Size of the Matrix can not be less than or equal zero.\n");
            return 0;
        }

        size_t size_of_matr = size_of_matr_row * size_of_matr_row;
        for (size_t i = 0; i < size_of_matr; i++)
        {
            if (!fgets(buf, 50, input))
            {
                perror("Reading Input Failed.\n");
                return 0;
            }

            float matr_entry = strtod(buf, &endptr);

            if (errno == ERANGE)
            {
                perror("Floating-point Number read was too small or too large.\n");
                return 0;
            }
            else if (endptr == buf)
            {
                printf("Nothing Read (Not enough entries for specified size of the Matrix)\n");
                return 0;
            }
            else if (*endptr && *endptr != '\n')
            {

                printf("Wrong Input Format (Matrix Entry). Please refer to --help/-h.\n");

                return 0;
            }
        }
    }

    return 1;
}

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
 * Writes Matrix M in the stream specified with out (Used for generating input file)
 */
void writeMatrix(FILE *out, size_t n, const float *M)
{
    for (size_t index = 0; index < n * n - 1; index++)
    {
        fprintf(out, "%f\n", M[index]);
    }
    fprintf(out, "%f\n", M[n * n - 1]);
}
/**
 * Writes Matrix M in the stream specified with out (Used for representing matrices to User)
 */
void write_pretty(FILE *out, size_t n, const float *M)
{
    for (size_t index = 0; index < n * n - 1; index++)
    {
        fprintf(out, "%f ", M[index]);
        if ((index + 1) % n == 0)
            fprintf(out, "\n");
    }
    fprintf(out, "%f\n\n", M[n * n - 1]);
}

#define STACK_LIMIT 700

/**
 * Reads matrix entries from the stream specified with fp and writes in matrix
 */
void read_matrix_from_stream(size_t n, FILE *fp, const float *matrix)
{

    size_t index = 0;

    size_t mat_size = n * n;

    while (mat_size > 0)
    {
        if (fscanf(fp, "%f", matrix + (index++)) == -1)
        {
            break;
        }
        mat_size--;
    }
}

/**
 * Basic matrix multiplication
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
 * Prints Error Matrix if the tolerated error is exceeded
 */
void print_error_matrix(size_t n, float *orgM, float *M, FILE *output, float tolerate)
{
    for (size_t i = 0; i < n * n; i++)
    {
        if (fabsf(orgM[i] - M[i]) >= tolerate)
        {
            fprintf(output, "<<%f (!=%f)>> ", M[i], orgM[i]);
        }
        else
            fprintf(output, "%f ", M[i]);
        if ((i + 1) % n == 0)
            fprintf(output, "\n");
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

#define LINE_SEPARATOR " \n############################################### \n\n"

/**
 * Runs Benchmark using given function
 */
void run_bench(void (*func)(size_t, const float *, float *, float *, float *), FILE *output, float *A, float *L, float *U, float *P, size_t iterations, char *name, size_t i, size_t size_of_matr_row, int print)
{

    if (print)
    {
        fprintf(output, "Operation %ld took (in Seconds) : \n",i + 1);
    }
    double start, end, time = 0;
    for (size_t k = 0; k < iterations; k++)
    {
        start = curtime();
        func(size_of_matr_row, A, L, U, P);
        end = curtime();
        time += (end - start);
    }
    iterations = (double)iterations;
    time /= iterations;
    fprintf(output, "%f\n", time);
}

/**
 * Writes Decomposition matrices on the stream specified with output
 */
void print_pretty(FILE *output, float *A, float *L, float *U, float *P, size_t size_of_matr_row, size_t i)
{

    fprintf(output, "\nOperation %ld: \n\n", i);
    fprintf(output, " Matrix A: \n\n");
    write_pretty(output, size_of_matr_row, A);
    fprintf(output, " Matrix L: \n\n");
    write_pretty(output, size_of_matr_row, L);

    fprintf(output, " Matrix U: \n\n");
    write_pretty(output, size_of_matr_row, U);

    fprintf(output, " Matrix P: \n\n");
    write_pretty(output, size_of_matr_row, P);
    fprintf(output, LINE_SEPARATOR);
}

/**
 * Generates Matrix with given exponent as and upper boundary
 */
void matrix_generator(size_t n, float *A, float exp)
{
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < n * n; i++)
        A[i] = ((float)rand() / (RAND_MAX)) * exp;
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
