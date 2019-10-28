#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

void MakeSet(size_t N, size_t parent[static N]) {
    for (size_t i = 0; i < N; i++) {
        parent[i] = i;
    }
}

size_t Find(size_t N, size_t parent[static N], size_t i) {
    if (i >= N) return SIZE_MAX;
    if (parent[i] != i) {
        return Find(N, parent, parent[i]);
    } else {
        return i;
    }
}

void FindReplace(size_t N, size_t parent[static N], size_t i, size_t value) {
    if (i >= N) return;
    if (parent[i] == i) {
        parent[i] = value;
    } else {
        size_t prev_parent = parent[i];
        parent[i] = value;
        FindReplace(N, parent, prev_parent, value);
    }
}

size_t FindCompress(size_t N, size_t parent[static N], size_t i) {
    if (i >= N) return SIZE_MAX;
    if (parent[i] != i) {
        parent[i] = FindCompress(N, parent, parent[i]);
    }
    return parent[i];
}

void Union(size_t N, size_t parent[static N], size_t i, size_t j) {
    if (i >= N || j >= N) return;
    FindReplace(N, parent, i, FindCompress(N, parent, j));
}

void print(size_t N, size_t parent[static N]) {
    for (size_t i = 0; i < N; i++) {
        printf("%zu ", parent[i]);
    }
    printf("\n");
}

#define TABLE_SIZE 10

int main() {
    size_t parent[TABLE_SIZE];
    MakeSet(TABLE_SIZE, parent);
    assert(Find(TABLE_SIZE, parent, 0) == 0);
    FindReplace(TABLE_SIZE, parent, 2, 3);
    FindReplace(TABLE_SIZE, parent, 3, 4);
    FindReplace(TABLE_SIZE, parent, 3, 5);
    FindReplace(TABLE_SIZE, parent, 8, 7);
    FindReplace(TABLE_SIZE, parent, 7, 9);
    assert(Find(TABLE_SIZE, parent, 3) == 5);
    assert(Find(TABLE_SIZE, parent, 7) == 9);
    FindCompress(TABLE_SIZE, parent, 2);
    FindReplace(TABLE_SIZE, parent, 0, 2);
    FindReplace(TABLE_SIZE, parent, 1, 7);
    FindCompress(TABLE_SIZE, parent, 8);
    Union(TABLE_SIZE, parent, 0, 1);
    assert(Find(TABLE_SIZE, parent, 1) == Find(TABLE_SIZE, parent, 3));
    assert(Find(TABLE_SIZE, parent, 1) != Find(TABLE_SIZE, parent, 6));

    return EXIT_SUCCESS;
}