#include "kernel_internal.h"

#include <stdint.h>

static size_t stride_magnitude(ptrdiff_t stride) {
    /* PTRDIFF_MIN cannot be negated in its signed type. */
    return stride < 0 ? (size_t)(-(stride + 1)) + 1 : (size_t)stride;
}

static int axis_span(size_t count, ptrdiff_t stride, size_t *span) {
    *span = 0;
    if (count <= 1) {
        return 1;
    }
    size_t magnitude = stride_magnitude(stride);
    /* Bound byte offsets as well as element offsets before doing arithmetic. */
    size_t limit = (size_t)PTRDIFF_MAX / sizeof(double);
    if (magnitude == 0 || count - 1 > limit / magnitude) {
        return 0;
    }
    *span = (count - 1) * magnitude;
    return 1;
}

int cnk_vector_layout_valid(const void *data, size_t size, ptrdiff_t stride) {
    size_t span;
    return size == 0 || (data != NULL && axis_span(size, stride, &span));
}

static size_t greatest_common_divisor(size_t left, size_t right) {
    while (right != 0) {
        size_t remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

static int matrix_elements_are_distinct(cnk_f64_const_matrix_view matrix) {
    if (matrix.rows <= 1 || matrix.cols <= 1) {
        return 1;
    }
    size_t row_step = stride_magnitude(matrix.row_stride);
    size_t column_step = stride_magnitude(matrix.column_stride);
    size_t divisor = greatest_common_divisor(row_step, column_step);
    /* The smallest colliding index differences are column_step/g and row_step/g. */
    return matrix.rows <= column_step / divisor
        || matrix.cols <= row_step / divisor;
}

int cnk_matrix_layout_valid(cnk_f64_const_matrix_view matrix) {
    if (matrix.rows == 0 || matrix.cols == 0) {
        return 1;
    }
    size_t row_span, column_span;
    size_t limit = (size_t)PTRDIFF_MAX / sizeof(double);
    if (!matrix.data
        || !axis_span(matrix.rows, matrix.row_stride, &row_span)
        || !axis_span(matrix.cols, matrix.column_stride, &column_span)
        || row_span > limit - column_span) {
        return 0;
    }
    return matrix_elements_are_distinct(matrix);
}
