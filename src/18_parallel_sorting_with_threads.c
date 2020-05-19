#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <threads.h>

typedef struct sorting_args sorting_args;

struct sorting_args {
    size_t length;
    double* ptrA;
    double* ptrB;
};

void merge_double(size_t N, const double A[N], double B[N], size_t begin, size_t mid, size_t end) {
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

size_t thread_index = 0;

int mergesort_caller(void* arg) {
    sorting_args* sortingArgs = arg;
    mergesort_double(sortingArgs->length, sortingArgs->ptrB, sortingArgs->ptrA, 0, sortingArgs->length);
    return 0;
}

int main() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_sec * 1000 + ts.tv_nsec / 1000);

    size_t THREAD_COUNT = 2;

    for (size_t ARR_SIZE = (1u << 15u); ARR_SIZE <= (1u << 18u); ARR_SIZE <<= 1u) {
        double A[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            A[i] = rand() % ARR_SIZE;
        }
        double B[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            B[i] = A[i];
        }
        clock_t t1 = clock();
        thrd_t thrd[THREAD_COUNT];
        size_t CHUNK_SIZE = ARR_SIZE / THREAD_COUNT;
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            sorting_args sortingArgs = (sorting_args) {.length = CHUNK_SIZE,
                                                       .ptrA = A + (i * CHUNK_SIZE),
                                                       .ptrB = B + (i * CHUNK_SIZE)};
            thrd_create(&thrd[i], mergesort_caller, &sortingArgs);
        }
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            thrd_join(thrd[i], 0);
        }
        merge_double(ARR_SIZE, A, B, 0, CHUNK_SIZE, 2 * CHUNK_SIZE);

        clock_t t2 = clock();

        double dur = 1000.0 * (t2 - t1) / CLOCKS_PER_SEC;
        printf("For sorting %zu elements with %zu threads:\n", ARR_SIZE, THREAD_COUNT);
        printf("Mergesort: %.2f ms\n", dur);
    }
    printf("\n");

    THREAD_COUNT = 4;

    for (size_t ARR_SIZE = (1u << 15u); ARR_SIZE <= (1u << 18u); ARR_SIZE <<= 1u) {
        double A[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            A[i] = rand() % ARR_SIZE;
        }
        double B[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            B[i] = A[i];
        }
        clock_t t1 = clock();
        thrd_t thrd[THREAD_COUNT];
        size_t CHUNK_SIZE = ARR_SIZE / THREAD_COUNT;
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            sorting_args sortingArgs = (sorting_args) {.length = CHUNK_SIZE,
                    .ptrA = A + (i * CHUNK_SIZE),
                    .ptrB = B + (i * CHUNK_SIZE)};
            thrd_create(&thrd[i], mergesort_caller, &sortingArgs);
        }
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            thrd_join(thrd[i], 0);
        }
        merge_double(ARR_SIZE, A, B, 0, CHUNK_SIZE, 2 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 2 * CHUNK_SIZE, 3 * CHUNK_SIZE, 4 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 0, 2 * CHUNK_SIZE, 4 * CHUNK_SIZE);

        clock_t t2 = clock();

        double dur = 1000.0 * (t2 - t1) / CLOCKS_PER_SEC;
        printf("For sorting %zu elements with %zu threads:\n", ARR_SIZE, THREAD_COUNT);
        printf("Mergesort: %.2f ms\n", dur);
    }
    printf("\n");

    THREAD_COUNT = 8;

    for (size_t ARR_SIZE = (1u << 15u); ARR_SIZE <= (1u << 18u); ARR_SIZE <<= 1u) {
        double A[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            A[i] = rand() % ARR_SIZE;
        }
        double B[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            B[i] = A[i];
        }
        clock_t t1 = clock();
        thrd_t thrd[THREAD_COUNT];
        size_t CHUNK_SIZE = ARR_SIZE / THREAD_COUNT;
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            sorting_args sortingArgs = (sorting_args) {.length = CHUNK_SIZE,
                    .ptrA = A + (i * CHUNK_SIZE),
                    .ptrB = B + (i * CHUNK_SIZE)};
            thrd_create(&thrd[i], mergesort_caller, &sortingArgs);
        }
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            thrd_join(thrd[i], 0);
        }
        merge_double(ARR_SIZE, A, B, 0, CHUNK_SIZE, 2 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 2 * CHUNK_SIZE, 3 * CHUNK_SIZE, 4 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 0, 2 * CHUNK_SIZE, 4 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 4 * CHUNK_SIZE, 5 * CHUNK_SIZE, 6 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 6 * CHUNK_SIZE, 7 * CHUNK_SIZE, 8 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 4 * CHUNK_SIZE, 6 * CHUNK_SIZE, 8 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 0 * CHUNK_SIZE, 4 * CHUNK_SIZE, 8 * CHUNK_SIZE);

        clock_t t2 = clock();

        double dur = 1000.0 * (t2 - t1) / CLOCKS_PER_SEC;
        printf("For sorting %zu elements with %zu threads:\n", ARR_SIZE, THREAD_COUNT);
        printf("Mergesort: %.2f ms\n", dur);
    }
    printf("\n");

    THREAD_COUNT = 16;

    for (size_t ARR_SIZE = (1u << 15u); ARR_SIZE <= (1u << 18u); ARR_SIZE <<= 1u) {
        double A[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            A[i] = rand() % ARR_SIZE;
        }
        double B[ARR_SIZE];
        for (size_t i = 0; i < ARR_SIZE; i++) {
            B[i] = A[i];
        }
        clock_t t1 = clock();
        thrd_t thrd[THREAD_COUNT];
        size_t CHUNK_SIZE = ARR_SIZE / THREAD_COUNT;
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            sorting_args sortingArgs = (sorting_args) {.length = CHUNK_SIZE,
                    .ptrA = A + (i * CHUNK_SIZE),
                    .ptrB = B + (i * CHUNK_SIZE)};
            thrd_create(&thrd[i], mergesort_caller, &sortingArgs);
        }
        for (size_t i = 0; i < THREAD_COUNT; i++) {
            thrd_join(thrd[i], 0);
        }
        merge_double(ARR_SIZE, A, B, 0, CHUNK_SIZE, 2 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 2 * CHUNK_SIZE, 3 * CHUNK_SIZE, 4 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 0, 2 * CHUNK_SIZE, 4 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 4 * CHUNK_SIZE, 5 * CHUNK_SIZE, 6 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 6 * CHUNK_SIZE, 7 * CHUNK_SIZE, 8 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 4 * CHUNK_SIZE, 6 * CHUNK_SIZE, 8 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 0 * CHUNK_SIZE, 4 * CHUNK_SIZE, 8 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 8 * CHUNK_SIZE, 9 * CHUNK_SIZE, 10 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 10 * CHUNK_SIZE, 11 * CHUNK_SIZE, 12 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 8 * CHUNK_SIZE, 10 * CHUNK_SIZE, 12 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 12 * CHUNK_SIZE, 13 * CHUNK_SIZE, 14 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 14 * CHUNK_SIZE, 15 * CHUNK_SIZE, 16 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 12 * CHUNK_SIZE, 14 * CHUNK_SIZE, 16 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 8 * CHUNK_SIZE, 12 * CHUNK_SIZE, 16 * CHUNK_SIZE);
        merge_double(ARR_SIZE, A, B, 0 * CHUNK_SIZE, 8 * CHUNK_SIZE, 16 * CHUNK_SIZE);

        clock_t t2 = clock();

        double dur = 1000.0 * (t2 - t1) / CLOCKS_PER_SEC;
        printf("For sorting %zu elements with %zu threads:\n", ARR_SIZE, THREAD_COUNT);
        printf("Mergesort: %.2f ms\n", dur);
    }
    printf("\n");

    return EXIT_SUCCESS;
}