#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Blob Blob;

struct Blob {
    char* data;
    Blob* prev;
    Blob* next;
};

Blob* blob_init(Blob* blob, const char* src) {
    if (blob) {
        if (src) {
            *blob = (Blob) {
                    .data = malloc(sizeof(char[strlen(src) + 1])),
                    .prev = 0,
                    .next = 0,
            };
            if (blob->data) {
                strcpy(blob->data, src);
            }
        } else {
            *blob = (Blob) {0};
        }
    }
    return blob;
}

Blob* blob_new(const char* src) {
    return blob_init(malloc(sizeof(Blob)), src);
}

Blob* blob_set_data(Blob* blob, char* src) {
    if (blob) {
        free(blob->data);
        blob->data = src;
    }
    return blob;
}

void blob_destroy(Blob* blob) {
    if (blob) {
        free(blob->data);
        if (blob->next) {
            blob->next->prev = blob->prev;
        }
        if (blob->prev) {
            blob->prev->next = blob->next;
        }
        blob_init(blob, 0);
    }
}

void blob_delete(Blob* blob) {
    blob_destroy(blob);
    free(blob);
}

Blob* blob_split(Blob* blob, size_t split_point) {
    if (blob && blob->data) {
        size_t len = strlen(blob->data);
        if (len > split_point) {
            char *data1 = malloc(split_point + 1);
            char *data2 = malloc(len - split_point + 1);
            if (data1 && data2) {
                strncpy(data1, blob->data, split_point);
                strncpy(data2, blob->data + split_point, len - split_point);
                blob_set_data(blob, data1);
                Blob *blob2 = blob_new(data2);
                blob2->prev = blob;
                blob2->next = blob->next;
                if (blob->next) {
                    blob->next->prev = blob2;
                    blob->next = blob2;
                }
            }
            free(data1);
            free(data2);
        }
    }
    return blob;
}

Blob* blob_join(Blob* blob1, Blob* blob2) {
    if (blob1 && blob2 && blob1->data && blob2->data && blob1->next == blob2 && blob2->prev == blob1) {
        size_t len1 = strlen(blob1->data);
        size_t len2 = strlen(blob2->data);
        if (len1 && len2) {
            char* data_joined = malloc(len1 + len2 + 1);
            if (data_joined) {
                strcpy(data_joined, blob1->data);
                strcpy(data_joined + len1, blob2->data);
                blob_set_data(blob1, data_joined);
                blob_destroy(blob2);
            }
        }
    }
    return blob1;
}

void blob_print_all(Blob* blob) {
    if (blob) {
        if (blob->data) {
            printf("%s", blob->data);
        }
        blob_print_all(blob->next);
    }
}

int main(int argc, char* argv[argc + 1]) {
    FILE* file = fopen("text.txt", "r");
    if (!file) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }
    char* line = 0;
    size_t len = 0;
    ssize_t read = 0;
    Blob* head = 0, *tail = 0;
    while ((read = getline(&line, &len, file)) != -1) {
        Blob* new_blob = blob_new(line);
        if (new_blob) {
            if (tail) {
                tail->next = new_blob;
                new_blob->prev = tail;
                tail = new_blob;
            } else {
                head = new_blob;
                tail = head;
            }
        }
    }
    blob_print_all(head);

    fclose(file);
    return EXIT_SUCCESS;
}