#ifndef CNUMKIT_ADAPTERS_H
#define CNUMKIT_ADAPTERS_H

/**
 * @file adapters.h
 * @brief Checked zero-copy adapters for legacy owned vectors and matrices.
 *
 * Adapters never allocate or transfer ownership. Borrowed views expire when
 * their owner is freed or its storage is replaced. Const adapters expose
 * read-only data. Invalid owners (including zero dimensions), NULL outputs,
 * and overflowing layouts return CNK_ERROR_INVALID_ARGUMENT without modifying
 * output. Success returns CNK_SUCCESS; both paths update thread-local errors.
 * Adapters check metadata only, not element values or allocation bounds.
 */

#include "cnumkit/error.h"
#include "cnumkit/vector.h"
#include "cnumkit/matrix.h"
#include "cnumkit/view.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Borrows a mutable contiguous view from an owned vector. */
CNUMKIT_EXPORT cnk_error_code cnk_vector_as_view(cnk_vector *owner, cnk_f64_view *output);
/** @brief Borrows a read-only contiguous view from an owned vector. */
CNUMKIT_EXPORT cnk_error_code cnk_vector_as_const_view(
    const cnk_vector *owner, cnk_f64_const_view *output);
/** @brief Borrows a mutable row-major view from an owned matrix. */
CNUMKIT_EXPORT cnk_error_code cnk_matrix_as_view(
    cnk_matrix *owner, cnk_f64_matrix_view *output);
/** @brief Borrows a read-only row-major view from an owned matrix. */
CNUMKIT_EXPORT cnk_error_code cnk_matrix_as_const_view(
    const cnk_matrix *owner, cnk_f64_const_matrix_view *output);

#ifdef __cplusplus
}
#endif

#endif
