#include "ludecomp.h"
#include "test.h"
#include "io.h"
#include <unistd.h>
#include "ludecomp_intrinsics.h"
#include "ludecomp_2.h"

/**
 * for benchmarking the program
 */
static inline double curtime(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}



void single_size_test(size_t n, size_t iterations)
{
    double time = 0;
    float genA[n * n];
    matrixGenerator(n, genA);

    float L[n * n], U[n * n], P[n * n];

    while (iterations-- > 0)
    {
        time = curtime();
        ludecomp(n, genA, L, U, P);
        time = curtime()-time;
        printf("%f\n", time);
    }
}

void test_without_P(size_t n)
{
    size_t cnt = 0;
    double time = 0;
    do
    {
        float genA[cnt * cnt];
        matrixGenerator(cnt, genA);
        float L[cnt * cnt], U[cnt * cnt];
        time = curtime();
        ludecomp_without_P(cnt, genA, L, U);
        time = curtime()-time;
        printf("%f\n", time);
    } while (cnt++ < n);
}

void test(size_t n)
{
    size_t cnt = 0;
    double time = 0;
    do
    {
        float genA[cnt * cnt];
        matrixGenerator(cnt, genA);
        float L[cnt * cnt], U[cnt * cnt], P[cnt * cnt];
        time = curtime();
        ludecomp(cnt, genA, L, U, P);
        printf("%f\n", time);
    } while (cnt++ < n);
}

int main(int argc, char **argv)
{
    ioFunction(ludecomp_2, argc, argv);

    return 0;
}
