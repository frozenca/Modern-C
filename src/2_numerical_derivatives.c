#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static double const eps = 1.0e-7;

#define PI 3.14159265358979323846

double derivative(double F(double), double x) {
    double x_p = x + eps;
    double x_m = x - eps;
    return (F(x_p) - F(x_m)) / (x_p - x_m);
}

double is_almost_equal(double a, double b, double epsilon) {
    return fabs(a - b) < epsilon;
}

int main() {
    double point_0 = 0.0, point_1 = PI / 2, point_2 = 4;

    double sin_d_0 = derivative(sin, point_0);
    assert(is_almost_equal(sin_d_0, cos(point_0), eps));
    printf("Derivative of sin at %g : %g\n", point_0, sin_d_0);

    double sin_d_1 = derivative(sin, point_1);
    assert(is_almost_equal(sin_d_1, cos(point_1), eps));
    printf("Derivative of sin at %g : %g\n", point_1, sin_d_1);

    double cos_d_0 = derivative(cos, point_0);
    assert(is_almost_equal(cos_d_0, -sin(point_0), eps));
    printf("Derivative of cos at %g : %g\n", point_0, cos_d_0);

    double cos_d_1 = derivative(cos, point_1);
    assert(is_almost_equal(cos_d_1, -sin(point_1), eps));
    printf("Derivative of cos at %g : %g\n", point_1, cos_d_1);

    double sqrt_d_2 = derivative(sqrt, point_2);
    assert(is_almost_equal(sqrt_d_2, 1 / (2.0 * sqrt(point_2)), eps));
    printf("Derivative of sqrt at %g : %g\n", point_2, sqrt_d_2);

    return EXIT_SUCCESS;
}