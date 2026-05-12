#include "cnumkit.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cnumkit/contracts.h"

cnk_vector *cnk_vector_create(size_t size) {
    CNK_REQUIRES(size > 0);

    if (size == 0) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Vector size must be greater than zero");
        return NULL;
    }

    if (size > SIZE_MAX / sizeof(double)) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Vector size overflows allocation size");
        return NULL;
    }

    cnk_vector *v = malloc(sizeof(cnk_vector));
    if (!v) {
        cnk_set_last_error(CNK_ERROR_ALLOCATION, "Failed to allocate vector structure");
        return NULL;
    }

    v->data = calloc(size, sizeof(double));
    if (!v->data) {
        free(v);
        cnk_set_last_error(CNK_ERROR_ALLOCATION, "Failed to allocate vector data");
        return NULL;
    }

    v->size = size;

    CNK_ENSURES(v != NULL);
    CNK_ENSURES(v->size == size);

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
    CNK_REQUIRES(v != NULL);
    CNK_REQUIRES(index < v->size);

    if (!v || !v->data || index >= v->size) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Invalid vector access");
        return 0.0;
    }

    return v->data[index];
}

int cnk_vector_set(cnk_vector *v, size_t index, double value) {
    if (!v || !v->data || index >= v->size) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Invalid vector write");
        return -1;
    }

    if (!isfinite(value)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Vector value must be finite");
        return -1;
    }

    v->data[index] = value;
    return 0;
}

int cnk_vector_dot(const cnk_vector *a, const cnk_vector *b, double *result) {
    CNK_REQUIRES(a != NULL && b != NULL);
    CNK_REQUIRES(a->size == b->size);
    CNK_REQUIRES(result != NULL);

    if (!a || !b || !result || !a->data || !b->data) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Null pointer passed to cnk_vector_dot");
        return -1;
    }

    if (a->size != b->size) {
        cnk_set_last_error(CNK_ERROR_DIMENSION_MISMATCH, "Vector dimensions do not match for dot product");
        return -1;
    }

    double sum = 0.0;

    for (size_t i = 0; i < a->size; i++) {
        sum += a->data[i] * b->data[i];
    }

    *result = sum;
    cnk_set_last_error(CNK_SUCCESS, NULL);
    return 0;
}

int cnk_vector_norm2(const cnk_vector *v, double *result) {
    if (!v || !result || !v->data) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Null pointer passed to cnk_vector_norm2");
        return -1;
    }

    double dot = 0.0;

    if (cnk_vector_dot(v, v, &dot) != 0) {
        return -1;
    }

    *result = sqrt(dot);
    cnk_set_last_error(CNK_SUCCESS, NULL);
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
