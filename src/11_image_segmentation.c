#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

static size_t const MERGE_CRITERION = 50;

typedef struct Segment Segment;

struct Segment {
    unsigned char value;
    size_t parent;
    double average;
    size_t size;
};

size_t FindCompress(size_t N, Segment pixels[static N], size_t i) {
    if (i >= N) return SIZE_MAX;
    if (pixels[i].parent != i) {
        pixels[i].parent = FindCompress(N, pixels, pixels[i].parent);
    }
    return pixels[i].parent;
}

bool Union(size_t N, Segment pixels[static N], size_t i, size_t j) {
    if (i >= N || j >= N) return false;
    size_t root_i = FindCompress(N, pixels, i);
    size_t root_j = FindCompress(N, pixels, j);
    if (root_i == root_j) return false;
    if (fabs(pixels[root_i].average - pixels[root_j].average) > MERGE_CRITERION) {
        return false;
    }
    if (pixels[root_i].size < pixels[root_j].size) {
        size_t temp = root_i;
        root_i = root_j;
        root_j = temp;
    }
    pixels[root_j].parent = root_i;
    double new_average = ((pixels[root_i].average * pixels[root_i].size)
            + (pixels[root_j].average * pixels[root_j].size)) / (double)(pixels[root_i].size + pixels[root_j].size);
    pixels[root_i].average = new_average;
    pixels[root_i].size += pixels[root_j].size;
    return true;
}

int main() {
    FILE* img = fopen("sample.bmp", "rb");
    FILE* outimg = fopen("sample_gray.bmp", "wb");

    unsigned char header[54] = {0};
    fread(header, sizeof(unsigned char), 54, img);
    fwrite(header, sizeof(unsigned char), 54, outimg);

    uint32_t width = *(uint32_t*)&header[18];
    uint32_t height = *(uint32_t*)&header[22];
    uint32_t stride = (width * 3 + 3u) & ~(3u);
    uint32_t padding = stride - width * 3;

    printf("width %u, height %u, stride %u, padding %u\n", width, height, stride, padding);

    size_t image_size = width * height;
    Segment greyscaled[image_size];

    unsigned char pixel[3] = {0};
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            fread(pixel, 3, 1, img);
            unsigned char gray = (unsigned char)(pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11);
            greyscaled[i * width + j] = (Segment){.value = gray, .parent = (i * width + j),
                                                  .average = gray, .size = 1};
        }
        fread(pixel, padding, 1, img);
    }
    size_t change_count = 1;

    size_t iteration = 0;
    while (change_count > 0) {
        change_count = 0;
        for (uint32_t j = 1; j < width; j++) {
            if (Union(image_size, greyscaled, j, j - 1)) {
                change_count++;
            }
        }
        for (uint32_t i = 1; i < height; i++) {
            if (Union(image_size, greyscaled, i * width, (i - 1) * width)) {
                change_count++;
            }
            for (uint32_t j = 1; j < width; j++) {
                if (Union(image_size, greyscaled, i * width + j, (i - 1) * width + j)) {
                    change_count++;
                }
                if (Union(image_size, greyscaled, i * width + j, i * width + j - 1)) {
                    change_count++;
                }
            }
        }
        printf("Iteration %zu, change_count %zu\n", ++iteration, change_count);
    }

    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            size_t root = FindCompress(image_size, greyscaled, i * width + j);
            unsigned char gray = (unsigned char) greyscaled[root].average;
            memset(pixel, gray, sizeof(pixel));
            fwrite(&pixel, 3, 1, outimg);
        }
        fwrite(pixel, padding, 1, outimg);
    }

    fclose(img);
    fclose(outimg);

    return EXIT_SUCCESS;
}
