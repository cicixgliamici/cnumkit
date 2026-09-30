#include "cnumkit/view.h"

cnk_f64_view cnk_f64_view_make(double *data, size_t size, ptrdiff_t stride) {
    cnk_f64_view view = {data, size, stride};
    return view;
}

cnk_f64_const_view cnk_f64_const_view_make(
    const double *data,
    size_t size,
    ptrdiff_t stride) {
    cnk_f64_const_view view = {data, size, stride};
    return view;
}

cnk_f64_matrix_view cnk_f64_matrix_view_make(
    double *data,
    size_t rows,
    size_t cols,
    ptrdiff_t row_stride,
    ptrdiff_t column_stride) {
    cnk_f64_matrix_view view = {
        data, rows, cols, row_stride, column_stride
    };
    return view;
}

cnk_f64_const_matrix_view cnk_f64_const_matrix_view_make(
    const double *data,
    size_t rows,
    size_t cols,
    ptrdiff_t row_stride,
    ptrdiff_t column_stride) {
    cnk_f64_const_matrix_view view = {
        data, rows, cols, row_stride, column_stride
    };
    return view;
}
