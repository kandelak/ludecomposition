#include "ludecomp.h"
#include "test.h"
#include "io.h"
#include <unistd.h>
#include "ludecomp_double_array.h"
#include "ludecomp_intrinsics.h"

void test_double_array_withoutP(size_t n) {
    size_t k = 0;
    double t = 0;
    do {
        float genA[k][k];
        matrixGenerator_double_array(k, genA);
        float L[k][k], U[k][k];
        t = ludecomp_double_array_withoutP(k, genA, L, U);
        printf("%f\n", t);
    } while (k++ < n);

}

void test_double_array(size_t n) {
    size_t k = 0;
    double t = 0;
    do {
        float genA[k][k];
        matrixGenerator_double_array(k, genA);
        float L[k][k], U[k][k], P[k][k];
        t = ludecomp_double_array(k, genA, L, U, P);
        printf("%f\n", t);
    } while (k++ < n);

}

void single_size_test(size_t n, size_t iterations) {
    double time = 0;
    float genA[n * n];
    matrixGenerator(n, genA);

    float L[n * n], U[n * n], P[n * n];

    while (iterations-- > 0) {
        time = ludecomp(n, genA, L, U, P);
        printf("%f\n", time);
    }
}

void test_without_P(size_t n) {
    size_t cnt = 0;
    double time = 0;
    do {
        float genA[cnt * cnt];
        matrixGenerator(cnt, genA);
        float L[cnt * cnt], U[cnt * cnt];
        time = ludecomp_without_P(cnt, genA, L, U);
        printf("%f\n", time);
    } while (cnt++ < n);

}

void test(size_t n) {
    size_t cnt = 0;
    double time = 0;
    do {
        float genA[cnt * cnt];
        matrixGenerator(cnt, genA);
        float L[cnt * cnt], U[cnt * cnt], P[cnt * cnt];
        time = ludecomp(cnt, genA, L, U, P);
        printf("%f\n", time);
    } while (cnt++ < n);

}


int main(int argc, char **argv) {
    float A[25];
    readFile("test.txt", A);
    float L[25];
    float U[25];
    float P[25];

    ludecomp_without_P(5, A, L, U);
    printMatrix(5, L);
    printMatrix(5, U);
}
