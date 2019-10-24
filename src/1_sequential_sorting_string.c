#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

void merge_string(size_t N, char* A[N], char* B[N], size_t begin, size_t mid, size_t end) {
    size_t i = begin;
    size_t j = mid;
    for (size_t k = begin; k < end; k++) {
        if (i < mid && (j >= end || strcmp(A[j], A[i]) >= 0)) {
            strcpy(B[k], A[i]);
            i++;
        } else {
            strcpy(B[k], A[j]);
            j++;
        }
    }
}

void mergesort_string(size_t N, char* A[N], char* B[N], size_t begin, size_t end) {
    if (end < begin || end - begin < 2) {
        return;
    }
    size_t mid = begin + (end - begin) / 2;
    mergesort_string(N, B, A, begin, mid);
    mergesort_string(N, B, A, mid, end);
    merge_string(N, A, B, begin, mid, end);
}

void mergesort_helper_string(size_t N, char* A[N]) {
    char* B[N];
    for (size_t i = 0; i < N; i++) {
        B[i] = calloc(strlen(A[i]) + 1, sizeof(char));
        if (!B[i]) {
            printf("Not enough memory\n");
            errno = 1;
            return;
        }
        strcpy(B[i], A[i]);
    }
    mergesort_string(N, B, A, 0, N);
}

void swap_strings(char* str1[static 1], char* str2[static 1]) {
    char* temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

void quicksort_string(size_t N, char* A[N]) {
    if (!N) {
        return;
    }
    size_t i = 0;
    for (size_t j = 0; j < N - 1; j++) {
        if (strcmp(A[j], A[N - 1]) < 0) {
            swap_strings(A + i, A + j);
            i++;
        }
    }
    swap_strings(A + i, A + N - 1);
    quicksort_string(i, A);
    i++;
    quicksort_string(N - i, A + i);
}

static char const charset[] = "abcdefghijklmnopqrstuvwxyz";

static char* rand_string(size_t N, char str[N]) {
    if (N) {
        for (size_t i = 0; i < N; i++) {
            size_t key = rand() % (sizeof charset - 1);
            str[i] = charset[key];
        }
    }
    return str;
}

static char* rand_string_alloc(size_t N) {
    char* s = calloc(N + 1, sizeof(char));
    return s;
}

static size_t const STR_SIZE = 10;
static size_t const ARR_SIZE = 20;

int main() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_sec * 1000 + ts.tv_nsec / 1000);

    char* A1[ARR_SIZE];
    printf("Array A1 before mergesort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        size_t length = 1 + rand() % STR_SIZE;
        A1[i] = rand_string_alloc(length);
        if (!A1[i]) {
            printf("Not enough memory\n");
            return EXIT_FAILURE;
        }
        A1[i] = rand_string(length, A1[i]);
        printf("%s ", A1[i]);
    }
    mergesort_helper_string(ARR_SIZE, A1);
    printf("\n");
    if (errno == 1) {
        return EXIT_FAILURE;
    }
    bool is_sorted = true;
    for (size_t i = 1; i < ARR_SIZE; i++) {
        if (strcmp(A1[i - 1], A1[i]) > 0) {
            is_sorted = false;
            break;
        }
    }
    printf("Array A1 is %s.\n", is_sorted ? "sorted" : "not sorted");
    printf("Array A1 after mergesort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        printf("%s ", A1[i]);
    }
    printf("\n");

    char* A2[ARR_SIZE];
    printf("Array A2 before quicksort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        size_t length = 1 + rand() % STR_SIZE;
        A2[i] = rand_string_alloc(length);
        if (!A2[i]) {
            printf("Not enough memory\n");
            return EXIT_FAILURE;
        }
        A2[i] = rand_string(length, A2[i]);
        printf("%s ", A2[i]);
    }
    quicksort_string(ARR_SIZE, A2);
    is_sorted = true;
    for (size_t i = 1; i < ARR_SIZE; i++) {
        if (strcmp(A2[i - 1], A2[i]) > 0) {
            is_sorted = false;
            break;
        }
    }
    printf("\n");
    printf("Array A2 is %s.\n", is_sorted ? "sorted" : "not sorted");
    printf("Array A2 after quicksort:\n");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        printf("%s ", A2[i]);
    }
    printf("\n");

    return EXIT_SUCCESS;
}