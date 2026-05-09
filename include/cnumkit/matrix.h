#ifndef CNUMKIT_MATRIX_H
#define CNUMKIT_MATRIX_H

/**
 * @file matrix.h
 * @brief Matrix operations and memory management.
 */

#include <stddef.h>
#include "cnumkit_export.h"

/**
 * @brief Represents a dense 2D matrix stored in row-major order.
 */
typedef struct {
    size_t rows;  /**< Number of rows in the matrix */
    size_t cols;  /**< Number of columns in the matrix */
    double *data; /**< Flat array containing matrix elements (row-major) */
} cnk_matrix;

/**
 * @brief Creates a new matrix with the specified dimensions.
 * 
 * The memory is allocated dynamically and all elements are initialized to zero.
 * 
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @return A pointer to the newly created matrix, or NULL if memory allocation fails.
 */
/*@
  requires rows > 0 && cols > 0;
  ensures \result == \null || (\result->rows == rows && \result->cols == cols && \valid(\result->data + (0 .. rows * cols - 1)));
*/
CNUMKIT_EXPORT cnk_matrix *cnk_matrix_create(size_t rows, size_t cols);

/**
 * @brief Frees the memory associated with a matrix.
 * 
 * @param m Pointer to the matrix to free. If m is NULL, the function does nothing.
 */
/*@
  requires m == \null || \valid(m);
*/
CNUMKIT_EXPORT void cnk_matrix_free(cnk_matrix *m);

/**
 * @brief Retrieves the value at a specific row and column in the matrix.
 * 
 * @param m Pointer to the matrix.
 * @param row The 0-based row index.
 * @param col The 0-based column index.
 * @return The value at (row, col), or 0.0 if out of bounds or m is NULL.
 */
/*@
  requires \valid(m);
  requires row < m->rows && col < m->cols;
*/
CNUMKIT_EXPORT double cnk_matrix_get(const cnk_matrix *m, size_t row, size_t col);

/**
 * @brief Sets the value at a specific row and column in the matrix.
 * 
 * @param m Pointer to the matrix.
 * @param row The 0-based row index.
 * @param col The 0-based column index.
 * @param value The value to set.
 * @return 0 on success, or -1 if out of bounds or m is NULL.
 */
/*@
  requires \valid(m);
  requires row < m->rows && col < m->cols;
*/
CNUMKIT_EXPORT int cnk_matrix_set(cnk_matrix *m, size_t row, size_t col, double value);

/**
 * @brief Creates an identity matrix of size n x n.
 * 
 * All diagonal elements are set to 1.0, and all other elements are 0.0.
 * 
 * @param n Size of the square identity matrix.
 * @return A pointer to the identity matrix, or NULL on allocation failure.
 */
/*@
  requires n > 0;
  ensures \result == \null || (\result->rows == n && \result->cols == n);
*/
CNUMKIT_EXPORT cnk_matrix *cnk_matrix_identity(size_t n);

/**
 * @brief Multiplies two matrices: result = a * b.
 * 
 * The number of columns in 'a' must equal the number of rows in 'b'.
 * If dimensions mismatch, an error is set and NULL is returned.
 * 
 * @param a The left matrix.
 * @param b The right matrix.
 * @return A newly allocated matrix containing the result, or NULL on error.
 */
/*@
  requires \valid(a) && \valid(b);
  requires a->cols == b->rows;
  ensures \result == \null || (\result->rows == a->rows && \result->cols == b->cols);
*/
CNUMKIT_EXPORT cnk_matrix *cnk_matrix_multiply(const cnk_matrix *a, const cnk_matrix *b);

/**
 * @brief Prints the matrix to the standard output.
 * 
 * Each row is printed on a new line enclosed in brackets.
 * 
 * @param m Pointer to the matrix.
 */
CNUMKIT_EXPORT void cnk_matrix_print(const cnk_matrix *m);

#endif /* CNUMKIT_MATRIX_H */
