#ifndef CNUMKIT_VECTOR_H
#define CNUMKIT_VECTOR_H

/**
 * @file vector.h
 * @brief Vector operations and memory management.
 */

#include <stddef.h>
#include "cnumkit_export.h"

/**
 * @brief Represents a mathematical column vector.
 */
typedef struct {
    size_t size;  /**< Number of elements in the vector */
    double *data; /**< Array of vector elements */
} cnk_vector;

/**
 * @brief Creates a new vector of the specified size.
 * 
 * The memory is allocated dynamically and initialized to zero.
 * 
 * @param size Number of elements.
 * @return A pointer to the newly created vector, or NULL if memory allocation fails.
 */
CNUMKIT_EXPORT cnk_vector *cnk_vector_create(size_t size);

/**
 * @brief Frees the memory associated with a vector.
 * 
 * @param v Pointer to the vector to free. If v is NULL, the function does nothing.
 */
CNUMKIT_EXPORT void cnk_vector_free(cnk_vector *v);

/**
 * @brief Retrieves the value at a specific index in the vector.
 * 
 * @param v Pointer to the vector.
 * @param index The 0-based index of the element.
 * @return The value at the specified index, or 0.0 if the index is out of bounds or v is NULL.
 */
CNUMKIT_EXPORT double cnk_vector_get(const cnk_vector *v, size_t index);

/**
 * @brief Sets the value at a specific index in the vector.
 * 
 * @param v Pointer to the vector.
 * @param index The 0-based index of the element.
 * @param value The value to set.
 * @return 0 on success, or -1 if the index is out of bounds or v is NULL.
 */
CNUMKIT_EXPORT int cnk_vector_set(cnk_vector *v, size_t index, double value);

/**
 * @brief Computes the dot product (scalar product) of two vectors.
 * 
 * Both vectors must have the same size. If they do not, the function sets
 * the thread-local error state.
 * 
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param result Pointer to a double where the result will be stored.
 * @return 0 on success, or -1 on dimension mismatch or NULL pointers.
 */
CNUMKIT_EXPORT int cnk_vector_dot(const cnk_vector *a, const cnk_vector *b, double *result);

/**
 * @brief Computes the Euclidean norm (L2 norm) of the vector.
 * 
 * @param v Pointer to the vector.
 * @param result Pointer to a double where the calculated norm will be stored.
 * @return 0 on success, or -1 if v is NULL.
 */
CNUMKIT_EXPORT int cnk_vector_norm2(const cnk_vector *v, double *result);

/**
 * @brief Prints the vector to the standard output.
 * 
 * Formats the vector as a horizontal array enclosed in brackets.
 * 
 * @param v Pointer to the vector.
 */
CNUMKIT_EXPORT void cnk_vector_print(const cnk_vector *v);

#endif /* CNUMKIT_VECTOR_H */
