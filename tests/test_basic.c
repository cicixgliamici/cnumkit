#include "cnumkit.h"
#include "test_framework.h"

#include <stdint.h>

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

TEST_SUITE("Basic Operations and Error Handling")
    RUN_TEST(test_vector_dot);
    RUN_TEST(test_vector_norm2);
    RUN_TEST(test_matrix_multiply);
    RUN_TEST(test_gaussian_solve);
    RUN_TEST(test_error_handling);
    RUN_TEST(test_invalid_arguments);
    RUN_TEST(test_singular_matrix_error);
TEST_SUITE_END()
