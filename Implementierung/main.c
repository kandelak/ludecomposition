#include "ludecomp.h"
#include "test.h"
#include "io.h"
#include <unistd.h>
#include "ludecomp_intrinsics.h"

/**
 * for benchmarking the program
 */
static inline double curtime(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

/**
 * To Choose the Implementation 
 */
struct Variant
{
    const char *name;

    void (*func)(char *);
};

typedef struct Variant Variant;

const Variant variants[] = {
    {"c", ludecomp},
    {"c_intrinsics", ludecomp_intrinsics},
};

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
        time = curtime() - time;
        printf("%f\n", time);
    }
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
        time = curtime() - time;
        printf("%f\n", time);
    } while (cnt++ < n);
}

int main(int argc, char **argv)
{
    ioFunction(ludecomp, argc, argv);

    return 0;
}
