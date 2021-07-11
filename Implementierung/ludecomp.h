#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "matr_utilities.h"
#include <immintrin.h>

/**
 * LU-Decomposition without using Pivoting method
 */
int ludecomp_without_P(size_t n, const float *A, float *L, float *U, float *P)
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

    // Calculating factors and  storing them in L. Setting column entries in U to 0 using subtraction and corresponding factors.
    for (size_t i = 0; i < n; i++)
    {

        for (size_t j = i; j < n - 1; j++)
        {
            // Check if this Matrix can be decomposed
            if (U[i + i * n]==0)
            {
                printf("Entry: %ld,%ld ",i,j);
                return 0;
            }
            // Calculating factor for given column entry
            float faktor = U[i + (j + 1) * n] / U[i + (i * n)];

            // Writing factors in L
            L[i + (j + 1) * n] = faktor;

            // Setting given column entry to null using subtraction
            for (size_t x = 0; x < n; x++)
            {
                U[(j + 1) * n + x] -= U[(i * n) + x] * faktor;
            }
        }
    }

    if (U[n * n - 1] == 0)
    {
        return 0;
    }
    return 1;
}
/**
 * Decomposition using Pivoting
 */
int ludecomp(size_t n, const float *A, float *L, float *U, float *P)
{
    size_t pivoted = 0;
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
        //Pivoting

        // Searching for maximal absolute value in Column i
        float max = U[i + i * n];

        size_t row_with_max = i;
        for (size_t k = i + 1; k < n; k++)
        {

            if (fabsf(U[i + k * n]) > max)
            {
                max = fabsf(U[i + k * n]);
                row_with_max = k;
            }
        }

        // If row with max abs value is the actual row itself -> skip
        if (row_with_max != i)
        {
            pivoted++;
            // Swapping rows i and row_with_max in U and L if needed
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
        }

        // pivotizing done

        for (size_t j = i; j < n - 1; j++)
        {
            if (U[i + (i * n)] == 0)
            {
                return 0;
            }
            float faktor = U[i + (j + 1) * n] / U[i + (i * n)];

            // Writing factors in L
            L[i + (j + 1) * n] = faktor;

            for (size_t x = 0; x < n; x++)
            {
                U[(j + 1) * n + x] -= U[(i * n) + x] * faktor;
            }
        }
    }
    if (U[n * n - 1] == 0)
    {
        
        return 0;
    }
    return 1;
}

/**
 * LU-Decomposition using SIMD Intrinsics
 */
int ludecomp_intrinsics(size_t n, const float *A, float *L, float *U, float *P)
{

    // Copying A in U
    size_t index = n * n;
    const float *tempA = A;
    float *tempU = U;

    // Vectorized
    while (index > 3)
    {
        _mm_storeu_ps(tempU, _mm_loadu_ps(tempA));
        index -= 4;
        tempU += 4;
        tempA += 4;
    }
    // Scalar
    for (size_t k = n * n - index; k < n * n; k++)
    {
        U[k] = A[k];
    }

    // Writing Identity matrices in P and L

    __m128 nulls = _mm_setzero_ps();
    size_t i = 0;
    while (i < n * n)
    {
        P[i] = 1;
        L[i] = 1;
        i++;
        // Vectorized
        if (i >= n * n)
            break;
        size_t k = 0;
        if (n > 3)
        {
            for (k = 0; k < (n - 4); k += 4)
            {
                _mm_storeu_ps(&P[i], nulls);
                _mm_storeu_ps(&L[i], nulls);
                i += 4;
            }
        }
        // Scalar
        for (size_t j = k; k < n; k++)
        {
            P[i] = 0;
            L[i] = 0;
            i++;
        }
    }

    for (size_t i = 0; i < n; i++)
    {

        //Pivoting

        // Searching for maximal absolute value in Column i
        float max = U[i + i * n];
        size_t row_with_max = i;
        for (size_t k = i + 1; k < n; k++)
        {
            if (fabsf(U[i + k * n]) > max)
            {
                max = fabsf(U[i + k * n]);
                row_with_max = k;
            }
        }

        // Swapping rows i and row_with_max in U and L if needed
        if (row_with_max != i)
        {
            size_t k = 0;

            // Swapping in U

            // Vectorized
            if (n > 3)
            {
                for (k = 0; k < (n - 4); k += 4)
                {
                    __m128 row1_U = _mm_loadu_ps(&U[i * n + k]);
                    __m128 row2_U = _mm_loadu_ps(&U[row_with_max * n + k]);
                    _mm_storeu_ps(&U[i * n + k], row2_U);
                    _mm_storeu_ps(&U[row_with_max * n + k], row1_U);
                }
            }

            // Scalar
            while (k < n)
            {
                float temp = U[i * n + k];
                U[i * n + k] = U[row_with_max * n + k];
                U[row_with_max * n + k] = temp;

                k++;
            }

            // Swappipng in L
            k = 0;
            // Vectorized
            if (i > 3)
            {
                for (k = 0; k < (i - 4); k += 4)
                {
                    __m128 row1_L = _mm_loadu_ps(&L[i * n + k]);
                    __m128 row2_L = _mm_loadu_ps(&L[row_with_max * n + k]);
                    _mm_storeu_ps(&L[i * n + k], row2_L);
                    _mm_storeu_ps(&L[row_with_max * n + k], row1_L);
                }
            }
            // Scalar
            while (k < i)
            {
                float temp = L[i * n + k];
                L[i * n + k] = L[row_with_max * n + k];
                L[row_with_max * n + k] = temp;
                k++;
            }

            // Swapping Columns in P
            for (size_t j = 0; j < n; j++)
            {
                float temp = P[j * n + i];
                P[j * n + i] = P[j * n + row_with_max];
                P[j * n + row_with_max] = temp;
            }
        }

        // Pivoting done

        for (size_t j = i; j < n - 1; j++)
        {
            // Calculating frequently used offsets in advance
            int offset2 = (j + 1) * n;
            int offset1 = i * n;
            if (U[i + offset1] == 0)
            {
                return 0;
            }
            float faktor = U[i + offset2] / U[i + offset1];

            float faktor_arr[4] = {faktor, faktor, faktor, faktor};

            // Writing factors in L

            L[i + offset2] = faktor;
            size_t k = 0;

            __m128 factors = _mm_loadu_ps(faktor_arr);

            // why n > 3 ?  n is unsigned -> if n < 3 -> n-3 = very big number
            // Vectorized
            if (n > 3)
            {
                for (k = 0; k < n - 3; k += 4)
                {
                    __m128 temp2 = _mm_loadu_ps(&U[offset1 + k]);
                    __m128 temp3 = _mm_mul_ps(factors, temp2);
                    temp2 = _mm_loadu_ps(&U[offset2 + k]);
                    temp3 = _mm_sub_ps(temp2, temp3);
                    _mm_storeu_ps(&U[(j + 1) * n + k], temp3);
                }
            }

            // Scalar
            for (size_t x = k; x < n; x++)
            {
                U[offset2 + x] -= U[offset1 + x] * faktor;
            }
        }
    }
    if (U[n * n - 1] == 0)
    {
        return 0;
    }
    return 1;
}
