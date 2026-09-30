#include "test_framework.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "cnumkit.h"

static void test_contiguous_kernels(void) {
    double x_data[] = {1.0, 2.0, 3.0};
    double y_data[] = {4.0, 5.0, 6.0};
    cnk_f64_const_view x = cnk_f64_const_view_make(x_data, 3, 1);
    cnk_f64_view y = cnk_f64_view_make(y_data, 3, 1);
    double result = -1.0;

    EXPECT_TRUE(cnk_f64_dot(x, cnk_f64_const_view_make(y_data, 3, 1), &result) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(result, 32.0);
    EXPECT_TRUE(cnk_f64_axpy(2.0, x, y) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(y_data[0], 6.0);
    EXPECT_ALMOST_EQ(y_data[1], 9.0);
    EXPECT_ALMOST_EQ(y_data[2], 12.0);
    EXPECT_TRUE(cnk_f64_scale(y, 0.5) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(y_data[2], 6.0);
}

static void test_strided_and_reversed_views(void) {
    double data[] = {1.0, 10.0, 2.0, 20.0, 3.0};
    double reversed[] = {3.0, 2.0, 1.0};
    cnk_f64_view strided = cnk_f64_view_make(data, 3, 2);
    cnk_f64_const_view reverse = cnk_f64_const_view_make(&reversed[2], 3, -1);
    double result = 0.0;

    EXPECT_TRUE(cnk_f64_dot(cnk_f64_const_view_make(data, 3, 2), reverse, &result) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(result, 14.0);
    EXPECT_TRUE(cnk_f64_fill(strided, 7.0) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(data[0], 7.0);
    EXPECT_ALMOST_EQ(data[1], 10.0);
    EXPECT_ALMOST_EQ(data[2], 7.0);
    EXPECT_ALMOST_EQ(data[3], 20.0);
    EXPECT_ALMOST_EQ(data[4], 7.0);
}

static void test_empty_views(void) {
    cnk_f64_const_view empty = cnk_f64_const_view_make(NULL, 0, 0);
    double result = -1.0;

    EXPECT_TRUE(cnk_f64_dot(empty, empty, &result) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(result, 0.0);
    EXPECT_TRUE(cnk_f64_fill(cnk_f64_view_make(NULL, 0, 0), 2.0) == CNK_SUCCESS);
}

static void test_validation_and_unchanged_outputs(void) {
    double values[] = {1.0, DBL_MAX};
    double other[] = {2.0};
    double result = 123.0;

    EXPECT_TRUE(cnk_f64_dot(
        cnk_f64_const_view_make(values, 2, 1),
        cnk_f64_const_view_make(other, 1, 1),
        &result) == CNK_ERROR_DIMENSION_MISMATCH);
    EXPECT_ALMOST_EQ(result, 123.0);
    EXPECT_TRUE(cnk_f64_scale(cnk_f64_view_make(values, 2, 1), 2.0) == CNK_ERROR_MATH);
    EXPECT_ALMOST_EQ(values[0], 1.0);
    EXPECT_ALMOST_EQ(values[1], DBL_MAX);
    EXPECT_TRUE(cnk_f64_fill(cnk_f64_view_make(NULL, 2, 1), 1.0) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(cnk_f64_fill(cnk_f64_view_make(values, 2, 0), 1.0) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(cnk_f64_fill(cnk_f64_view_make(values, 2, 1), NAN) == CNK_ERROR_MATH);
}

static void test_exact_in_place_axpy(void) {
    double values[] = {1.0, -2.0, 4.0};
    cnk_f64_const_view input = cnk_f64_const_view_make(values, 3, 1);
    cnk_f64_view output = cnk_f64_view_make(values, 3, 1);

    EXPECT_TRUE(cnk_f64_axpy(2.0, input, output) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(values[0], 3.0);
    EXPECT_ALMOST_EQ(values[1], -6.0);
    EXPECT_ALMOST_EQ(values[2], 12.0);
}

static void test_copy_and_late_failure(void) {
    double source[] = {1.0, 2.0, 3.0};
    double destination[] = {9.0, 8.0, 7.0, 6.0, 5.0};
    cnk_f64_const_view reverse = cnk_f64_const_view_make(source + 2, 3, -1);
    cnk_f64_view output = cnk_f64_view_make(destination, 3, 2);
    EXPECT_TRUE(cnk_f64_copy(reverse, output) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(destination[0], 3.0);
    EXPECT_ALMOST_EQ(destination[1], 8.0);
    EXPECT_ALMOST_EQ(destination[2], 2.0);
    EXPECT_ALMOST_EQ(destination[3], 6.0);
    EXPECT_ALMOST_EQ(destination[4], 1.0);
    EXPECT_TRUE(cnk_f64_copy(cnk_f64_const_view_make(destination, 3, 2), output) == CNK_SUCCESS);

    double original[5];
    memcpy(original, destination, sizeof(original));
    source[0] = NAN; /* The invalid value is last in logical traversal order. */
    EXPECT_TRUE(cnk_f64_copy(reverse, output) == CNK_ERROR_MATH);
    EXPECT_TRUE(memcmp(original, destination, sizeof(original)) == 0);
    EXPECT_TRUE(cnk_f64_axpy(2.0, reverse, output) == CNK_ERROR_MATH);
    EXPECT_TRUE(memcmp(original, destination, sizeof(original)) == 0);
    EXPECT_TRUE(cnk_f64_copy(cnk_f64_const_view_make(source, 1, 1), output)
        == CNK_ERROR_DIMENSION_MISMATCH);
    EXPECT_TRUE(cnk_f64_copy(cnk_f64_const_view_make(NULL, 0, 0),
        cnk_f64_view_make(NULL, 0, 0)) == CNK_SUCCESS);
}

static void test_norm_extreme_scales(void) {
    const double scales[] = {1e-300, 1.0, 1e300};
    for (size_t index = 0; index < 3; ++index) {
        double values[] = {3.0 * scales[index], 99.0, -4.0 * scales[index]};
        double norm = -1.0;
        EXPECT_TRUE(cnk_f64_norm2(cnk_f64_const_view_make(values + 2, 2, -2), &norm)
            == CNK_SUCCESS);
        /* Normalize the assertion so an absolute epsilon cannot hide underflow. */
        EXPECT_TRUE(fabs(norm / (5.0 * scales[index]) - 1.0) < 1e-14);
        EXPECT_TRUE(fabs(norm / hypot(values[0], values[2]) - 1.0) < 1e-14);
    }
    double tiny = nextafter(0.0, 1.0);
    double norm = -1.0;
    EXPECT_TRUE(cnk_f64_norm2(cnk_f64_const_view_make(&tiny, 1, PTRDIFF_MIN), &norm)
        == CNK_SUCCESS);
    EXPECT_TRUE(norm == tiny);
    double zeros[] = {0.0, -0.0};
    EXPECT_TRUE(cnk_f64_norm2(cnk_f64_const_view_make(zeros, 2, 1), &norm) == CNK_SUCCESS);
    EXPECT_TRUE(norm == 0.0);
    EXPECT_TRUE(cnk_f64_norm2(cnk_f64_const_view_make(NULL, 0, 0), &norm) == CNK_SUCCESS);
    EXPECT_TRUE(norm == 0.0);
}

static void test_reduction_failures(void) {
    double values[] = {DBL_MAX, DBL_MAX};
    double result = 7.0;
    cnk_f64_const_view input = cnk_f64_const_view_make(values, 2, 1);
    EXPECT_TRUE(cnk_f64_norm2(input, &result) == CNK_ERROR_MATH);
    EXPECT_TRUE(result == 7.0);
    EXPECT_TRUE(cnk_f64_dot(input, input, &result) == CNK_ERROR_MATH);
    EXPECT_TRUE(result == 7.0);
    values[1] = INFINITY;
    EXPECT_TRUE(cnk_f64_norm2(input, &result) == CNK_ERROR_MATH);
    EXPECT_TRUE(result == 7.0);
    EXPECT_TRUE(cnk_f64_dot(input, input, &result) == CNK_ERROR_MATH);
    EXPECT_TRUE(result == 7.0);
    EXPECT_TRUE(cnk_f64_norm2(input, NULL) == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(cnk_f64_dot(input, input, NULL) == CNK_ERROR_INVALID_ARGUMENT);
}

static void test_offset_validation(void) {
    double value = 6.0;
    const ptrdiff_t strides[] = {PTRDIFF_MIN, PTRDIFF_MAX, 0};
    for (size_t index = 0; index < 3; ++index) {
        EXPECT_TRUE(cnk_f64_fill(cnk_f64_view_make(&value, 2, strides[index]), 1.0)
            == CNK_ERROR_INVALID_ARGUMENT);
        EXPECT_TRUE(value == 6.0);
    }
    EXPECT_TRUE(cnk_f64_scale(cnk_f64_view_make(&value, SIZE_MAX, 1), 1.0)
        == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(value == 6.0);
    EXPECT_TRUE(cnk_f64_fill(cnk_f64_view_make(&value, 1, PTRDIFF_MIN), 2.0) == CNK_SUCCESS);
    EXPECT_TRUE(value == 2.0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_SUCCESS);
}

static void test_legacy_vector_reductions(void) {
    double values[] = {3e200, 4e200};
    cnk_vector owner = {2, values};
    double result = 0.0;
    EXPECT_TRUE(cnk_vector_norm2(&owner, &result) == 0);
    EXPECT_TRUE(fabs(result / 5e200 - 1.0) < 1e-14);
    result = 42.0;
    EXPECT_TRUE(cnk_vector_dot(&owner, &owner, &result) == -1);
    EXPECT_TRUE(result == 42.0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
}

TEST_SUITE("Borrowed Views And Scalar Kernels")
    RUN_TEST(test_contiguous_kernels);
    RUN_TEST(test_strided_and_reversed_views);
    RUN_TEST(test_empty_views);
    RUN_TEST(test_validation_and_unchanged_outputs);
    RUN_TEST(test_exact_in_place_axpy);
    RUN_TEST(test_copy_and_late_failure);
    RUN_TEST(test_norm_extreme_scales);
    RUN_TEST(test_reduction_failures);
    RUN_TEST(test_offset_validation);
    RUN_TEST(test_legacy_vector_reductions);
TEST_SUITE_END()
