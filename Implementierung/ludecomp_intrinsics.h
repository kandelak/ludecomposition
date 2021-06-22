#include <immintrin.h>

double ludecomp_without_P_intrinsics(size_t n, const float *A, float *L, float *U) {

    double start = curtime();
    // Copying A in U
    size_t index = n * n;
    const float *tempA = A;
    const float *tempU = U;
    while (index > 3) {
        _mm_storeu_ps(tempU, _mm_loadu_ps(tempA));
        index -= 4;
        tempU += 4;
        tempA += 4;
    }

    for (size_t k = n * n - index; k < n * n; k++) {
        U[k] = A[k];
    }



    // Writing Identity matrices in P and L


    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j) {
                L[i * n + j] = 1;
            } else {
                L[i * n + j] = 0;
            }
        }
    }

    for (size_t i = 0; i < n; i++) {

        for (size_t j = i; j < n - 1; j++) {

            float faktor = U[i + (j + 1) * n] / U[i + (i * n)];

            float faktor_arr[4] = {faktor, faktor, faktor, faktor};

            // Writing factors in L

            L[i + (j + 1) * n] = faktor;
            int k = 0;

            for (k = 0; k < n - 3; k += 4) {
                __m128 temp = _mm_loadu_ps(faktor_arr);
                __m128 temp2 = _mm_loadu_ps(&U[(i * n) + k]);
                temp = _mm_mul_ps(temp, temp2);
                temp2 = _mm_loadu_ps(&U[(j + 1) * n + k]);
                temp = _mm_sub_ps(temp2, temp);
                _mm_storeu_ps(&U[(j + 1) * n + k], temp);
            }

            for (size_t x = k; x < n; x++) {
                U[(j + 1) * n + x] -= U[(i * n) + x] * faktor;
            }

        }
    }
    double end = curtime();
    return end - start;
}