#include "cnumkit.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cnumkit/contracts.h"

/* Centralizing row-major indexing prevents layout calculations from drifting between operations. */
static size_t cnk_matrix_index(const cnk_matrix *m, size_t row, size_t col) {
    return row * m->cols + col;
}

/* Validate both dimension multiplication and the later byte-size multiplication. */
static int cnk_checked_matrix_count(size_t rows, size_t cols, size_t *count) {
    if (!count || rows == 0 || cols == 0) {
        return -1;
    }

    if (rows > SIZE_MAX / cols) {
        return -1;
    }

    *count = rows * cols;
    if (*count > SIZE_MAX / sizeof(double)) {
        return -1;
    }

    return 0;
}

cnk_matrix *cnk_matrix_create(size_t rows, size_t cols) {
    CNK_REQUIRES(rows > 0 && cols > 0);

    if (rows == 0 || cols == 0) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Matrix dimensions must be greater than zero");
        return NULL;
    }

    size_t element_count = 0;
    if (cnk_checked_matrix_count(rows, cols, &element_count) != 0) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Matrix dimensions overflow size_t");
        return NULL;
    }

    cnk_matrix *m = malloc(sizeof(cnk_matrix));
    if (!m) {
        cnk_set_last_error(CNK_ERROR_ALLOCATION, "Failed to allocate matrix structure");
        return NULL;
    }

    m->data = calloc(element_count, sizeof(double));
    if (!m->data) {
        free(m);
        cnk_set_last_error(CNK_ERROR_ALLOCATION, "Failed to allocate matrix data");
        return NULL;
    }

    m->rows = rows;
    m->cols = cols;

    CNK_ENSURES(m != NULL);
    CNK_ENSURES(m->rows == rows && m->cols == cols);

    cnk_set_last_error(CNK_SUCCESS, NULL);
    return m;
}

void cnk_matrix_free(cnk_matrix *m) {
    if (!m) {
        cnk_set_last_error(CNK_SUCCESS, NULL);
        return;
    }

    free(m->data);
    free(m);
    cnk_set_last_error(CNK_SUCCESS, NULL);
}

double cnk_matrix_get(const cnk_matrix *m, size_t row, size_t col) {
    CNK_REQUIRES(m != NULL);
    CNK_REQUIRES(row < m->rows && col < m->cols);

    if (!m || !m->data || row >= m->rows || col >= m->cols) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Invalid matrix access");
        return 0.0;
    }

    double value = m->data[cnk_matrix_index(m, row, col)];
    cnk_set_last_error(CNK_SUCCESS, NULL);
    return value;
}

int cnk_matrix_set(cnk_matrix *m, size_t row, size_t col, double value) {
    CNK_REQUIRES(m != NULL);
    CNK_REQUIRES(row < m->rows && col < m->cols);

    if (!m || !m->data || row >= m->rows || col >= m->cols) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Invalid matrix write");
        return -1;
    }

    if (!isfinite(value)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Matrix value must be finite");
        return -1;
    }

    m->data[cnk_matrix_index(m, row, col)] = value;
    cnk_set_last_error(CNK_SUCCESS, NULL);
    return 0;
}

cnk_matrix *cnk_matrix_identity(size_t n) {
    CNK_REQUIRES(n > 0);

    cnk_matrix *m = cnk_matrix_create(n, n);
    if (!m) {
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        if (cnk_matrix_set(m, i, i, 1.0) != 0) {
            cnk_matrix_free(m);
            return NULL;
        }
    }

    cnk_set_last_error(CNK_SUCCESS, NULL);
    return m;
}

cnk_matrix *cnk_matrix_multiply(const cnk_matrix *a, const cnk_matrix *b) {
    CNK_REQUIRES(a != NULL && b != NULL);
    CNK_REQUIRES(a->cols == b->rows);

    if (!a || !b || !a->data || !b->data) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Null pointer passed to cnk_matrix_multiply");
        return NULL;
    }

    if (a->cols != b->rows) {
        cnk_set_last_error(CNK_ERROR_DIMENSION_MISMATCH, "Matrix dimensions are incompatible for multiplication");
        return NULL;
    }

    size_t element_count = 0;
    if (cnk_checked_matrix_count(a->rows, b->cols, &element_count) != 0) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Result matrix dimensions overflow size_t");
        return NULL;
    }

    cnk_matrix *result = cnk_matrix_create(a->rows, b->cols);
    if (!result) {
        /* Preserve the more specific allocation or dimension error from the constructor. */
        return NULL;
    }

    /* Keep the simple O(n^3) loop order as the readable scalar reference implementation. */
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            double sum = 0.0;

            for (size_t k = 0; k < a->cols; k++) {
                sum += cnk_matrix_get(a, i, k) * cnk_matrix_get(b, k, j);
            }

            if (!isfinite(sum) || cnk_matrix_set(result, i, j, sum) != 0) {
                cnk_matrix_free(result);
                cnk_set_last_error(CNK_ERROR_MATH, "Matrix multiplication produced a non-finite result");
                return NULL;
            }
        }
    }

    cnk_set_last_error(CNK_SUCCESS, NULL);
    return result;
}

void cnk_matrix_print(const cnk_matrix *m) {
    if (!m || !m->data) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Invalid matrix passed to cnk_matrix_print");
        printf("(null matrix)\n");
        return;
    }

    for (size_t i = 0; i < m->rows; i++) {
        printf("[");
        for (size_t j = 0; j < m->cols; j++) {
            printf("%10.6f", cnk_matrix_get(m, i, j));

            if (j + 1 < m->cols) {
                printf(", ");
            }
        }
        printf("]\n");
    }
    cnk_set_last_error(CNK_SUCCESS, NULL);
}
