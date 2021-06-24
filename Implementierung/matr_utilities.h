#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

void matrixMul(size_t n, float *A, float *B, float *Res)
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

void printErrorMatrix(size_t n, float *orgM, float *M, FILE *output)
{
    for (size_t i = 0; i < n * n; i++)
    {
        if (fabsf(orgM[i] - M[i]) >= 1e-2)
        {
            fprintf(output, "\x1B[31m%f (!=%f),", M[i], orgM[i]);
        }
        else
            fprintf(output, "\x1B[31m%f,", M[i]);
        if ((i + 1) % n == 0)
            fprintf(output, "\n");
    }
    fprintf(output, "\n");
}

int testMatrixEQ(size_t n, float *orgM, float *M)
{
    int res = 0;
    for (size_t i = 0; i < n * n; i++)
    {
        if (fabsf(orgM[i] - M[i]) >= 1e-1)
            res++;
    }
    return res;
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

void matrixGenerator(size_t n, float *A)
{
    srand((unsigned int)time(NULL));
    float a =
        7867865.34598; // Zufallsgenerator:
    // https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
    for (size_t i = 0; i < n * n; i++)
        A[i] = (((float)rand() / (float)(RAND_MAX)) - 0.5) * a;
}

/**
 * Returns 1 if no errors found and 0 if yes. 
 */
int printResultWithoutSolution(size_t n, float *A, float *L, float *U,
                               float *P, FILE *output)
{
    int res = 1;
    float LxU[n * n], PxLxU[n * n];
    matrixMul(n, L, U, LxU);
    matrixMul(n, P, LxU, PxLxU);
    int test_PxLxU = testMatrixEQ(n, A, PxLxU);
    if (test_PxLxU != 0)
    {
        fprintf(output, "\033[0;31mWrong Output. Printing Error Matrix...\n\n\n");
        printErrorMatrix(n, A, PxLxU, output);
        res = 0;
    }
    return res;
}

// void printResultWithSolution(size_t n, float *orgL, float *L, float *orgU,
//                              float *U, float *orgP, float *P, FILE *output)
// {
//     printf("Testing ...\n");
//     int fehlerL = testMatrixEQ(n, orgL, L);
//     int fehlerU = testMatrixEQ(n, orgU, U);
//     int fehlerP = testMatrixEQ(n, orgP, P);
//     printf("%d Fehler in L;   %d Fehler in U;   %d Fehler in P\n", fehlerL,
//            fehlerU, fehlerP);
//     if (fehlerL != 0)
//     {
//         printf("Fehler in L:\n");
//         printErrorMatrix(n, orgL, L, output);
//     }
//     if (fehlerU != 0)
//     {
//         printf("Fehler in U:\n");
//         printErrorMatrix(n, orgU, U, output);

//         if (fehlerP != 0)
//         {
//             printf("Fehler in P:\n");
//             printErrorMatrix(n, orgP, P, output);
//         }
//         printf("\n\n\n");
//     }
