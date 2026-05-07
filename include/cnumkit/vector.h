#ifndef CNUMKIT_VECTOR_H
#define CNUMKIT_VECTOR_H

#include <stddef.h>

typedef struct {
    size_t size;
    double *data;
} cnk_vector;

/*
 * Creates a vector of the given size.
 * The memory is initialized to zero.
 * Returns NULL on allocation failure.
 */
cnk_vector *cnk_vector_create(size_t size);

/*
 * Frees a vector created with cnk_vector_create.
 */
void cnk_vector_free(cnk_vector *v);

/*
 * Gets the value at index.
 * Returns 0.0 if the vector is NULL or index is invalid.
 */
double cnk_vector_get(const cnk_vector *v, size_t index);

/*
 * Sets the value at index.
 * Returns 0 on success, -1 on error.
 */
int cnk_vector_set(cnk_vector *v, size_t index, double value);

/*
 * Computes the dot product between two vectors.
 * Returns 0 on success, -1 on error.
 */
int cnk_vector_dot(const cnk_vector *a, const cnk_vector *b, double *result);

/*
 * Computes the Euclidean norm.
 * Returns 0 on success, -1 on error.
 */
int cnk_vector_norm2(const cnk_vector *v, double *result);

/*
 * Prints a vector to stdout.
 */
void cnk_vector_print(const cnk_vector *v);

#endif
