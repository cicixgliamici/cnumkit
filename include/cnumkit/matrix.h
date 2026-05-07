#ifndef CNUMKIT_MATRIX_H
#define CNUMKIT_MATRIX_H

#include <stddef.h>

typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} cnk_matrix;

/*
 * Creates a rows x cols matrix.
 * Data is stored in row-major order.
 * The memory is initialized to zero.
 */
cnk_matrix *cnk_matrix_create(size_t rows, size_t cols);

/*
 * Frees a matrix created with cnk_matrix_create.
 */
void cnk_matrix_free(cnk_matrix *m);

/*
 * Gets the value at position row, col.
 * Returns 0.0 if matrix is NULL or indices are invalid.
 */
double cnk_matrix_get(const cnk_matrix *m, size_t row, size_t col);

/*
 * Sets the value at position row, col.
 * Returns 0 on success, -1 on error.
 */
int cnk_matrix_set(cnk_matrix *m, size_t row, size_t col, double value);

/*
 * Creates an identity matrix of size n x n.
 */
cnk_matrix *cnk_matrix_identity(size_t n);

/*
 * Matrix multiplication: result = a * b.
 * Returns a newly allocated matrix, or NULL on error.
 */
cnk_matrix *cnk_matrix_multiply(const cnk_matrix *a, const cnk_matrix *b);

/*
 * Prints a matrix to stdout.
 */
void cnk_matrix_print(const cnk_matrix *m);

#endif
