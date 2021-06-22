#include <stddef.h>

/**
 * LU Decomposition using Doolitle Algorithm, which doesn't use the Gaussian Elimination
 */

void ludecomp_2(size_t n, const float *A, float *L, float *U, float *P)
{
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
                P[i * n + j] = 0;
                L[i * n + j] = 0;
            }
        }
    }

    //Decomposing
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; i < n; i++)
        {
            //Handling the U
            if (i == 0)
            {
                for (size_t k = 0; k < n; k++)
                {
                    U[k] = A[k];
                }
            }
            else
            {
                size_t sum = 0;
                for (size_t k = 0; k < i; k++)
                {
                    sum += L[i * n + k] * U[k * n + j];
                }
                U[i * n + j] = A[i * n + j] - sum;
            }

            //Handling the L
            if (j == 0)
            {
                L[i * n + j] = A[i * n + j] / U[j * n + j];
            }
            else
            {
                size_t sum = 0;
                for (size_t k = 0; k < j; i++)
                {
                    sum += L[i * n + k] * U[k * n + j];
                }
                L[i * n + j] = (A[i * n + j] - sum) / U[j * n + j];
            }
        }
    }
}
