#include "cnumkit/kernels.h"
#include "cnumkit/adapters.h"
#include "test_framework.h"

#include <float.h>
#include <stdint.h>
#include <string.h>

static void test_matvec_layouts(void) {
    /* Padding must neither participate in arithmetic nor be overwritten. */
    double coefficients[] = {1, 2, 3, NAN, 4, 5, 6, NAN};
    double input[] = {3, 2, 1};
    double output[] = {NAN, 123, NAN};
    cnk_f64_const_matrix_view matrix = cnk_f64_const_matrix_view_make(coefficients, 2, 3, 4, 1);
    EXPECT_TRUE(cnk_f64_matvec(matrix, cnk_f64_const_view_make(input + 2, 3, -1),
        cnk_f64_view_make(output + 2, 2, -2)) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(output[2], 14);
    EXPECT_ALMOST_EQ(output[0], 32);
    EXPECT_ALMOST_EQ(output[1], 123);

    double weights[] = {2, -1};
    double transposed_output[3];
    cnk_f64_const_matrix_view transpose = cnk_f64_const_matrix_view_make(coefficients, 3, 2, 1, 4);
    EXPECT_TRUE(cnk_f64_matvec(transpose, cnk_f64_const_view_make(weights, 2, 1),
        cnk_f64_view_make(transposed_output, 3, 1)) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(transposed_output[0], -2);
    EXPECT_ALMOST_EQ(transposed_output[1], -1);
    EXPECT_ALMOST_EQ(transposed_output[2], 0);
}

static void test_matmul_layouts(void) {
    double left[] = {1, 2, 3, 4, 5, 6};
    /* The right matrix is column-major; the output has reversed padded rows. */
    double right[] = {7, 9, 11, 8, 10, 12};
    double output[] = {NAN, NAN, 123, NAN, NAN};
    EXPECT_TRUE(cnk_f64_matmul(cnk_f64_const_matrix_view_make(left, 2, 3, 3, 1),
        cnk_f64_const_matrix_view_make(right, 3, 2, 1, 3),
        cnk_f64_matrix_view_make(output + 3, 2, 2, -3, 1)) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(output[3], 58);
    EXPECT_ALMOST_EQ(output[4], 64);
    EXPECT_ALMOST_EQ(output[0], 139);
    EXPECT_ALMOST_EQ(output[1], 154);
    EXPECT_ALMOST_EQ(output[2], 123);
}

static void test_matrix_late_failure(void) {
    double left[] = {1, 2, 3, DBL_MAX};
    double right[] = {1, 0, 0, 2};
    double output[] = {11, 12, 13, 14};
    double original[4];
    memcpy(original, output, sizeof(original));
    cnk_f64_const_matrix_view matrix = cnk_f64_const_matrix_view_make(left, 2, 2, 2, 1);
    cnk_f64_const_matrix_view rhs = cnk_f64_const_matrix_view_make(right, 2, 2, 2, 1);
    cnk_f64_matrix_view result = cnk_f64_matrix_view_make(output, 2, 2, 2, 1);
    EXPECT_TRUE(cnk_f64_matmul(matrix, rhs, result) == CNK_ERROR_MATH);
    EXPECT_TRUE(memcmp(output, original, sizeof(original)) == 0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    double input[] = {1, 2};
    EXPECT_TRUE(cnk_f64_matvec(matrix, cnk_f64_const_view_make(input, 2, 1),
        cnk_f64_view_make(output, 2, 1)) == CNK_ERROR_MATH);
    EXPECT_TRUE(memcmp(output, original, sizeof(original)) == 0);
    left[3] = NAN;
    EXPECT_TRUE(cnk_f64_matmul(matrix, rhs, result) == CNK_ERROR_MATH);
    EXPECT_TRUE(memcmp(output, original, sizeof(original)) == 0);
    left[3] = 4;
    EXPECT_TRUE(cnk_f64_matmul(matrix, rhs, result) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_get_last_error() == CNK_SUCCESS);
}

static void test_empty_matrix_products(void) {
    double output[] = {1, 2, 3, 4};
    EXPECT_TRUE(cnk_f64_matmul(cnk_f64_const_matrix_view_make(NULL, 2, 0, 0, 0),
        cnk_f64_const_matrix_view_make(NULL, 0, 2, 0, 0),
        cnk_f64_matrix_view_make(output, 2, 2, 2, 1)) == CNK_SUCCESS);
    for (size_t index = 0; index < 4; ++index) {
        EXPECT_TRUE(output[index] == 0.0);
    }
    output[0] = output[1] = 3;
    EXPECT_TRUE(cnk_f64_matvec(cnk_f64_const_matrix_view_make(NULL, 2, 0, 0, 0),
        cnk_f64_const_view_make(NULL, 0, 0), cnk_f64_view_make(output, 2, 1)) == CNK_SUCCESS);
    EXPECT_TRUE(output[0] == 0.0 && output[1] == 0.0);
    EXPECT_TRUE(cnk_f64_matmul(cnk_f64_const_matrix_view_make(NULL, SIZE_MAX, 0, 0, 0),
        cnk_f64_const_matrix_view_make(NULL, 0, 0, 0, 0),
        cnk_f64_matrix_view_make(NULL, SIZE_MAX, 0, 0, 0)) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_f64_matvec(cnk_f64_const_matrix_view_make(NULL, 0, 0, 0, 0),
        cnk_f64_const_view_make(NULL, 0, 0), cnk_f64_view_make(NULL, 0, 0)) == CNK_SUCCESS);
}

static void test_invalid_matrix_layouts(void) {
    double data[] = {1, 2, 3, 4};
    double output[] = {7, 8, 9, 10};
    cnk_f64_const_matrix_view good = cnk_f64_const_matrix_view_make(data, 2, 2, 2, 1);
    cnk_f64_matrix_view result = cnk_f64_matrix_view_make(output, 2, 2, 2, 1);
    const cnk_f64_const_matrix_view invalid[] = {
        {NULL, 2, 2, 2, 1}, {data, 2, 2, 0, 1}, {data, 2, 2, 1, 1},
        {data, 2, 2, PTRDIFF_MIN, 1}, {data, SIZE_MAX, 2, 2, 1},
        {data, 2, 2, PTRDIFF_MAX / (ptrdiff_t)sizeof(double), 1}
    };
    for (size_t index = 0; index < sizeof(invalid) / sizeof(invalid[0]); ++index) {
        EXPECT_TRUE(cnk_f64_matmul(invalid[index], good, result) == CNK_ERROR_INVALID_ARGUMENT);
        EXPECT_TRUE(output[0] == 7 && output[3] == 10);
    }
    cnk_f64_matrix_view collision = cnk_f64_matrix_view_make(output, 2, 2, 1, -1);
    EXPECT_TRUE(cnk_f64_matmul(good, good, collision) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(cnk_f64_matmul(good, good,
        cnk_f64_matrix_view_make(output, 1, 2, 2, 1)) == CNK_ERROR_DIMENSION_MISMATCH);
    EXPECT_TRUE(cnk_f64_matvec(good, cnk_f64_const_view_make(data, 1, 1),
        cnk_f64_view_make(output, 2, 1)) == CNK_ERROR_DIMENSION_MISMATCH);
}

static int layout_has_duplicate_elements(size_t rows, size_t columns,
    ptrdiff_t row_stride, ptrdiff_t column_stride) {
    /* Enumerate addresses independently of the production GCD-based validator. */
    ptrdiff_t offsets[9];
    size_t count = 0;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            ptrdiff_t offset = (ptrdiff_t)row * row_stride + (ptrdiff_t)column * column_stride;
            for (size_t previous = 0; previous < count; ++previous) {
                if (offsets[previous] == offset) {
                    return 1;
                }
            }
            offsets[count++] = offset;
        }
    }
    return 0;
}

static void test_layout_validation_against_enumerated_offsets(void) {
    double storage[49];
    for (size_t index = 0; index < 49; ++index) {
        storage[index] = 1.0;
    }
    double ones[] = {1, 1, 1};
    double output[3];
    for (size_t rows = 1; rows <= 3; ++rows) {
        for (size_t cols = 1; cols <= 3; ++cols) {
            for (ptrdiff_t row_stride = -6; row_stride <= 6; ++row_stride) {
                for (ptrdiff_t column_stride = -6; column_stride <= 6; ++column_stride) {
                    cnk_f64_const_matrix_view matrix = cnk_f64_const_matrix_view_make(
                        storage + 24, rows, cols, row_stride, column_stride);
                    cnk_error_code status = cnk_f64_matvec(matrix,
                        cnk_f64_const_view_make(ones, cols, 1), cnk_f64_view_make(output, rows, 1));
                    int duplicate = layout_has_duplicate_elements(rows, cols, row_stride, column_stride);
                    EXPECT_TRUE(status == (duplicate ? CNK_ERROR_INVALID_ARGUMENT : CNK_SUCCESS));
                    if (!duplicate) {
                        EXPECT_TRUE(output[0] == (double)cols && output[rows - 1] == (double)cols);
                    }
                }
            }
        }
    }
}

static void test_owned_adapters(void) {
    cnk_vector *vector = cnk_vector_create(2);
    cnk_matrix *matrix = cnk_matrix_identity(2);
    EXPECT_NOT_NULL(vector);
    EXPECT_NOT_NULL(matrix);
    if (!vector || !matrix) {
        cnk_vector_free(vector);
        cnk_matrix_free(matrix);
        return;
    }
    cnk_f64_view vector_view;
    cnk_f64_const_view const_vector;
    cnk_f64_matrix_view matrix_view;
    cnk_f64_const_matrix_view const_matrix;
    EXPECT_TRUE(cnk_vector_as_view(vector, &vector_view) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_vector_as_const_view(vector, &const_vector) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_matrix_as_view(matrix, &matrix_view) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_matrix_as_const_view(matrix, &const_matrix) == CNK_SUCCESS);
    EXPECT_TRUE(vector_view.data == vector->data && const_vector.data == vector->data);
    EXPECT_TRUE(matrix_view.data == matrix->data && const_matrix.data == matrix->data);
    EXPECT_TRUE(cnk_f64_fill(vector_view, 3.0) == CNK_SUCCESS);
    double output[2];
    EXPECT_TRUE(cnk_f64_matvec(const_matrix, const_vector, cnk_f64_view_make(output, 2, 1))
        == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(output[0], 3.0);
    EXPECT_ALMOST_EQ(output[1], 3.0);
    EXPECT_TRUE(cnk_vector_as_view(NULL, &vector_view) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(vector_view.data == vector->data && vector_view.size == 2);
    EXPECT_TRUE(cnk_matrix_as_view(NULL, &matrix_view) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(matrix_view.data == matrix->data && matrix_view.rows == 2);
    EXPECT_TRUE(cnk_vector_as_const_view(vector, NULL) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(cnk_matrix_as_const_view(matrix, NULL) == CNK_ERROR_INVALID_ARGUMENT);
    cnk_matrix malformed = {2, SIZE_MAX, matrix->data};
    EXPECT_TRUE(cnk_matrix_as_const_view(&malformed, &const_matrix) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(const_matrix.cols == 2);
    cnk_vector_free(vector);
    cnk_matrix_free(matrix);
}

TEST_SUITE("Matrix Kernels And Owned Adapters")
    RUN_TEST(test_matvec_layouts);
    RUN_TEST(test_matmul_layouts);
    RUN_TEST(test_matrix_late_failure);
    RUN_TEST(test_empty_matrix_products);
    RUN_TEST(test_invalid_matrix_layouts);
    RUN_TEST(test_layout_validation_against_enumerated_offsets);
    RUN_TEST(test_owned_adapters);
TEST_SUITE_END()
