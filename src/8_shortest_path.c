#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SIZE_N 6

void dijkstra(size_t N, size_t A[static N][N], size_t source, size_t target) {
    size_t visited[N], dist[N], prev[N], unvisited_count = 0;
    for (size_t i = 0; i < N; i++) {
        dist[i] = SIZE_MAX;
        prev[i] = SIZE_MAX;
        visited[i] = false;
        unvisited_count++;
    }
    dist[source] = 0;
    while (unvisited_count) {
        size_t min_dist = SIZE_MAX, min_vertex = 0;
        for (size_t i = 0; i < N; i++) {
            if (!visited[i] && dist[i] < min_dist) {
                min_dist = dist[i];
                min_vertex = i;
            }
        }
        unvisited_count--;
        assert(min_dist != SIZE_MAX);
        visited[min_vertex] = true;
        for (size_t i = 0; i < N; i++) {
            if (A[min_vertex][i]) {
                size_t value = dist[min_vertex] + A[min_vertex][i];
                if (value < dist[i]) {
                    dist[i] = value;
                    prev[i] = min_vertex;
                }
            }
        }
    }
    size_t curr = target;
    while (curr != source) {
        printf("%zu - %zu\n", prev[curr], curr);
        curr = prev[curr];
    }

}

int main() {
    size_t A[SIZE_N][SIZE_N] = {{0, 7, 9, 0, 0, 14},
                                {7, 0, 10, 15, 0, 0},
                                {9, 10, 0, 11, 0, 2},
                                {0, 15, 11, 0, 6, 0},
                                {0, 0, 0, 6, 0, 9},
                                {14, 0, 2, 0, 9, 0}};
    printf("A:\n");
    for (size_t i = 0; i < SIZE_N; i++) {
        for (size_t j = 0; j < SIZE_N; j++) {
            printf("%zu ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    size_t source = 0, target = 5;
    printf("Shortest path from source %zu to target %zu (in reversed order):\n", source, target);
    dijkstra(SIZE_N, A, source, target);

    return EXIT_SUCCESS;
}