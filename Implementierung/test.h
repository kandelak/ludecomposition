#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
                fprintf(out, "%ld\n", max_size_of_row / 10);
        }

        for (size_t k = start; k <= max_size_of_row; k += 10)
        {
                float *A;

                size_t size_of_matr = k * k;

                A = malloc(sizeof(float) * size_of_matr);

                if (!A)
                {
                        perror("Could not allocate Memory");
                        exit(EXIT_FAILURE);
                }
                int i = 0;
                float exp = 1000.0;
                while (i++ < 1)
                {

                        fprintf(out, "%ld\n", k);
                        matrix_generator(k, A, exp);
                        exp *= 10;
                       // must_pivotize(k, A);
                        writeMatrix(out, k, A);
                }
                free(A);
        }

        fclose(out);
}