#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

void merge(void* A, void* B, size_t N, size_t size, int comp(const void*, const void*), size_t begin, size_t mid, size_t end) {
    size_t i = begin;
    size_t j = mid;
    for (size_t k = begin; k < end; k++) {
        if (i < mid && (j >= end || comp(A + size * i, A + size * j) <= 0)) {
            memcpy(B + k * size, A + i * size, size);
            i++;
        } else {
            memcpy(B + k * size, A + j * size, size);
            j++;
        }
    }
}

void mergesort_helper(void* B, void* A, size_t N, size_t size, int comp(const void*, const void*), size_t begin, size_t end) {
    if (end < begin || end - begin < 2) {
        return;
    }
    size_t mid = begin + (end - begin) / 2;
    mergesort_helper(B, A, N, size, comp, begin, mid);
    mergesort_helper(B, A, N, size, comp, mid, end);
    merge(A, B, N, size, comp, begin, mid, end);
}

void mergesort(void* A, size_t N, size_t size, int comp(const void*, const void*)) {
    void* B = malloc(N * size);
    if (B) {
        memcpy(B, A, N * size);
        mergesort_helper(B, A, N, size, comp, 0, N);
    }
}

size_t partition(void* A, size_t N, size_t size, int comp(const void*, const void*), size_t begin, size_t end) {
    size_t i = begin;
    for (size_t j = begin; j <= end; j++) {
        if (comp(A + size * j, A + size * end) < 0) {
            unsigned char temp[size];
            memcpy(temp, A + size * j, size);
            memcpy(A + size * j, A + size * i, size);
            memcpy(A + size * i, temp, size);
            i++;
        }
    }
    unsigned char temp[size];
    memcpy(temp, A + size * i, size);
    memcpy(A + size * i, A + size * end, size);
    memcpy(A + size * end, temp, size);
    return i;
}

void quicksort(void* A, size_t N, size_t size, int comp(const void*, const void*), size_t begin, size_t end) {
    if (begin < end && end < N) {
        size_t p = partition(A, N, size, comp, begin, end);
        quicksort(A, N, size, comp, begin, p - 1);
        quicksort(A, N, size, comp, p + 1, end);
    }
}

int compare_double(const void* a, const void* b) {
    double arg1 = *(const double *) a;
    double arg2 = *(const double *) b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

int main(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_sec * 1000 + ts.tv_nsec / 1000);

    for (size_t ARR_SIZE = 2; ARR_SIZE <= (1u << 18u); ARR_SIZE <<= 1u) {
        double A1[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            A1[i] = rand() % ARR_SIZE;
        }
        clock_t t1 = clock();
        mergesort(A1, ARR_SIZE, sizeof(double), compare_double);
        clock_t t2 = clock();

        double A2[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            A2[i] = rand() % ARR_SIZE;
        }
        clock_t t3 = clock();
        quicksort(A2, ARR_SIZE, sizeof(double), compare_double, 0, ARR_SIZE - 1);
        clock_t t4 = clock();

        double A3[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            A3[i] = rand() % ARR_SIZE;
        }

        clock_t t5 = clock();
        qsort(A3, ARR_SIZE, sizeof(double), compare_double);
        clock_t t6 = clock();

        double dur1 = 1000.0 * (t2 - t1) / CLOCKS_PER_SEC;
        double dur2 = 1000.0 * (t4 - t3) / CLOCKS_PER_SEC;
        double dur3 = 1000.0 * (t6 - t5) / CLOCKS_PER_SEC;
        printf("For sorting %zu elements:\n", ARR_SIZE);
        printf("Mergesort: %.2f ms\n", dur1);
        printf("Quicksort: %.2f ms\n", dur2);
        printf("qsort: %.2f ms\n", dur3);
    }
}