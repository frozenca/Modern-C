#include <tgmath.h>
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>

static double const eps = 1.0e-7;
static double complex const eps_c = 1.0e-7 + 1.0e-7 * I;
static double const tolerance = 1.0e-4;
static size_t const MAX_ITER = 30;

typedef double func_r(double);
typedef double complex func_c(double complex);

double derivative_r(func_r F, double x) {
    double x_p = x + eps;
    double x_m = x - eps;
    return (F(x_p) - F(x_m)) / (x_p - x_m);
}

double complex derivative_c(func_c F, double complex x) {
    double complex x_p = x + eps_c;
    double complex x_m = x - eps_c;
    return (F(x_p) - F(x_m)) / (x_p - x_m);
}

double find_real_root(double F(double), double init_point) {
    double x_0 = init_point;
    for (size_t iter = 1; iter < MAX_ITER; iter++) {
        double h = F(x_0) / derivative_r(F, x_0);
        double x_1 = x_0 - h;
        printf("Iteration %zu: x = %9.6f\n", iter, x_1);
        if (fabs(h) < tolerance) {
            printf("Root = %9.6f\n", x_1);
            return x_1;
        }
        x_0 = x_1;
    }
    return DBL_MAX;
}

typedef struct Polynomial Polynomial;

struct Polynomial {
    size_t N;
    double complex* coeff;
};

Polynomial* poly_init(Polynomial* poly, size_t N, const double complex coeff[static (N + 1)], bool zero_init) {
    if (poly) {
        *poly = (Polynomial) {
                .N = N,
                .coeff = malloc((N + 1) * sizeof(double complex)),
        };
        if (poly->coeff) {
            if (!zero_init) {
                for (size_t i = 0; i <= N; i++) {
                    poly->coeff[i] = coeff[i];
                }
            }
        } else {
            *poly = (Polynomial) {0};
        }
    }
    return poly;
}

Polynomial* poly_new(size_t N, const double complex coeff[static (N + 1)], bool zero_init) {
    return poly_init(malloc(sizeof(Polynomial)), N, coeff, zero_init);
}

double complex poly_evaluate(Polynomial* poly, double complex x) {
    double complex res = {0};
    if (poly) {
        res = poly->coeff[0];
        for (size_t i = 1; i <= poly->N; i++) {
            res = res * x + poly->coeff[i];
        }
    }
    return res;
}

void poly_delete(Polynomial* poly) {
    if (poly) {
        free(poly->coeff);
    }
    free(poly);
}

Polynomial* poly_div(const Polynomial* p, const Polynomial* d, Polynomial* r) {
    Polynomial* q = 0;
    if (p && d && p->N > d->N) {
        size_t N = p->N - d->N;
        double complex coeff[N];
        memset(coeff, 0, N * sizeof(double complex));
        q = poly_new(N, coeff, true);
        poly_delete(r);
        r = poly_new(p->N, p->coeff, false);

        double complex ratio = 0;
        for (size_t i = p->N; i >= d->N && i <= p->N; i--) {
            ratio = r->coeff[i] / d->coeff[d->N];
            q->coeff[i - d->N] = ratio;
            r->coeff[i] = 0;
            for (size_t j = 0; j < d->N; j++) {
                r->coeff[i - d->N + j] -= d->coeff[j] * ratio;
            }
        }
        while (!r->coeff[--(r->N)]);
    }
    return q;
}

double complex poly_derivative(Polynomial* p, double complex x) {
    double complex x_p = x + eps_c;
    double complex x_m = x - eps_c;
    return (poly_evaluate(p, x_p) - poly_evaluate(p, x_m)) / (x_p - x_m);
}

double complex poly_find_root(Polynomial* p, double complex init_point) {
    if (p) {
        double complex x_0 = init_point;
        for (size_t iter = 1; iter < MAX_ITER; iter++) {
            double complex h = poly_evaluate(p, x_0) / poly_derivative(p, x_0);
            double complex x_1 = x_0 - h;
            printf("Iteration %zu: x = %9.6f + %9.6fI\n", iter, creal(x_1), cimag(x_1));
            if (cabs(h) < tolerance) {
                printf("Root = %9.6f + %9.6fI\n", creal(x_1), cimag(x_1));
                return x_1;
            }
            x_0 = x_1;
        }
    }
    return DBL_MAX;
}

void poly_find_roots(Polynomial* p, double complex init_point) {
    if (p) {
        double complex root = poly_find_root(p, init_point);
        double complex coeff[2] = {1, -root};
        Polynomial* d = poly_new(1, coeff, false);
        Polynomial* r = 0;
        Polynomial* q = poly_div(p, d, r);
        poly_find_roots(q, init_point);
    }
}

int main(void) {
    find_real_root(log, 2.0);

    // x^2 - 3x + 2
    double complex coeff1[3] = {1.0, -3.0, 2.0};
    Polynomial* p1 = poly_new(2, coeff1, false);
    poly_find_roots(p1, 0);

    // x^2 + 1
    double complex coeff2[3] = {1.0, 0.0, 1.0};
    Polynomial* p2 = poly_new(2, coeff2, false);
    // poly_find_roots(p2, 0); // newton's method does not converge for real initial conditions
    poly_find_roots(p2, 0.5 * I);

    return EXIT_SUCCESS;
}