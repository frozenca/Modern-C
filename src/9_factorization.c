#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool is_prime(size_t p) {
    assert(p > 1);
    if (p == 2) return true;
    for (size_t i = 3; i <= sqrt(p); i += 2) {
        if (p % i == 0) return false;
    }
    return true;
}

void factorize_recur(size_t n) {
    if (n % 2 == 0) {
        printf("2 ");
        while (n % 2 == 0) {
            n /= 2;
        }
    }
    for (size_t i = 3; i <= n; i += 2) {
        if (is_prime(i) && (n % i == 0)) {
            printf("%zu ", i);
            while (n % i == 0) {
                n /= i;
            }
            factorize_recur(n);
            break;
        }
    }
}

void factorize(size_t n) {
    printf("%zu: ", n);
    factorize_recur(n);
    printf("\n");
}

int main(int argc, char* argv[argc + 1]) {
    for (size_t i = 1; i < argc; i++) {
        char* end = 0;
        size_t n = strtoull(argv[i], &end, 10);
        factorize(n);
    }
    return EXIT_SUCCESS;
}