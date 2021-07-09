#include <immintrin.h>
#include <math.h>

void ludecomp_intrinsics(size_t n, const float *A, float *L, float *U, float *P)
{

    // Copying A in U
    size_t index = n * n;
    const float *tempA = A;
    float *tempU = U;
    while (index > 3)
    {
        _mm_storeu_ps(tempU, _mm_loadu_ps(tempA));
        index -= 4;
        tempU += 4;
        tempA += 4;
    }

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
            //swapping in L(Only before i - th column) if (k < i)
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
            // Row with Leading entry in Column
            int offset2 = (j + 1) * n;
            // Actual Row
            int offset1 = i * n;

            float faktor = U[i + offset2] / U[i + offset1];

            float faktor_arr[4] = {faktor, faktor, faktor, faktor};

            // Writing factors in L

            L[i + offset2] = faktor;
            size_t k = 0;

            __m128 factors = _mm_loadu_ps(faktor_arr);

            // n is unsigned -> if n<3 -> n-3 = very big number
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
            // Scalar completion if needed
            for (size_t x = k; x < n; x++)
            {
                U[offset2 + x] -= U[offset1 + x] * faktor;
            }
        }
    }
}
