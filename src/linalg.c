#include "cnumkit.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "cnumkit/contracts.h"

#define CNK_EPSILON 1e-12

static void swap_rows(double *data, size_t cols, size_t row_a, size_t row_b) {
    if (row_a == row_b) {
        return;
    }

    for (size_t j = 0; j < cols; j++) {
        double temp = data[row_a * cols + j];
        data[row_a * cols + j] = data[row_b * cols + j];
        data[row_b * cols + j] = temp;
    }
}

cnk_vector *cnk_linalg_solve_gaussian(const cnk_matrix *A, const cnk_vector *b) {
    CNK_REQUIRES(A != NULL && b != NULL);
    CNK_REQUIRES(A->rows == A->cols);
    CNK_REQUIRES(A->rows == b->size);

    if (!A || !b || !A->data || !b->data) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Null pointer passed to cnk_linalg_solve_gaussian");
        return NULL;
    }

    if (A->rows != A->cols || A->rows != b->size) {
        cnk_set_last_error(CNK_ERROR_DIMENSION_MISMATCH, "Linear system dimensions are incompatible");
        return NULL;
    }

    size_t n = A->rows;
    if (n == 0 || n == SIZE_MAX) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Linear system size is invalid");
        return NULL;
    }

    /*
     * We create an augmented matrix [A | b].
     * Dimensions: n x (n + 1)
     */
    size_t aug_cols = n + 1;
    if (n > SIZE_MAX / aug_cols) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Augmented matrix dimensions overflow size_t");
        return NULL;
    }

    double *aug = calloc(n * aug_cols, sizeof(double));
    if (!aug) {
        cnk_set_last_error(CNK_ERROR_ALLOCATION, "Failed to allocate augmented matrix");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            aug[i * aug_cols + j] = cnk_matrix_get(A, i, j);
        }

        aug[i * aug_cols + n] = cnk_vector_get(b, i);
    }

    /*
     * Forward elimination with partial pivoting.
     */
    for (size_t k = 0; k < n; k++) {
        size_t pivot_row = k;
        double max_abs = fabs(aug[k * aug_cols + k]);

        for (size_t i = k + 1; i < n; i++) {
            double value = fabs(aug[i * aug_cols + k]);

            if (value > max_abs) {
                max_abs = value;
                pivot_row = i;
            }
        }

        if (max_abs < CNK_EPSILON) {
            free(aug);
            cnk_set_last_error(CNK_ERROR_SINGULAR_MATRIX, "Matrix is singular or nearly singular");
            return NULL;
        }

        swap_rows(aug, aug_cols, k, pivot_row);

        for (size_t i = k + 1; i < n; i++) {
            double factor = aug[i * aug_cols + k] / aug[k * aug_cols + k];

            for (size_t j = k; j < aug_cols; j++) {
                aug[i * aug_cols + j] -= factor * aug[k * aug_cols + j];
            }
        }
    }

    /*
     * Back substitution.
     */
    cnk_vector *x = cnk_vector_create(n);
    if (!x) {
        free(aug);
        return NULL;
    }

    for (size_t i = n; i-- > 0;) {
        double sum = aug[i * aug_cols + n];

        for (size_t j = i + 1; j < n; j++) {
            sum -= aug[i * aug_cols + j] * cnk_vector_get(x, j);
        }

        double diagonal = aug[i * aug_cols + i];

        if (fabs(diagonal) < CNK_EPSILON) {
            cnk_vector_free(x);
            free(aug);
            cnk_set_last_error(CNK_ERROR_SINGULAR_MATRIX, "Matrix is singular or nearly singular");
            return NULL;
        }

        cnk_vector_set(x, i, sum / diagonal);
    }

    free(aug);
    cnk_set_last_error(CNK_SUCCESS, NULL);
    return x;
}
