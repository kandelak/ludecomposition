#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

/**
 * Last @param (flaot* P) is always Identity Matrix whilst operating without Pivoting
 * Why still there ? : For simplicity of choosing the version of the Implemenetation via @struct Implementation
 */
void ludecomp_without_P(size_t n, const float *A, float *L, float *U, float *P)
{

    // Copying A in U
    for (size_t index = 0; index < n * n; index++)
    {
        U[index] = A[index];
    }

    // Writing Identity matrices in L and P
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            if (i == j)
            {
                L[i * n + j] = 1;
                P[i * n + j] = 1;
            }
            else
            {
                L[i * n + j] = 0;
                P[i * n + j] = 0;
            }
        }
    }

    for (size_t i = 0; i < n; i++)
    {

        for (size_t j = i; j < n - 1; j++)
        {

            float faktor = U[i + (j + 1) * n] / U[i + (i * n)];

            // Writing factors in L
            L[i + (j + 1) * n] = faktor;

            for (size_t x = 0; x < n; x++)
            {
                U[(j + 1) * n + x] -= U[(i * n) + x] * faktor;
            }
        }
    }
}

void ludecomp(size_t n, const float *A, float *L, float *U, float *P)
{
    // Copying A in U
    for (size_t index = 0; index < n * n; index++)
    {
        U[index] = A[index];
    }

    // Writing Identity matrices in P and L
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            if (i == j)
            {
                L[i * n + j] = 1;
                P[i * n + j] = 1;
            }
            else
            {
                L[i * n + j] = 0;
                P[i * n + j] = 0;
            }
        }
    }

    for (size_t i = 0; i < n; i++)
    {

        // Searching for maximal absolute value in Column i
        float max = U[i + i * n];
        size_t row_with_max = i;
        for (size_t k = i + 1; k < n; k++)
        {
            if (fabsf(U[i + k * n]) > max)
            {
                max = U[i + k * n];
                row_with_max = k;
            }
        }

        // Pivoting

        // Swapping rows i and row_with_max in U and L
        for (size_t k = 0; k < n; k++)
        {
            float temp = U[i * n + k];
            U[i * n + k] = U[row_with_max * n + k];
            U[row_with_max * n + k] = temp;

            // swapping in L (Only before i-th column)
            if (k < i)
            {
                temp = L[i * n + k];
                L[i * n + k] = L[row_with_max * n + k];
                L[row_with_max * n + k] = temp;
            }
        }

        // Swapping Columns in P
        for (size_t k = 0; k < n; k++)
        {
            float temp = P[k * n + i];
            P[k * n + i] = P[k * n + row_with_max];
            P[k * n + row_with_max] = temp;
        }

        // pivotizing done

        for (size_t j = i; j < n - 1; j++)
        {

            float faktor = U[i + (j + 1) * n] / U[i + (i * n)];

            // Writing factors in L
            L[i + (j + 1) * n] = faktor;

            for (size_t x = 0; x < n; x++)
            {
                U[(j + 1) * n + x] -= U[(i * n) + x] * faktor;
            }
        }
    }
}
