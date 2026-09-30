#ifndef CNK_KERNEL_INTERNAL_H
#define CNK_KERNEL_INTERNAL_H

#include "cnumkit/kernels.h"

/* Metadata validation cannot establish the size or lifetime of an allocation. */
int cnk_vector_layout_valid(const void *data, size_t size, ptrdiff_t stride);
int cnk_matrix_layout_valid(cnk_f64_const_matrix_view matrix);
cnk_error_code cnk_scalar_dot(cnk_f64_const_view left,
    cnk_f64_const_view right, double *result);

static inline ptrdiff_t cnk_vector_offset(size_t index, ptrdiff_t stride) {
    return (ptrdiff_t)index * stride;
}

static inline cnk_error_code cnk_kernel_status(cnk_error_code code) {
    cnk_set_last_error(code, NULL);
    return code;
}

#endif
