#ifndef CNUMKIT_LINALG_H
#define CNUMKIT_LINALG_H

#include "cnumkit/matrix.h"
#include "cnumkit/vector.h"

/*
 * Solves a linear system Ax = b using Gaussian elimination
 * with partial pivoting.
 *
 * A must be square.
 * b must have the same size as A rows.
 *
 * Returns a newly allocated vector x, or NULL on error.
 */
cnk_vector *cnk_linalg_solve_gaussian(const cnk_matrix *A, const cnk_vector *b);

#endif
