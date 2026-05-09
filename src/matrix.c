#include "cnumkit.h"

#include <stdio.h>
#include <stdlib.h>

#include "cnumkit/contracts.h"

static size_t cnk_matrix_index(const cnk_matrix *m, size_t row, size_t col) {
    return row * m->cols + col;
}

cnk_matrix *cnk_matrix_create(size_t rows, size_t cols) {
    CNK_REQUIRES(rows > 0 && cols > 0);

    if (rows == 0 || cols == 0) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Matrix dimensions must be greater than zero");
        return NULL;
    }

    cnk_matrix *m = malloc(sizeof(cnk_matrix));
    if (!m) {
        cnk_set_last_error(CNK_ERROR_ALLOCATION, "Failed to allocate matrix structure");
        return NULL;
    }

    m->data = calloc(rows * cols, sizeof(double));
    if (!m->data) {
        free(m);
        cnk_set_last_error(CNK_ERROR_ALLOCATION, "Failed to allocate matrix data");
        return NULL;
    }

    m->rows = rows;
    m->cols = cols;

    CNK_ENSURES(m != NULL);
    CNK_ENSURES(m->rows == rows && m->cols == cols);

    return m;
}

void cnk_matrix_free(cnk_matrix *m) {
    if (!m) {
        return;
    }

    free(m->data);
    free(m);
}

double cnk_matrix_get(const cnk_matrix *m, size_t row, size_t col) {
    CNK_REQUIRES(m != NULL);
    CNK_REQUIRES(row < m->rows && col < m->cols);

    if (!m || !m->data || row >= m->rows || col >= m->cols) {
        return 0.0;
    }

    return m->data[cnk_matrix_index(m, row, col)];
}

int cnk_matrix_set(cnk_matrix *m, size_t row, size_t col, double value) {
    CNK_REQUIRES(m != NULL);
    CNK_REQUIRES(row < m->rows && col < m->cols);

    if (!m || !m->data || row >= m->rows || col >= m->cols) {
        return -1;
    }

    m->data[cnk_matrix_index(m, row, col)] = value;
    return 0;
}

cnk_matrix *cnk_matrix_identity(size_t n) {
    CNK_REQUIRES(n > 0);

    cnk_matrix *m = cnk_matrix_create(n, n);
    if (!m) {
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        cnk_matrix_set(m, i, i, 1.0);
    }

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

    cnk_matrix *result = cnk_matrix_create(a->rows, b->cols);
    if (!result) {
        // Error already set by cnk_matrix_create
        return NULL;
    }

    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            double sum = 0.0;

            for (size_t k = 0; k < a->cols; k++) {
                sum += cnk_matrix_get(a, i, k) * cnk_matrix_get(b, k, j);
            }

            cnk_matrix_set(result, i, j, sum);
        }
    }

    return result;
}

void cnk_matrix_print(const cnk_matrix *m) {
    if (!m || !m->data) {
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
}
