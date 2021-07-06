#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "matr_utilities.h"

/**
 *  @param max_size_of_row :  Maximum size of the Matrix to be tested
 *  @param only_this_size!=0 :  Tests randomized Matrix only of this size
 *  @paran only_this_size==0 : Generates randomized input Matrices of size o @param max_size_of_row 
 */

#define TOLERATE_ERROR 2e-1
void generate_random_tests(size_t max_size_of_row, int only_this_size, char *output)
{

        FILE *out;
        {
                out = fopen(output, "w");
                if (!out)
                {
                        perror("Error occurred while opening file for generating the randomized inputs for Testing");
                        exit(EXIT_FAILURE);
                }
        }
        size_t start = 1;
        if (only_this_size)
        {
                start = max_size_of_row;
                fprintf(out, "%ld\n", 1);
        }
        else
        {
                fprintf(out, "%ld\n", max_size_of_row);
        }

        for (size_t k = start; k <= max_size_of_row; k++)
        {
                float *A;

                size_t size_of_matr = k * k;

                A = malloc(sizeof(float) * size_of_matr);

                if (!A)
                {
                        perror("Could not allocate Memory");
                        exit(EXIT_FAILURE);
                }
                float a;
                float b;
                float intervals[4][4] = {{0, 1}, {-789, -2}, {10000, 200000}, {400000, 400002}};

                int i = 0;
                // while (i < 4)
                {
                        a = intervals[i][0];
                        b = intervals[i][1];
                        i++;
                        fprintf(out, "%ld\n", k);
                        matrix_generator_intervals(k, A, a, b);
                        writeMatrix(out, k, A);
                }

                free(A);
        }
        fclose(out);
}
