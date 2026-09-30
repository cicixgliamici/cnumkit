#include "cnumkit/adapters.h"
#include "kernel_internal.h"

#include <stdint.h>

cnk_error_code cnk_vector_as_const_view(const cnk_vector *owner, cnk_f64_const_view *output) {
    if (!owner || !output || owner->size == 0
        || !cnk_vector_layout_valid(owner->data, owner->size, 1)) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    *output = cnk_f64_const_view_make(owner->data, owner->size, 1);
    return cnk_kernel_status(CNK_SUCCESS);
}

cnk_error_code cnk_vector_as_view(cnk_vector *owner, cnk_f64_view *output) {
    if (!output) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    cnk_f64_const_view borrowed;
    cnk_error_code status = cnk_vector_as_const_view(owner, &borrowed);
    if (status == CNK_SUCCESS) {
        *output = cnk_f64_view_make(owner->data, borrowed.size, borrowed.stride);
    }
    return status;
}

cnk_error_code cnk_matrix_as_const_view(const cnk_matrix *owner,
    cnk_f64_const_matrix_view *output) {
    /* Validate before the unsigned-to-signed row-stride conversion. */
    if (!owner || !output || owner->rows == 0 || owner->cols == 0
        || owner->cols > (size_t)PTRDIFF_MAX) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    cnk_f64_const_matrix_view borrowed = cnk_f64_const_matrix_view_make(
        owner->data, owner->rows, owner->cols, (ptrdiff_t)owner->cols, 1);
    if (!cnk_matrix_layout_valid(borrowed)) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    *output = borrowed;
    return cnk_kernel_status(CNK_SUCCESS);
}

cnk_error_code cnk_matrix_as_view(cnk_matrix *owner, cnk_f64_matrix_view *output) {
    if (!output) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    cnk_f64_const_matrix_view borrowed;
    cnk_error_code status = cnk_matrix_as_const_view(owner, &borrowed);
    if (status == CNK_SUCCESS) {
        *output = cnk_f64_matrix_view_make(owner->data, borrowed.rows, borrowed.cols,
            borrowed.row_stride, borrowed.column_stride);
    }
    return status;
}
