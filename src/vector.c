#include "cnumkit/vector.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

cnk_vector *cnk_vector_create(size_t size) {
    if (size == 0) {
        return NULL;
    }

    cnk_vector *v = malloc(sizeof(cnk_vector));
    if (!v) {
        return NULL;
    }

    v->data = calloc(size, sizeof(double));
    if (!v->data) {
        free(v);
        return NULL;
    }

    v->size = size;
    return v;
}

void cnk_vector_free(cnk_vector *v) {
    if (!v) {
        return;
    }

    free(v->data);
    free(v);
}

double cnk_vector_get(const cnk_vector *v, size_t index) {
    if (!v || !v->data || index >= v->size) {
        return 0.0;
    }

    return v->data[index];
}

int cnk_vector_set(cnk_vector *v, size_t index, double value) {
    if (!v || !v->data || index >= v->size) {
        return -1;
    }

    v->data[index] = value;
    return 0;
}

int cnk_vector_dot(const cnk_vector *a, const cnk_vector *b, double *result) {
    if (!a || !b || !result || !a->data || !b->data) {
        return -1;
    }

    if (a->size != b->size) {
        return -1;
    }

    double sum = 0.0;

    for (size_t i = 0; i < a->size; i++) {
        sum += a->data[i] * b->data[i];
    }

    *result = sum;
    return 0;
}

int cnk_vector_norm2(const cnk_vector *v, double *result) {
    if (!v || !result || !v->data) {
        return -1;
    }

    double dot = 0.0;

    if (cnk_vector_dot(v, v, &dot) != 0) {
        return -1;
    }

    *result = sqrt(dot);
    return 0;
}

void cnk_vector_print(const cnk_vector *v) {
    if (!v || !v->data) {
        printf("(null vector)\n");
        return;
    }

    printf("[");
    for (size_t i = 0; i < v->size; i++) {
        printf("%.6f", v->data[i]);

        if (i + 1 < v->size) {
            printf(", ");
        }
    }
    printf("]\n");
}
