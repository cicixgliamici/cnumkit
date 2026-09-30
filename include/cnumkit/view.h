#ifndef CNUMKIT_VIEW_H
#define CNUMKIT_VIEW_H

/**
 * @file view.h
 * @brief Non-owning views over caller-managed double-precision data.
 *
 * Strides are measured in double elements, not bytes. data points to logical
 * element zero, including for reversed views. Kernel validation requires
 * nonzero strides on axes longer than one element and distinct matrix elements
 * (no broadcasting). The sum of axis spans in bytes must fit PTRDIFF_MAX.
 * Empty views may have NULL data and arbitrary unused strides/dimensions.
 * Single-element axes ignore their stride. The caller must provide aligned,
 * live storage containing every addressed element; metadata checks cannot
 * verify allocation bounds. Constructors only assemble descriptors: they do
 * not validate or change thread-local errors. Kernels validate before access.
 */

#include <stddef.h>

#include "cnumkit_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read-only logical vector over caller-owned storage.
 *
 * A negative stride is valid when data points to the first logical element.
 * Empty views may use a NULL data pointer. The view never owns its storage.
 */
typedef struct {
    const double *data;
    size_t size;
    ptrdiff_t stride;
} cnk_f64_const_view;

/**
 * @brief Mutable logical vector over caller-owned storage.
 *
 * A negative stride is valid when data points to the first logical element.
 * Empty views may use a NULL data pointer. The view never owns its storage.
 */
typedef struct {
    double *data;
    size_t size;
    ptrdiff_t stride;
} cnk_f64_view;

/**
 * @brief Read-only logical matrix over caller-owned storage.
 *
 * Addressing follows data[row * row_stride + column * column_stride]. The
 * caller must ensure that every addressed element belongs to a live object.
 */
typedef struct {
    const double *data;
    size_t rows;
    size_t cols;
    ptrdiff_t row_stride;
    ptrdiff_t column_stride;
} cnk_f64_const_matrix_view;

/**
 * @brief Mutable logical matrix over caller-owned storage.
 *
 * Addressing follows data[row * row_stride + column * column_stride]. The
 * caller must ensure that every addressed element belongs to a live object.
 */
typedef struct {
    double *data;
    size_t rows;
    size_t cols;
    ptrdiff_t row_stride;
    ptrdiff_t column_stride;
} cnk_f64_matrix_view;

/** @brief Creates a borrowed mutable vector view without allocating memory. */
CNUMKIT_EXPORT cnk_f64_view cnk_f64_view_make(
    double *data,
    size_t size,
    ptrdiff_t stride);

/** @brief Creates a borrowed read-only vector view without allocating memory. */
CNUMKIT_EXPORT cnk_f64_const_view cnk_f64_const_view_make(
    const double *data,
    size_t size,
    ptrdiff_t stride);

/** @brief Creates a borrowed mutable matrix view without allocating memory. */
CNUMKIT_EXPORT cnk_f64_matrix_view cnk_f64_matrix_view_make(
    double *data,
    size_t rows,
    size_t cols,
    ptrdiff_t row_stride,
    ptrdiff_t column_stride);

/** @brief Creates a borrowed read-only matrix view without allocating memory. */
CNUMKIT_EXPORT cnk_f64_const_matrix_view cnk_f64_const_matrix_view_make(
    const double *data,
    size_t rows,
    size_t cols,
    ptrdiff_t row_stride,
    ptrdiff_t column_stride);

#ifdef __cplusplus
}
#endif

#endif /* CNUMKIT_VIEW_H */
