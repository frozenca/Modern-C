#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static size_t const ITERATION_LIMIT = 100000;

#define PI 3.14159265358979323846

void approx_pi(size_t precision) {
    double sqrt12 = sqrt(12.0);
    assert(precision);
    double value = 0.0;
    size_t k = 0;
    printf("Approximate pi by %zu digits\n", precision - 1);
    while (k < ITERATION_LIMIT) {
        value += sqrt12 / ((2.0 * k + 1.0) * pow(-3.0, k));
        printf("At iteration %zu, the value is %.16f, ", k, value);
        double error = fabs(value - PI);
        printf("error is %.16f\n", error);
        if (error < pow(0.1, precision)) {
            printf("Done!\n");
            break;
        }
        k++;
    }
}

int main() {
    approx_pi(5);
    approx_pi(11);
    return EXIT_SUCCESS;
}
