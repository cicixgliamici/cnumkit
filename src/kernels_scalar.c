#include "kernel_internal.h"

#include <float.h>
#include <math.h>

cnk_error_code cnk_f64_fill(cnk_f64_view destination, double value) {
    if (!cnk_vector_layout_valid(destination.data, destination.size, destination.stride)) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    if (!isfinite(value)) {
        return cnk_kernel_status(CNK_ERROR_MATH);
    }
    /* Index only actual elements; a final stride increment could leave the object. */
    for (size_t index = 0; index < destination.size; ++index) {
        destination.data[cnk_vector_offset(index, destination.stride)] = value;
    }
    return cnk_kernel_status(CNK_SUCCESS);
}

cnk_error_code cnk_f64_scale(cnk_f64_view values, double factor) {
    if (!cnk_vector_layout_valid(values.data, values.size, values.stride)) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    if (!isfinite(factor)) {
        return cnk_kernel_status(CNK_ERROR_MATH);
    }
    /* Preflight every result before writing to preserve the failure guarantee. */
    for (size_t index = 0; index < values.size; ++index) {
        double value = values.data[cnk_vector_offset(index, values.stride)];
        if (!isfinite(value) || !isfinite(value * factor)) {
            return cnk_kernel_status(CNK_ERROR_MATH);
        }
    }
    for (size_t index = 0; index < values.size; ++index) {
        values.data[cnk_vector_offset(index, values.stride)] *= factor;
    }
    return cnk_kernel_status(CNK_SUCCESS);
}

static cnk_error_code validate_pair(cnk_f64_const_view left, cnk_f64_const_view right) {
    if (!cnk_vector_layout_valid(left.data, left.size, left.stride)
        || !cnk_vector_layout_valid(right.data, right.size, right.stride)) {
        return CNK_ERROR_INVALID_ARGUMENT;
    }
    return left.size == right.size ? CNK_SUCCESS : CNK_ERROR_DIMENSION_MISMATCH;
}

cnk_error_code cnk_f64_copy(cnk_f64_const_view source, cnk_f64_view destination) {
    cnk_error_code status = validate_pair(source,
        cnk_f64_const_view_make(destination.data, destination.size, destination.stride));
    if (status != CNK_SUCCESS) {
        return cnk_kernel_status(status);
    }
    for (size_t index = 0; index < source.size; ++index) {
        if (!isfinite(source.data[cnk_vector_offset(index, source.stride)])) {
            return cnk_kernel_status(CNK_ERROR_MATH);
        }
    }
    for (size_t index = 0; index < source.size; ++index) {
        destination.data[cnk_vector_offset(index, destination.stride)] =
            source.data[cnk_vector_offset(index, source.stride)];
    }
    return cnk_kernel_status(CNK_SUCCESS);
}

cnk_error_code cnk_f64_axpy(double alpha, cnk_f64_const_view x, cnk_f64_view y) {
    cnk_error_code status = validate_pair(x,
        cnk_f64_const_view_make(y.data, y.size, y.stride));
    if (status != CNK_SUCCESS) {
        return cnk_kernel_status(status);
    }
    if (!isfinite(alpha)) {
        return cnk_kernel_status(CNK_ERROR_MATH);
    }
    for (size_t index = 0; index < x.size; ++index) {
        double x_value = x.data[cnk_vector_offset(index, x.stride)];
        double y_value = y.data[cnk_vector_offset(index, y.stride)];
        if (!isfinite(x_value) || !isfinite(y_value)
            || !isfinite(alpha * x_value + y_value)) {
            return cnk_kernel_status(CNK_ERROR_MATH);
        }
    }
    for (size_t index = 0; index < x.size; ++index) {
        y.data[cnk_vector_offset(index, y.stride)] +=
            alpha * x.data[cnk_vector_offset(index, x.stride)];
    }
    return cnk_kernel_status(CNK_SUCCESS);
}

cnk_error_code cnk_scalar_dot(cnk_f64_const_view left,
    cnk_f64_const_view right, double *result) {
    /* Wider accumulation helps only where long double exceeds double.
       This is not an exact summation or an extended-precision guarantee. */
    long double sum = 0.0L;
    for (size_t index = 0; index < left.size; ++index) {
        double left_value = left.data[cnk_vector_offset(index, left.stride)];
        double right_value = right.data[cnk_vector_offset(index, right.stride)];
        if (!isfinite(left_value) || !isfinite(right_value)) {
            return CNK_ERROR_MATH;
        }
        sum += (long double)left_value * (long double)right_value;
    }
    if (!isfinite(sum) || fabsl(sum) > DBL_MAX) {
        return CNK_ERROR_MATH;
    }
    *result = (double)sum;
    return CNK_SUCCESS;
}

cnk_error_code cnk_f64_dot(cnk_f64_const_view left,
    cnk_f64_const_view right, double *result) {
    cnk_error_code status = validate_pair(left, right);
    if (!result) {
        status = CNK_ERROR_INVALID_ARGUMENT;
    }
    if (status == CNK_SUCCESS) {
        status = cnk_scalar_dot(left, right, result);
    }
    return cnk_kernel_status(status);
}

cnk_error_code cnk_f64_norm2(cnk_f64_const_view values, double *result) {
    if (!result || !cnk_vector_layout_valid(values.data, values.size, values.stride)) {
        return cnk_kernel_status(CNK_ERROR_INVALID_ARGUMENT);
    }
    double scale = 0.0;
    double sum_squares = 1.0;
    for (size_t index = 0; index < values.size; ++index) {
        double magnitude = fabs(values.data[cnk_vector_offset(index, values.stride)]);
        if (!isfinite(magnitude)) {
            return cnk_kernel_status(CNK_ERROR_MATH);
        }
        /* Square ratios rather than raw values to avoid unnecessary overflow/underflow. */
        if (magnitude > scale) {
            double ratio = scale / magnitude;
            sum_squares = 1.0 + sum_squares * ratio * ratio;
            scale = magnitude;
        } else if (magnitude != 0.0) {
            double ratio = magnitude / scale;
            sum_squares += ratio * ratio;
        }
    }
    double norm = scale * sqrt(sum_squares);
    if (!isfinite(norm)) {
        return cnk_kernel_status(CNK_ERROR_MATH);
    }
    *result = norm;
    return cnk_kernel_status(CNK_SUCCESS);
}
