#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double vector_product(size_t N, const double A[static N], const double B[static N]) {
    double C = 0.0;
    for (size_t i = 0; i < N; i++) {
        C += A[i] * B[i];
    }
    return C;
}

void matrix_vector_product(size_t M, size_t N, const double A[static M][N], const double v[static N],
                           double b[static M]) {
    for (size_t i = 0; i < M; i++) {
        b[i] = 0.0;
        for (size_t j = 0; j < N; j++) {
            b[i] += A[i][j] * v[j];
        }
    }
}

static const double eps = 1e-5;

double almost_equal(const double x, const double y) {
    return fabs(x - y) < eps;
}

void gauss_elimination(size_t M, size_t N, double A[static M][N]) {
    size_t r = 0;
    size_t c = 0;
    while (r <= M && c <= N) {
        size_t pivot = 0;
        double max_leading = DBL_MIN;
        // search pivot
        for (size_t i = r; i < M; i++) {
            if (max_leading < fabs(A[i][c])) {
                max_leading = fabs(A[i][c]);
                pivot = i;
            }
        }
        if (almost_equal(A[pivot][c], 0.0)) {
            // if there's no pivot, pass to next column
            c++;
        } else {
            // swap rows
            for (size_t j = 0; j < N; j++) {
                double temp = A[r][j];
                A[r][j] = A[pivot][j];
                A[pivot][j] = temp;
            }
            // column elimination
            for (size_t i = r + 1; i < M; i++) {
                double ratio = A[i][c] / A[r][c];
                A[i][c] = 0.0;
                for (size_t j = c + 1; j < N; j++) {
                    A[i][j] -= A[r][j] * ratio;
                }
            }
            r++;
            c++;
        }
    }
}

#define SIZE_M 4
#define SIZE_N 3

int main() {

    double v1[SIZE_N] = {1.6, 2.7, 3.8,};
    double v2[SIZE_N] = {1.1, 3.5, 2.1,};
    printf("v1: ");
    for (size_t i = 0; i < SIZE_N; i++) {
        printf("%f ", v1[i]);
    }
    printf("\n");
    printf("v2: ");
    for (size_t i = 0; i < SIZE_N; i++) {
        printf("%f ", v2[i]);
    }
    printf("\n");
    double v = vector_product(SIZE_N, v1, v2);
    printf("product of v1 and v2: %f\n", v);

    double A[SIZE_M][SIZE_N] = {{6.7,  2.9, 11.3},
                                {5.8,  2.6, 8.5},
                                {9.2,  9.3, 6.5},
                                {19.5, 8.2, 6.2}};
    double b[SIZE_M] = {0};
    printf("A:\n");
    for (size_t i = 0; i < SIZE_M; i++) {
        for (size_t j = 0; j < SIZE_N; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    matrix_vector_product(SIZE_M, SIZE_N, A, v1, b);
    printf("product of A and v1: ");
    for (size_t i = 0; i < SIZE_M; i++) {
        printf("%f ", b[i]);
    }
    printf("\n");

    gauss_elimination(SIZE_M, SIZE_N, A);
    printf("A after gaussian elimination:\n");
    for (size_t i = 0; i < SIZE_M; i++) {
        for (size_t j = 0; j < SIZE_N; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}
