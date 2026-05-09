#include "cnumkit.h"
#include "test_framework.h"

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

TEST_SUITE("Basic Operations and Error Handling")
    RUN_TEST(test_vector_dot);
    RUN_TEST(test_matrix_multiply);
    RUN_TEST(test_error_handling);
TEST_SUITE_END()
