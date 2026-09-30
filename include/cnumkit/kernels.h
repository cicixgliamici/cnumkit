#ifndef CNUMKIT_KERNELS_H
#define CNUMKIT_KERNELS_H

/**
 * @file kernels.h
 * @brief Portable scalar kernels over non-owning double-precision views.
 *
 * Kernels allocate no memory and return a status while updating thread-local
 * diagnostics. On failure, outputs are unchanged. Inputs and arithmetic must
 * be finite; fill/copy/matvec/matmul overwrite destinations without reading
 * their previous values. Empty outputs perform no numerical input reads.
 *
 * The caller must keep storage live and prevent concurrent mutation. Cross-view
 * overlap is a caller precondition, not detected at runtime: copy/AXPY permit
 * exact aliasing (same data, size and stride); matvec/matmul require output
 * elements disjoint from all input elements. Inputs may overlap each other.
 * dot/norm2 may write their scalar result into an input element after reduction.
 */

#include "cnumkit/error.h"
#include "cnumkit/view.h"
#include "cnumkit_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fills every element of a mutable view with one finite value.
 * @return CNK_SUCCESS or a specific error code.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_fill(cnk_f64_view destination, double value);

/**
 * @brief Multiplies every element of a mutable view by a finite scalar.
 *
 * The destination remains unchanged if validation or arithmetic fails.
 *
 * @return CNK_SUCCESS or a specific error code.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_scale(cnk_f64_view values, double factor);

/**
 * @brief Computes y = alpha * x + y over equally sized views.
 *
 * Exact in-place aliasing between x and y is supported. Other overlapping
 * layouts are not supported. The destination remains unchanged on failure.
 *
 * @return CNK_SUCCESS or a specific error code.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_axpy(
    double alpha,
    cnk_f64_const_view x,
    cnk_f64_view y);

/**
 * @brief Computes the dot product of two equally sized views.
 *
 * Empty views produce zero. Result remains unchanged on failure.
 *
 * @return CNK_SUCCESS or a specific error code.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_dot(
    cnk_f64_const_view left,
    cnk_f64_const_view right,
    double *result);

/**
 * @brief Copies finite source elements to an equally sized destination.
 * @return CNK_SUCCESS, CNK_ERROR_INVALID_ARGUMENT, CNK_ERROR_DIMENSION_MISMATCH,
 * or CNK_ERROR_MATH. Empty copies succeed; see the module's aliasing contract.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_copy(
    cnk_f64_const_view source, cnk_f64_view destination);

/**
 * @brief Computes a scaled sum-of-squares L2 norm without squaring raw inputs.
 * @return CNK_SUCCESS, CNK_ERROR_INVALID_ARGUMENT, or CNK_ERROR_MATH.
 * Empty/all-zero views produce zero. Non-finite input or an unrepresentable
 * final norm is an error. Tiny values are retained subject to floating-point
 * rounding and the platform's subnormal handling.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_norm2(cnk_f64_const_view values, double *result);

/**
 * @brief Overwrites output with matrix * input, using arbitrary valid strides.
 * @return CNK_SUCCESS, CNK_ERROR_INVALID_ARGUMENT, CNK_ERROR_DIMENSION_MISMATCH,
 * or CNK_ERROR_MATH.
 * Requires matrix.cols == input.size and matrix.rows == output.size.
 * A zero inner dimension writes zeros. Output must not overlap inputs.
 * Two computation passes preserve output on failure without allocating scratch.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_matvec(cnk_f64_const_matrix_view matrix,
    cnk_f64_const_view input, cnk_f64_view output);

/**
 * @brief Overwrites output with left * right, using arbitrary valid strides.
 * @return CNK_SUCCESS, CNK_ERROR_INVALID_ARGUMENT, CNK_ERROR_DIMENSION_MISMATCH,
 * or CNK_ERROR_MATH.
 * Requires left.cols == right.rows and output shape (left.rows, right.cols).
 * A zero inner dimension writes zeros. Output must not overlap inputs.
 * Two computation passes preserve output on failure without allocating scratch.
 * Like dot, accumulation uses platform long double, without exactness or
 * cross-platform bitwise reproducibility guarantees.
 */
CNUMKIT_EXPORT cnk_error_code cnk_f64_matmul(cnk_f64_const_matrix_view left,
    cnk_f64_const_matrix_view right, cnk_f64_matrix_view output);

#ifdef __cplusplus
}
#endif

#endif /* CNUMKIT_KERNELS_H */
