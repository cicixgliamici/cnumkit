#include "cnumkit.h"
#include "test_framework.h"

#include <float.h>
#include <stdint.h>
#include <string.h>

static double relative_residual(
    const cnk_matrix *matrix,
    const cnk_vector *solution,
    const cnk_vector *rhs
) {
    double residual_norm = 0.0;
    double matrix_norm = 0.0;
    double solution_norm = 0.0;
    double rhs_norm = 0.0;

    for (size_t row = 0; row < matrix->rows; row++) {
        double product = 0.0;
        for (size_t col = 0; col < matrix->cols; col++) {
            double coefficient = cnk_matrix_get(matrix, row, col);
            product += coefficient * cnk_vector_get(solution, col);
            matrix_norm = hypot(matrix_norm, coefficient);
        }

        double rhs_value = cnk_vector_get(rhs, row);
        double difference = product - rhs_value;
        residual_norm = hypot(residual_norm, difference);
        rhs_norm = hypot(rhs_norm, rhs_value);
    }

    for (size_t i = 0; i < solution->size; i++) {
        double value = cnk_vector_get(solution, i);
        solution_norm = hypot(solution_norm, value);
    }

    double denominator = matrix_norm * solution_norm + rhs_norm;
    return denominator == 0.0 ? residual_norm : residual_norm / denominator;
}

static void test_vector_dot(void) {
    cnk_vector *a = cnk_vector_create(3);
    cnk_vector *b = cnk_vector_create(3);

    EXPECT_NOT_NULL(a);
    EXPECT_NOT_NULL(b);

    cnk_vector_set(a, 0, 1.0);
    cnk_vector_set(a, 1, 2.0);
    cnk_vector_set(a, 2, 3.0);

    cnk_vector_set(b, 0, 4.0);
    cnk_vector_set(b, 1, 5.0);
    cnk_vector_set(b, 2, 6.0);

    double result = 0.0;
    EXPECT_TRUE(cnk_vector_dot(a, b, &result) == 0);
    EXPECT_ALMOST_EQ(result, 32.0);

    cnk_vector_free(a);
    cnk_vector_free(b);
}

static void test_vector_norm2(void) {
    cnk_vector *v = cnk_vector_create(2);

    EXPECT_NOT_NULL(v);

    cnk_vector_set(v, 0, 3.0);
    cnk_vector_set(v, 1, 4.0);

    double result = 0.0;
    EXPECT_TRUE(cnk_vector_norm2(v, &result) == 0);
    EXPECT_ALMOST_EQ(result, 5.0);

    cnk_vector_free(v);
}

static void test_matrix_multiply(void) {
    cnk_matrix *A = cnk_matrix_create(2, 2);
    cnk_matrix *B = cnk_matrix_create(2, 2);

    EXPECT_NOT_NULL(A);
    EXPECT_NOT_NULL(B);

    cnk_matrix_set(A, 0, 0, 1.0);
    cnk_matrix_set(A, 0, 1, 2.0);
    cnk_matrix_set(A, 1, 0, 3.0);
    cnk_matrix_set(A, 1, 1, 4.0);

    cnk_matrix_set(B, 0, 0, 5.0);
    cnk_matrix_set(B, 0, 1, 6.0);
    cnk_matrix_set(B, 1, 0, 7.0);
    cnk_matrix_set(B, 1, 1, 8.0);

    cnk_matrix *C = cnk_matrix_multiply(A, B);

    EXPECT_NOT_NULL(C);
    EXPECT_ALMOST_EQ(cnk_matrix_get(C, 0, 0), 19.0);
    EXPECT_ALMOST_EQ(cnk_matrix_get(C, 0, 1), 22.0);
    EXPECT_ALMOST_EQ(cnk_matrix_get(C, 1, 0), 43.0);
    EXPECT_ALMOST_EQ(cnk_matrix_get(C, 1, 1), 50.0);

    cnk_matrix_free(A);
    cnk_matrix_free(B);
    cnk_matrix_free(C);
}

static void test_gaussian_solve(void) {
    cnk_matrix *A = cnk_matrix_create(3, 3);
    cnk_vector *b = cnk_vector_create(3);

    EXPECT_NOT_NULL(A);
    EXPECT_NOT_NULL(b);

    cnk_matrix_set(A, 0, 0, 3.0);
    cnk_matrix_set(A, 0, 1, 2.0);
    cnk_matrix_set(A, 0, 2, -1.0);
    cnk_matrix_set(A, 1, 0, 2.0);
    cnk_matrix_set(A, 1, 1, -2.0);
    cnk_matrix_set(A, 1, 2, 4.0);
    cnk_matrix_set(A, 2, 0, -1.0);
    cnk_matrix_set(A, 2, 1, 0.5);
    cnk_matrix_set(A, 2, 2, -1.0);

    cnk_vector_set(b, 0, 1.0);
    cnk_vector_set(b, 1, -2.0);
    cnk_vector_set(b, 2, 0.0);

    cnk_vector *x = cnk_linalg_solve_gaussian(A, b);

    EXPECT_NOT_NULL(x);
    EXPECT_ALMOST_EQ(cnk_vector_get(x, 0), 1.0);
    EXPECT_ALMOST_EQ(cnk_vector_get(x, 1), -2.0);
    EXPECT_ALMOST_EQ(cnk_vector_get(x, 2), -2.0);
    EXPECT_TRUE(relative_residual(A, x, b) < 1e-12);

    cnk_matrix_free(A);
    cnk_vector_free(b);
    cnk_vector_free(x);
}

static void test_error_handling(void) {
    /* 
     * Intentional dimension mismatch to test error handling 
     */
    cnk_matrix *A = cnk_matrix_create(2, 3);
    cnk_matrix *B = cnk_matrix_create(4, 2);
    
    EXPECT_NOT_NULL(A);
    EXPECT_NOT_NULL(B);

    /* Should fail and return NULL */
    cnk_matrix *C = cnk_matrix_multiply(A, B);
    EXPECT_NULL(C);
    
    /* Verify the exact error code */
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_DIMENSION_MISMATCH);
    
    cnk_matrix_free(A);
    cnk_matrix_free(B);
}

static void test_invalid_arguments(void) {
    EXPECT_NULL(cnk_vector_create(0));
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_NULL(cnk_matrix_create(0, 3));
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_NULL(cnk_matrix_create(SIZE_MAX, 2));
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_TRUE(cnk_vector_dot(NULL, NULL, NULL) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_TRUE(cnk_matrix_set(NULL, 0, 0, 1.0) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    /* Non-square matrix for gaussian solve */
    cnk_matrix *rect = cnk_matrix_create(2, 3);
    cnk_vector *vec = cnk_vector_create(2);
    EXPECT_NULL(cnk_linalg_solve_gaussian(rect, vec));
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_DIMENSION_MISMATCH);
    cnk_matrix_free(rect);
    cnk_vector_free(vec);
}

static void test_singular_matrix_error(void) {
    cnk_matrix *A = cnk_matrix_create(2, 2);
    cnk_vector *b = cnk_vector_create(2);

    EXPECT_NOT_NULL(A);
    EXPECT_NOT_NULL(b);

    cnk_matrix_set(A, 0, 0, 1.0);
    cnk_matrix_set(A, 0, 1, 2.0);
    cnk_matrix_set(A, 1, 0, 2.0);
    cnk_matrix_set(A, 1, 1, 4.0);
    cnk_vector_set(b, 0, 1.0);
    cnk_vector_set(b, 1, 2.0);

    cnk_vector *x = cnk_linalg_solve_gaussian(A, b);

    EXPECT_NULL(x);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_SINGULAR_MATRIX);

    cnk_matrix_free(A);
    cnk_vector_free(b);
}

static void test_nan_inf_propagation(void) {
    cnk_vector *v = cnk_vector_create(1);
    cnk_matrix *m = cnk_matrix_create(1, 1);
    
    EXPECT_NOT_NULL(v);
    EXPECT_NOT_NULL(m);
    
    /* Test NAN */
    EXPECT_TRUE(cnk_vector_set(v, 0, NAN) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    
    EXPECT_TRUE(cnk_matrix_set(m, 0, 0, NAN) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    
    /* Test INFINITY */
    EXPECT_TRUE(cnk_vector_set(v, 0, INFINITY) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    
    EXPECT_TRUE(cnk_matrix_set(m, 0, 0, INFINITY) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    
    cnk_vector_free(v);
    cnk_matrix_free(m);
}

static void test_near_singular_matrix(void) {
    cnk_matrix *A = cnk_matrix_create(2, 2);
    cnk_vector *b = cnk_vector_create(2);

    EXPECT_NOT_NULL(A);
    EXPECT_NOT_NULL(b);

    /* The second row is nearly dependent, so the relative pivot check must reject it. */
    cnk_matrix_set(A, 0, 0, 1.0);
    cnk_matrix_set(A, 0, 1, 1.0);
    cnk_matrix_set(A, 1, 0, 1.0);
    cnk_matrix_set(A, 1, 1, 1.0 + 1e-15);
    
    cnk_vector_set(b, 0, 1.0);
    cnk_vector_set(b, 1, 1.0);

    cnk_vector *x = cnk_linalg_solve_gaussian(A, b);

    EXPECT_NULL(x);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_SINGULAR_MATRIX);

    cnk_matrix_free(A);
    cnk_vector_free(b);
}

static void test_error_state_and_output_policy(void) {
    double output = 42.0;

    cnk_set_last_error(CNK_ERROR_UNKNOWN, "test error");
    cnk_vector *v = cnk_vector_create(1);
    EXPECT_NOT_NULL(v);
    EXPECT_TRUE(cnk_get_last_error() == CNK_SUCCESS);

    EXPECT_TRUE(cnk_vector_dot(NULL, v, &output) == -1);
    EXPECT_ALMOST_EQ(output, 42.0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_TRUE(cnk_vector_set(v, 0, 2.0) == 0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_SUCCESS);
    cnk_vector_free(v);
}

static void test_non_finite_matrix_result(void) {
    cnk_matrix *a = cnk_matrix_create(1, 1);
    cnk_matrix *b = cnk_matrix_create(1, 1);

    EXPECT_NOT_NULL(a);
    EXPECT_NOT_NULL(b);
    EXPECT_TRUE(cnk_matrix_set(a, 0, 0, DBL_MAX) == 0);
    EXPECT_TRUE(cnk_matrix_set(b, 0, 0, 2.0) == 0);

    EXPECT_NULL(cnk_matrix_multiply(a, b));
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);

    cnk_matrix_free(a);
    cnk_matrix_free(b);
}

static void test_error_message_is_copied(void) {
    char message[] = "temporary message";

    cnk_set_last_error(CNK_ERROR_UNKNOWN, message);
    message[0] = 'X';

    EXPECT_TRUE(strcmp(cnk_get_last_error_message(), "temporary message") == 0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_UNKNOWN);
}

static void check_scaled_linear_system(double scale) {
    cnk_matrix *matrix = cnk_matrix_create(2, 2);
    cnk_vector *rhs = cnk_vector_create(2);

    EXPECT_NOT_NULL(matrix);
    EXPECT_NOT_NULL(rhs);

    cnk_matrix_set(matrix, 0, 0, 3.0 * scale);
    cnk_matrix_set(matrix, 0, 1, 1.0 * scale);
    cnk_matrix_set(matrix, 1, 0, 1.0 * scale);
    cnk_matrix_set(matrix, 1, 1, 2.0 * scale);
    cnk_vector_set(rhs, 0, 5.0 * scale);
    cnk_vector_set(rhs, 1, 5.0 * scale);

    cnk_vector *solution = cnk_linalg_solve_gaussian(matrix, rhs);
    EXPECT_NOT_NULL(solution);
    if (solution) {
        EXPECT_ALMOST_EQ(cnk_vector_get(solution, 0), 1.0);
        EXPECT_ALMOST_EQ(cnk_vector_get(solution, 1), 2.0);
        EXPECT_TRUE(relative_residual(matrix, solution, rhs) < 1e-12);
    }

    cnk_matrix_free(matrix);
    cnk_vector_free(rhs);
    cnk_vector_free(solution);
}

static void test_scale_invariant_gaussian_solve(void) {
    check_scaled_linear_system(1e-150);
    check_scaled_linear_system(1e150);
}

static void test_one_by_one_and_pivoting_solve(void) {
    cnk_matrix *single = cnk_matrix_create(1, 1);
    cnk_vector *single_rhs = cnk_vector_create(1);
    cnk_matrix_set(single, 0, 0, 4.0);
    cnk_vector_set(single_rhs, 0, 10.0);

    cnk_vector *single_solution = cnk_linalg_solve_gaussian(single, single_rhs);
    EXPECT_NOT_NULL(single_solution);
    EXPECT_ALMOST_EQ(cnk_vector_get(single_solution, 0), 2.5);

    cnk_matrix *pivot = cnk_matrix_create(2, 2);
    cnk_vector *pivot_rhs = cnk_vector_create(2);
    cnk_matrix_set(pivot, 0, 0, 0.0);
    cnk_matrix_set(pivot, 0, 1, 2.0);
    cnk_matrix_set(pivot, 1, 0, 1.0);
    cnk_matrix_set(pivot, 1, 1, 1.0);
    cnk_vector_set(pivot_rhs, 0, 4.0);
    cnk_vector_set(pivot_rhs, 1, 3.0);

    cnk_vector *pivot_solution = cnk_linalg_solve_gaussian(pivot, pivot_rhs);
    EXPECT_NOT_NULL(pivot_solution);
    EXPECT_ALMOST_EQ(cnk_vector_get(pivot_solution, 0), 1.0);
    EXPECT_ALMOST_EQ(cnk_vector_get(pivot_solution, 1), 2.0);

    cnk_matrix_free(single);
    cnk_vector_free(single_rhs);
    cnk_vector_free(single_solution);
    cnk_matrix_free(pivot);
    cnk_vector_free(pivot_rhs);
    cnk_vector_free(pivot_solution);
}

static void test_non_finite_linear_system(void) {
    cnk_matrix *matrix = cnk_matrix_identity(2);
    cnk_vector *rhs = cnk_vector_create(2);

    /* Public data fields allow this test to simulate externally corrupted input. */
    matrix->data[1] = NAN;
    EXPECT_NULL(cnk_linalg_solve_gaussian(matrix, rhs));
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);

    matrix->data[1] = 0.0;
    rhs->data[0] = INFINITY;
    EXPECT_NULL(cnk_linalg_solve_gaussian(matrix, rhs));
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);

    cnk_matrix_free(matrix);
    cnk_vector_free(rhs);
}

TEST_SUITE("Basic Operations and Error Handling")
    RUN_TEST(test_vector_dot);
    RUN_TEST(test_vector_norm2);
    RUN_TEST(test_matrix_multiply);
    RUN_TEST(test_gaussian_solve);
    RUN_TEST(test_error_handling);
    RUN_TEST(test_invalid_arguments);
    RUN_TEST(test_singular_matrix_error);
    RUN_TEST(test_nan_inf_propagation);
    RUN_TEST(test_near_singular_matrix);
    RUN_TEST(test_error_state_and_output_policy);
    RUN_TEST(test_non_finite_matrix_result);
    RUN_TEST(test_error_message_is_copied);
    RUN_TEST(test_scale_invariant_gaussian_solve);
    RUN_TEST(test_one_by_one_and_pivoting_solve);
    RUN_TEST(test_non_finite_linear_system);
TEST_SUITE_END()
