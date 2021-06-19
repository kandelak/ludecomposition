void matrixMul(size_t n, float *A, float *B, float *Res) {
    size_t i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            Res[i * n + j] = 0;
            for (k = 0; k < n; k++)
                Res[i * n + j] += A[i * n + k] * B[k * n + j];
        }
    }
}

void printErrorMatrix(size_t n, float *orgM, float *M) {
    for (size_t i = 0; i < n * n; i++) {
        if (abs(orgM[i] - M[i]) >= 1e-9)
            printf("\x1B[31m%f (!=%f),", M[i], orgM[i]);
        else
            printf("\x1B[0m%f,", M[i]);
        if ((i + 1) % n == 0)
            printf("\n");
    }
    printf("\x1B[0m\n");
}

int testMatrixEQ(size_t n, float *orgM, float *M) {
    int res = 0;
    for (size_t i = 0; i < n * n; i++) {
        if (abs(orgM[i] - M[i]) >= 1e-9)
            res++;
    }
    return res;
}

void printMatrix(size_t n, const float *M) {
    for (size_t index = 0; index < n * n; index++) {
        printf("%f ", M[index]);
        if ((index + 1) % n == 0)
            printf("\n");
    }
}

void matrixGenerator(size_t n, float *A) {
    srand((unsigned int) time(NULL));
    float a =
            7867865.34598; // Zufallsgenerator:
    // https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
    for (size_t i = 0; i < n * n; i++)
        A[i] = (((float) rand() / (float) (RAND_MAX)) - 0.5) * a;
}


void matrixGenerator_double_array(size_t n, float A[n][n]) {
    srand((unsigned int) time(NULL));
    float a =
            7867865.34598; // Zufallsgenerator:
    // https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
    for (size_t i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A[i][j] = (((float) rand() / (float) (RAND_MAX)) - 0.5) * a;
        }
}

void matrixMul_double_array(size_t n, float A[n][n], float B[n][n], float res[n][n]) {

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++)
                res[i][j] += A[i][k] * B[k][j];
        }
    }
}

void printMatrix_double_array(size_t n, const float M[n][n]) {
    for (size_t index = 0; index < n; index++) {
        for (int j = 0; j < n; j++) {
            printf("%f ", M[index][j]);
        }
        printf("\n");
    }

    printf("\n\n");
}

void printResultWithoutSolution(size_t n, float *A, float *L, float *U,
                                float *P) {
    printf("###### Fehleranalyse:\n");
    float LxU[n * n], PxLxU[n * n];
    matrixMul(n, L, U, LxU);
    matrixMul(n, P, LxU, PxLxU);
    int fehlerPxLxU = testMatrixEQ(n, A, PxLxU);
    printf("%d Fehler bei L*U*P\n", fehlerPxLxU);
    if (fehlerPxLxU != 0) {
        printf("Fehler in Erbenis von L*U*P:\n");
        printErrorMatrix(n, A, PxLxU);
    }
    printf("\n\n\n");
}

void printResultWithSolution(size_t n, float *orgL, float *L, float *orgU,
                             float *U, float *orgP, float *P) {
    printf("###### Fehleranalyse:\n");
    int fehlerL = testMatrixEQ(n, orgL, L);
    int fehlerU = testMatrixEQ(n, orgU, U);
    int fehlerP = testMatrixEQ(n, orgP, P);
    printf("%d Fehler in L;   %d Fehler in U;   %d Fehler in P\n", fehlerL,
           fehlerU, fehlerP);
    if (fehlerL != 0) {
        printf("Fehler in L:\n");
        printErrorMatrix(n, orgL, L);
    }
    if (fehlerU != 0) {
        printf("Fehler in U:\n");
        printErrorMatrix(n, orgU, U);
    }
    if (fehlerP != 0) {
        printf("Fehler in P:\n");
        printErrorMatrix(n, orgP, P);
    }
    printf("\n\n\n");
}
