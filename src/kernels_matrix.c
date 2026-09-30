#include "kernel_internal.h"

static cnk_f64_const_view matrix_row(cnk_f64_const_matrix_view matrix, size_t row) {
    /* Empty rows need no pointer arithmetic, including arithmetic on NULL. */
    const double *data = matrix.cols == 0 ? NULL
        : matrix.data + cnk_vector_offset(row, matrix.row_stride);
    return cnk_f64_const_view_make(data, matrix.cols, matrix.column_stride);
}

static cnk_f64_const_view matrix_column(cnk_f64_const_matrix_view matrix, size_t column) {
    const double *data = matrix.rows == 0 ? NULL
        : matrix.data + cnk_vector_offset(column, matrix.column_stride);
    return cnk_f64_const_view_make(data, matrix.rows, matrix.row_stride);
}

static cnk_error_code matvec_pass(cnk_f64_const_matrix_view matrix,
    cnk_f64_const_view input, cnk_f64_view output, int write_output) {
    for (size_t row = 0; row < matrix.rows; ++row) {
        double value;
        cnk_error_code status = cnk_scalar_dot(matrix_row(matrix, row), input, &value);
        if (status != CNK_SUCCESS) {
            return status;
        }
        if (write_output) {
            output.data[cnk_vector_offset(row, output.stride)] = value;
        }
    }
    return CNK_SUCCESS;
}

cnk_error_code cnk_f64_matvec(cnk_f64_const_matrix_view matrix,
    cnk_f64_const_view input, cnk_f64_view output) {
    if (!cnk_matrix_layout_valid(matrix)
        || !cnk_vector_layout_valid(input.data, input.size, input.stride)
        || !cnk_vector_layout_valid(output.data, output.size, output.stride)) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    if (matrix.cols != input.size || matrix.rows != output.size) {
        return cnk_kernel_status(CNK_ERROR_DIMENSION_MISMATCH);
    }
    /* Two passes provide transactional output with no hidden workspace allocation. */
    cnk_error_code status = matvec_pass(matrix, input, output, 0);
    if (status == CNK_SUCCESS) {
        status = matvec_pass(matrix, input, output, 1);
    }
    return cnk_kernel_status(status);
}

static cnk_error_code matmul_pass(cnk_f64_const_matrix_view left,
    cnk_f64_const_matrix_view right, cnk_f64_matrix_view output, int write_output) {
    /* Return before iterating potentially huge unused dimensions of empty outputs. */
    if (output.rows == 0 || output.cols == 0) {
        return CNK_SUCCESS;
    }
    for (size_t row = 0; row < output.rows; ++row) {
        for (size_t column = 0; column < output.cols; ++column) {
            double value;
            cnk_error_code status = cnk_scalar_dot(matrix_row(left, row),
                matrix_column(right, column), &value);
            if (status != CNK_SUCCESS) {
                return status;
            }
            if (write_output) {
                ptrdiff_t offset = cnk_vector_offset(row, output.row_stride)
                    + cnk_vector_offset(column, output.column_stride);
                output.data[offset] = value;
            }
        }
    }
    return CNK_SUCCESS;
}

cnk_error_code cnk_f64_matmul(cnk_f64_const_matrix_view left,
    cnk_f64_const_matrix_view right, cnk_f64_matrix_view output) {
    cnk_f64_const_matrix_view output_layout = cnk_f64_const_matrix_view_make(
        output.data, output.rows, output.cols, output.row_stride, output.column_stride);
    if (!cnk_matrix_layout_valid(left) || !cnk_matrix_layout_valid(right)
        || !cnk_matrix_layout_valid(output_layout)) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    if (left.cols != right.rows || output.rows != left.rows || output.cols != right.cols) {
        return cnk_kernel_status(CNK_ERROR_DIMENSION_MISMATCH);
    }
    cnk_error_code status = matmul_pass(left, right, output, 0);
    if (status == CNK_SUCCESS) {
        status = matmul_pass(left, right, output, 1);
    }
    return cnk_kernel_status(status);
}
