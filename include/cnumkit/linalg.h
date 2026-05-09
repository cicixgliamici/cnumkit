#ifndef CNUMKIT_LINALG_H
#define CNUMKIT_LINALG_H

/**
 * @file linalg.h
 * @brief Linear algebra solvers and algorithms.
 */

#include "cnumkit/matrix.h"
#include "cnumkit/vector.h"
#include "cnumkit_export.h"

/**
 * @brief Solves a system of linear equations Ax = b using Gaussian elimination.
 * 
 * The algorithm uses partial pivoting for improved numerical stability.
 * The matrix A must be a square matrix, and the vector b must have the same 
 * number of elements as there are rows in A.
 * 
 * @param A The square coefficient matrix.
 * @param b The right-hand side constant vector.
 * @return A newly allocated vector x containing the solution, or NULL if the 
 *         matrix is singular or dimensions mismatch.
 */
/*@
  requires \valid(A) && \valid(b);
  requires A->rows == A->cols;
  requires A->rows == b->size;
*/
CNUMKIT_EXPORT cnk_vector *cnk_linalg_solve_gaussian(const cnk_matrix *A, const cnk_vector *b);

#endif /* CNUMKIT_LINALG_H */
