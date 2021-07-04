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

/**
 * For Benchmarking the program
 */
static inline double curtime(void)
{
    struct timespec t;
    clock_gettime(1, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

void writeMatrix(FILE *out, size_t n, const float *M)
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
 * Allocates Memory Space for Matrix
 * If size to large : Heap Allocation
 * If not : Stack Allocation
 * Explanation : 
 * matr_size : Matrix size
 * matr_size*sizeof()float*4 <=> matr_size*4*4 for number of BYTES allocation needed for 4 Matrices (We need always 4 Matrices for our Calculations)
 * MAX_STACK_MEM : Maximum number of BYTES one can allocate in the linux operating system
 * @return Pointer to the Allocated space for the Matrix with of @param matr_size 
 */

/**
 *
 * Warning : Exception handling not implemented yet !!!
 *
 * @param n : size
 * @param fp
 * @param matrix
 */
void read_matrix_from_stream(size_t n, FILE *fp, float *matrix)
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

    if (mat_size > (index - 1))
    {
        printf("ERROR : Wrong number of entries for the Matrix for the specified size  \n"
               "Usage : \nfirst number is always the number of Matrices to be calculated  \n "
               "Row/Column size of the particular Matrix\n"
               "Entries of the Matrix\n\n"
               "Row/Column size of the particular Matrix\n"
               "Entries of the Matrix..\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
}

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

int test_matrix_eq(size_t n, float *orgM, float *M, float tolerate)
{
    int res = 1;
    for (size_t i = 0; i < n * n; i++)
    {
        if (fabsf(orgM[i] - M[i]) >= tolerate)
        {
            printf("<<%f!=%f>> ", M[i], orgM[i]);
            res = 0;
        }
    }
    return res;
}

#define LINE_SEPARATOR " \n############################################### \n\n"

void run_bench(void (*func)(size_t, const float *, float *, float *, float *), FILE *output, float *A, float *L, float *U, float *P, size_t iterations, char *name, size_t i, size_t size_of_matr_row)
{
    double start, end;
    fprintf(output, "%s Implementation on Operation %ld took (in Seconds) : \n", name, i + 1);
    for (size_t k = 0; k < iterations; k++)
    {
        start = curtime();
        func(size_of_matr_row, A, L, U, P);
        end = curtime();
        fprintf(output, "%f\n", end - start);
    }
}

void print_pretty(FILE *output, float *A, float *L, float *U, float *P, size_t size_of_matr_row, size_t i)
{
    fprintf(output, "\nOperation %ld: \n\n", i + 1);
    fprintf(output, " Matrix A: \n\n");
    writeMatrix(output, size_of_matr_row, A);
    fprintf(output, " Matrix L: \n\n");
    writeMatrix(output, size_of_matr_row, L);

    fprintf(output, " Matrix U: \n\n");
    writeMatrix(output, size_of_matr_row, U);

    fprintf(output, " Matrix P: \n\n");
    writeMatrix(output, size_of_matr_row, P);
    fprintf(output, LINE_SEPARATOR);

    //print_result_without_solution(size_of_matr_row, A, L, U, P, output, 1e-3);
}

void printMatrix(size_t n, const float *M, FILE *output)
{
    for (size_t index = 0; index < n * n; index++)
    {
        fprintf(output, "%f ", M[index]);
        if ((index + 1) % n == 0)
            printf("\n");
    }
}

void matrix_generator_intervals(size_t n, float *A, float a, float b)
{
    srand((unsigned int)time(NULL));

    for (size_t i = 0; i < n * n; i++)
        A[i] = ((float)rand() / (float)(RAND_MAX)) * (b - a) + a;
}

/**
 * Returns 1 if no errors found and 0 if yes. 
 */
int print_result_without_solution(size_t n, float *A, float *L, float *U,
                                  float *P, FILE *output, float tolerate)
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
        if (size_of_matr < 100)
        {
            fprintf(output, "<<Wrong Output. Printing Error Matrix...>>\n\n\n");
           // print_error_matrix(n, A, PxLxU, output, tolerate);
        }
        res = 0;
    }

    free(LxU);
    free(PxLxU);
    return res;
}

// void print_result_with_solution(size_t n, float *orgL, float *L, float *orgU,
//                              float *U, float *orgP, float *P, FILE *output)
// {
//     printf("Testing ...\n");
//     int fehlerL = test_matrix_eq(n, orgL, L);
//     int fehlerU = test_matrix_eq(n, orgU, U);
//     int fehlerP = test_matrix_eq(n, orgP, P);
//     printf("%d Fehler in L;   %d Fehler in U;   %d Fehler in P\n", fehlerL,
//            fehlerU, fehlerP);
//     if (fehlerL != 0)
//     {
//         printf("Fehler in L:\n");
//         print_error_matrix(n, orgL, L, output);
//     }
//     if (fehlerU != 0)
//     {
//         printf("Fehler in U:\n");
//         print_error_matrix(n, orgU, U, output);

//         if (fehlerP != 0)
//         {
//             printf("Fehler in P:\n");
//             print_error_matrix(n, orgP, P, output);
//         }
//         printf("\n\n\n");
//     }
