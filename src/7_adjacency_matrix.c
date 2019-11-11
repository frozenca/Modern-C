#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SIZE_N 6

void BFS(size_t N, bool A[static N][N], bool visited[static N], size_t source) {
    assert(source < N);
    size_t front = 0, rear = 0;
    size_t Q[N];
    for (size_t i = 0; i < N; i++) {
        Q[i] = 0;
    }
    visited[source] = true;
    Q[rear++] = source;
    printf("%zu ", source);
    while (front < rear) {
        size_t v = Q[front++];
        for (size_t i = 0; i < N; i++) {
            if (A[v][i] && !visited[i]) {
                visited[i] = true;
                printf("%zu ", i);
                Q[rear++] = i;
            }
        }
    }
    printf("\n");
}

void connected_components(size_t N, bool A[static N][N]) {
    bool visited[N];
    for (size_t i = 0; i < N; i++) {
        visited[i] = false;
    }
    for (size_t i = 0; i < N; i++) {
        if (!visited[i]) {
            BFS(N, A, visited, i);
        }
    }
    printf("\n");
}

size_t minimum_key(size_t N, size_t key[static N], bool visited[static N]) {
    size_t min_key = SIZE_MAX, min_index = 0;
    for (size_t i = 0; i < N; i++) {
        if (!visited[i] && key[i] < min_key) {
            min_key = key[i];
            min_index = i;
        }
    }
    return min_index;
}

void spanning_tree(size_t N, bool A[static N][N]) {
    size_t parent[N];
    size_t key[N];
    bool visited[N];
    for (size_t i = 0; i < N; i++) {
        parent[i] = 0;
        key[i] = SIZE_MAX;
        visited[i] = false;
    }
    key[0] = 0;
    parent[0] = SIZE_MAX;
    for (size_t count = 0; count < N; count++) {
        size_t u = minimum_key(N, key, visited);
        visited[u] = true;
        for (size_t v = 0; v < N; v++) {
            if (A[u][v] && !visited[v] && A[u][v] < key[v]) {
                parent[v] = u;
                key[v] = A[u][v];
            }
        }
    }
    for (size_t i = 1; i < N; i++) {
        printf("%zu - %zu\n", parent[i], i);
    }
}

int main() {
    bool A[SIZE_N][SIZE_N] = {{false, true, false, false, false, false},
                              {false, false, true, false, false, true},
                              {false, false, false, false, false, false},
                              {false, true, false, false, true, false},
                              {false, false, false, false, false, true},
                              {false, false, false, false, false, false},};

    printf("A:\n");
    for (size_t i = 0; i < SIZE_N; i++) {
        for (size_t j = 0; j < SIZE_N; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("BFS on A: ");
    bool v[SIZE_N] = {0,};
    BFS(SIZE_N, A, v, 0);

    bool B[SIZE_N][SIZE_N] = {{false, true, false, false, false, false},
                              {true, false, true, true, false, false},
                              {false, true, false, false, false, false},
                              {false, true, false, false, true, false},
                              {false, false, false, true, false, false},
                              {false, false, false, false, false, false},};

    printf("B:\n");
    for (size_t i = 0; i < SIZE_N; i++) {
        for (size_t j = 0; j < SIZE_N; j++) {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Connected components on B: ");
    connected_components(SIZE_N, B);


    bool C[SIZE_N][SIZE_N] = {{false, true, false, false, false, false},
                              {true, false, true, true, false, false},
                              {false, true, false, false, false, false},
                              {false, true, false, false, true, false},
                              {false, false, false, true, false, true},
                              {false, false, false, false, true, false},};

    printf("C:\n");
    for (size_t i = 0; i < SIZE_N; i++) {
        for (size_t j = 0; j < SIZE_N; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Spanning tree of C:\n");
    spanning_tree(SIZE_N, C);

    return EXIT_SUCCESS;
}