#include <assert.h>
#include <math.h>
#include <complex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static double const eps = 1.0e-7;

#define PI 3.14159265358979323846

double complex derivative(double complex F(double complex), double complex x) {
	double complex x_p = x + eps;
    double complex x_m = x - eps;
    return (F(x_p) - F(x_m)) / (x_p - x_m);
}

bool is_almost_equal(double complex a, double complex b, double epsilon) {
    return cabs(a - b) < epsilon;
}

int main() {
    double complex point_0 = 0.0, point_1 = PI / 2 + (PI / 2) * I, point_2 = 4 + 4 * I;

    double complex sin_d_0 = derivative(csin, point_0);
    assert(is_almost_equal(sin_d_0, ccos(point_0), eps));
    printf("Derivative of sin at %g + %g i : %g + %g i\n",
        creal(point_0), cimag(point_0), creal(sin_d_0), cimag(sin_d_0));

    double complex sin_d_1 = derivative(csin, point_1);
    assert(is_almost_equal(sin_d_1, ccos(point_1), eps));
    printf("Derivative of sin at %g + %g i : %g + %g i\n",
        creal(point_1), cimag(point_1), creal(sin_d_1), cimag(sin_d_1));
        
    double complex cos_d_0 = derivative(ccos, point_0);
    assert(is_almost_equal(cos_d_0, -csin(point_0), eps));
    printf("Derivative of cos at %g + %g i : %g + %g i\n",
        creal(point_0), cimag(point_0), creal(cos_d_0), cimag(cos_d_0));

    double complex cos_d_1 = derivative(ccos, point_1);
    assert(is_almost_equal(cos_d_1, -csin(point_1), eps));
    printf("Derivative of cos at %g + %g i : %g + %g i\n",
        creal(point_1), cimag(point_1), creal(cos_d_1), cimag(cos_d_1));

    double complex sqrt_d_2 = derivative(csqrt, point_2);
    assert(is_almost_equal(sqrt_d_2, 1 / (2.0 * csqrt(point_2)), eps));
    printf("Derivative of sqrt at %g + %g i : %g + %g i\n",
        creal(point_2), cimag(point_2), creal(sqrt_d_2), cimag(sqrt_d_2));

    return EXIT_SUCCESS;
}
