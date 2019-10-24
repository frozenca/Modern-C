#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void merge_double(size_t N, double A[N], double B[N], size_t begin, size_t mid, size_t end) {
    size_t i = begin;
    size_t j = mid;
    for (size_t k = begin; k < end; k++) {
        if (i < mid && (j >= end || A[i] <= A[j])) {
            B[k] = A[i];
            i++;
        } else {
            B[k] = A[j];
            j++;
        }
    }
}

void mergesort_double(size_t N, double A[N], double B[N], size_t begin, size_t end) {
    if (end < begin || end - begin < 2) {
        return;
    }
    size_t mid = begin + (end - begin) / 2;
    mergesort_double(N, B, A, begin, mid);
    mergesort_double(N, B, A, mid, end);
    merge_double(N, A, B, begin, mid, end);
}

void mergesort_helper_double(size_t N, double A[N]) {
    double B[N];
    for (size_t i = 0; i < N; i++) {
        B[i] = A[i];
    }
    mergesort_double(N, B, A, 0, N);
}

size_t partition_double(size_t N, double A[N], size_t begin, size_t end) {
    double pivot = A[end];
    size_t i = begin;
    for (size_t j = begin; j <= end; j++) {
        if (A[j] < pivot) {
            double temp = A[j];
            A[j] = A[i];
            A[i] = temp;
            i++;
        }
    }
    double temp = A[i];
    A[i] = A[end];
    A[end] = temp;
    return i;
}

void quicksort_double(size_t N, double A[N], size_t begin, size_t end) {
    if (begin < end && end < N) {
        size_t p = partition_double(N, A, begin, end);
        quicksort_double(N, A, begin, p - 1);
        quicksort_double(N, A, p + 1, end);
    }
}

static size_t const ARR_SIZE = 20;

int main() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_sec * 1000 + ts.tv_nsec / 1000);

    double A1[ARR_SIZE];
    printf("Array A1 before mergesort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        A1[i] = rand() % ARR_SIZE;
        printf("%g ", A1[i]);
    }
    mergesort_helper_double(ARR_SIZE, A1);
    printf("\n");
    bool is_sorted = true;
    for (size_t i = 1; i < ARR_SIZE; i++) {
        if (A1[i] < A1[i - 1]) {
            is_sorted = false;
            break;
        }
    }
    printf("Array A1 is %s.\n", is_sorted ? "sorted" : "not sorted");
    printf("Array A1 after mergesort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        printf("%g ", A1[i]);
    }
    printf("\n");

    double A2[ARR_SIZE];
    printf("Array A2 before quicksort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        A2[i] = rand() % ARR_SIZE;
        printf("%g ", A2[i]);
    }
    quicksort_double(ARR_SIZE, A2, 0, ARR_SIZE - 1);
    printf("\n");
    is_sorted = true;
    for (size_t i = 1; i < ARR_SIZE; i++) {
        if (A2[i] < A2[i - 1]) {
            is_sorted = false;
            break;
        }
    }
    printf("Array A2 is %s.\n", is_sorted ? "sorted" : "not sorted");
    printf("Array A2 after quicksort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        printf("%g ", A2[i]);
    }
    printf("\n");

    return EXIT_SUCCESS;
}