#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define LINE_SEPARATOR " \n############################################### \n\n"

void printHelp()
{
    printf("\n--help/-h : Display brief summaries of command line Options\n\n");
    printf("--bench-all : Generates and runs benchmark on the pre-defined input configuration (in bench.txt)\n\n"
           "       Example: ./main --bench-all\n\n"
           "       Note: After this flag is used you can then use the generated file for benchmarking other implementations\n\n"
           "       Example: ./main --input=bench.txt -v 2 --no-print -b\n\n");
    printf("--input/-f: Specify input stream\n\n"
           "       Example: ./main --input=input_file.txt\n"
           "       Example: ./main --input input_file.txt\n"
           "       Example: ./main -f=input_file.txt\n\n"
           "       Note: Standart input stream will not be used as default or at all. Always specify input file!\n\n"
           "       How input works?\n\n"
           "       Input for our program is just enter (new line) seperated numbers in the file\n\n"
           "       First number is always integer and specifies the number of the matrices to be decomposed\n"
           "       After that each and every time first comes the row/column size (Also integer) of the Matrix and then the matrix entries\n"
           "       Note: Input validity check fails if these constraints are not kept. For instance, if you write something before entering the enter,that is not a number\n\n");
    printf("--no-print: Deactivates user-friendly printing. For instance for testing/benchmarking purposes or during computations on larger matrices.\n\n"
           "       Example: ./main --input=input_file.txt --no-print\n\n"
           "       Note: Always use this if the matrices are too large\n\n");
    printf("-b: Activates Benchmarking\n\n"
           "       Example: ./main --input=input_file.txt -b\n"
           "       Example: ./main --input=input_file.txt -b --no-print\n\n");
    printf("--output/-o: Chooses output stream for storing the results\n\n"
           "       Note: Standart output stream is the default output stream\n\n"
           "       Example: ./main --input=input_file.txt -b --output=results.txt\n\n");
    printf("-v: Chooses the implementation for the decomposition\n\n"
           "   Implementations:\n\n"
           "       0 : C\n"
           "       1 : C with intrinsics\n"
           "       2 : Assembler with SIMD\n"
           "       3 : Assembler\n"
           "       4 : C without Pivoting\n\n"
           "       Note: C is the default implementation\n\n"
           "       Note: C without Pivoting is not working properly most of the time. Nevertheless you can still try to decompose with it\n\n"
           "       Example: ./main --input=input_file.txt --output=results.txt -v 2\n"
           "       Example: ./main -f input_file.txt -v 1 -b");
    printf("-i : Choose number of iterations\n\n"
           "       Note: If benchmarking the average time is computed automatically\n\n"
           "       Example: ./main --input input_file.txt -o result.txt -b -v 3 -i 10\n\n");
    printf("--test-all/-k : Tests chosen implementation with pre-defined input from predefined file (test.txt)\n\n");
    printf("--random-test : Generates single random input with specified row/column size of the matrix (in file random_single.txt)\n\n"
           "       Example: ./main --random-test 500\n"
           "       This will generate 500x500 randomized matrix\n\n"
           "       After that you can use this command to run the test:\n\n"
           "       Example: ./main --input=random_single.txt --no-print -t\n"
           "       Example: ./main --input=random_single.txt --no-print -t -v 2\n\n"
           "       Or you can also run bench using this command and write results in output file:"
           "       Example: ./main --input=random_single.txt --no-print -b -o result.txt -v 3\n\n");
    printf("--random-tests : Generates multiple random inputs for testing purposes (in file random_multiple.txt)\n\n"
           "       Example: ./main --random-tests\n"
           "       After that you can run tests again and write results in output file using this command:\n"
           "       Example: ./main --input=random_multiple.txt --output=result.txt --no-print -t -v 2");
    printf("--pivot/-p : Shows comparision example for decomposition with and without pivoting\n\n"
           "       Example: ./main --pivot\n\n");
    printf("-t : Tests the specified input file and version of Implementation\n\n"
           "       Example: ./main --input=input_file.txt -t -v 2 --no-print\n\n");
}

/**
 * Checks validity of the input
 */

int check_validity(FILE *input)
{
    char buf[50];
    size_t line_number = 1;
    size_t num_of_matr;

    if (!fgets(buf, 50, input))
    {
        fprintf(stderr, "Reading Input Failed. (empty file)\n");
        printf("Line: %lu\n",line_number);
        return 0;
    }

    errno = 0;

    char *endptr;

    num_of_matr = strtol(buf, &endptr, 10);


    if (errno == ERANGE)
    {
        perror("Integer Number read is too small or too large.\n");
        printf("Line: %lu\n",line_number);
        return 0;
    }
    else if (endptr == buf)
    {
        printf("Nothing Read (No input for the number of the matrices)\n");
        printf("Line: %lu\n",line_number);
        return 0;
    }
    else if (*endptr && *endptr != '\n')
    {
        printf("Wrong Input Format (All numbers must be enter seperated). Did you specify number of the Matrices?. Please refer to --help/-h.\n");
        printf("Line: %lu\n",line_number);
        return 0;
    }
    else if ((int)num_of_matr <= 0)
    {
        printf("Number of Matrices can not be less than or equal 0.\n");
        printf("Line: %lu\n",line_number);
        return 0;
    }

        line_number++;

    for (size_t k = 0; k < num_of_matr; k++)
    {
        if (!fgets(buf, 50, input))
        {
            fprintf(stderr, "Reading Input Failed. (less input than specified/empty line)\n");
            printf("Line: %lu\n",line_number);
            return 0;
        }
        
        size_t size_of_matr_row = strtol(buf, &endptr, 10);

        if (errno == ERANGE)
        {
            perror("Integer number read was too small or too large.\n");
            printf("Line: %lu\n",line_number);
            return 0;
        }
        else if (endptr == buf)
        {
            printf("Nothing Read (No input for the Size of the Matrix)\n");
            printf("Line: %lu\n",line_number);
            return 0;
        }
        else if (*endptr && *endptr != '\n')
        {
            printf("Wrong Input Format (All numbers must be enter seperated). Did you specify row/column length of the Matrix?  Please refer to --help/-h.\n");
            printf("Line: %lu\n",line_number);
            return 0;
        }
        else if ((int)size_of_matr_row <= 0)
        {
            printf("Row/Column size of the Matrix can not be less than or equal zero.\n");
            printf("Line: %lu\n",line_number);
            return 0;
        }

        line_number++;
        size_t size_of_matr = size_of_matr_row * size_of_matr_row;
        for (size_t i = 0; i < size_of_matr; i++)
        {
            if (!fgets(buf, 50, input))
            {
                fprintf(stderr, "Reading Input Failed. (less input than specified/empty line)\n");
                printf("Line: %lu\n",line_number);
                return 0;
            }

            float matr_entry = strtod(buf, &endptr);

            if (errno == ERANGE)
            {
                perror("Floating-point Number read was too small or too large.\n");
                printf("Line: %lu\n",line_number);
                return 0;
            }
            else if (endptr == buf)
            {
                printf("Nothing Read (Not enough entries for specified size of the Matrix)\n");
                printf("Line: %lu\n",line_number);
                return 0;
            }
            else if (*endptr && *endptr != '\n')
            {

                printf("Wrong Input Format (Matrix Entry. All numbers must be enter seperated). Please refer to --help/-h.\n");
                printf("Line: %lu\n",line_number);
                return 0;
            }
            line_number++;
        }
    }

    return 1;
}

/**
 * Writes Matrix M in the stream specified with out (Used for representing matrices to User)
 */
void write_pretty(FILE *out, size_t n, const float *M)
{
    for (size_t index = 0; index < n * n - 1; index++)
    {
        fprintf(out, "%f ", M[index]);
        if ((index + 1) % n == 0)
            fprintf(out, "\n");
    }
    fprintf(out, "%f\n\n", M[n * n - 1]);
}

#define STACK_LIMIT 700

/**
 * Reads matrix entries from the stream specified with fp and writes in matrix
 */
void read_matrix_from_stream(size_t n, FILE *fp, float *matrix)
{

    size_t index = 0;

    size_t mat_size = n * n;

    while (mat_size > 0)
    {
        if (fscanf(fp, "%f", matrix + (index++)) == -1)
        {
            break;
        }
        mat_size--;
    }
}

/**
 * Writes Decomposition matrices in the stream specified with output
 */
void print_pretty(FILE *output, const float *A, float *L, float *U, float *P, size_t size_of_matr_row, size_t i)
{

    fprintf(output, "\nOperation %lu: \n\n", i);
    fprintf(output, " Matrix A: \n\n");
    write_pretty(output, size_of_matr_row, A);
    fprintf(output, " Matrix L: \n\n");
    write_pretty(output, size_of_matr_row, L);

    fprintf(output, " Matrix U: \n\n");
    write_pretty(output, size_of_matr_row, U);

    fprintf(output, " Matrix P: \n\n");
    write_pretty(output, size_of_matr_row, P);
    fprintf(output, LINE_SEPARATOR);
}

/**
 * Writes Matrix M in the stream specified with out (Used for generating input file)
 */
void write_matrix_in_stream(FILE *out, size_t n, const float *M)
{
    for (size_t index = 0; index < n * n - 1; index++)
    {
        fprintf(out, "%f\n", M[index]);
    }
    fprintf(out, "%f\n", M[n * n - 1]);
}
