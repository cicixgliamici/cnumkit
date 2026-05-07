#include "cnumkit/linalg.h"
#include "cnumkit/matrix.h"
#include "cnumkit/vector.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#define TEST_EPSILON 1e-9

static int almost_equal(double a, double b) {
    return fabs(a - b) < TEST_EPSILON;
}

static void test_vector_dot(void) {
    cnk_vector *a = cnk_vector_create(3);
    cnk_vector *b = cnk_vector_create(3);

    assert(a != NULL);
    assert(b != NULL);

    cnk_vector_set(a, 0, 1.0);
    cnk_vector_set(a, 1, 2.0);
    cnk_vector_set(a, 2, 3.0);

    cnk_vector_set(b, 0, 4.0);
    cnk_vector_set(b, 1, 5.0);
    cnk_vector_set(b, 2, 6.0);

    double result = 0.0;
    assert(cnk_vector_dot(a, b, &result) == 0);
    assert(almost_equal(result, 32.0));

    cnk_vector_free(a);
    cnk_vector_free(b);
}

static void test_matrix_multiply(void) {
    cnk_matrix *A = cnk_matrix_create(2, 2);
    cnk_matrix *B = cnk_matrix_create(2, 2);

    assert(A != NULL);
    assert(B != NULL);

    cnk_matrix_set(A, 0, 0, 1.0);
    cnk_matrix_set(A, 0, 1, 2.0);
    cnk_matrix_set(A, 1, 0, 3.0);
    cnk_matrix_set(A, 1, 1, 4.0);

    cnk_matrix_set(B, 0, 0, 5.0);
    cnk_matrix_set(B, 0, 1, 6.0);
    cnk_matrix_set(B, 1, 0, 7.0);
    cnk_matrix_set(B, 1, 1, 8.0);

    cnk_matrix *C = cnk_matrix_multiply(A, B);

    assert(C != NULL);
    assert(almost_equal(cnk_matrix_get(C, 0, 0), 19.0));
    assert(almost_equal(cnk_matrix_get(C, 0, 1), 22.0));
    assert(almost_equal(cnk_matrix_get(C, 1, 0), 43.0));
    assert(almost_equal(cnk_matrix_get(C, 1, 1), 50.0));

    cnk_matrix_free(A);
    cnk_matrix_free(B);
    cnk_matrix_free(C);
}

static void test_solve_gaussian(void) {
    /*
     * System:
     *
     * 2x + y = 5
     * x - y = 1
     *
     * Solution:
     * x = 2
     * y = 1
     */

    cnk_matrix *A = cnk_matrix_create(2, 2);
    cnk_vector *b = cnk_vector_create(2);

    assert(A != NULL);
    assert(b != NULL);

    cnk_matrix_set(A, 0, 0, 2.0);
    cnk_matrix_set(A, 0, 1, 1.0);
    cnk_matrix_set(A, 1, 0, 1.0);
    cnk_matrix_set(A, 1, 1, -1.0);

    cnk_vector_set(b, 0, 5.0);
    cnk_vector_set(b, 1, 1.0);

    cnk_vector *x = cnk_linalg_solve_gaussian(A, b);

    assert(x != NULL);
    assert(almost_equal(cnk_vector_get(x, 0), 2.0));
    assert(almost_equal(cnk_vector_get(x, 1), 1.0));

    cnk_matrix_free(A);
    cnk_vector_free(b);
    cnk_vector_free(x);
}

int main(void) {
    test_vector_dot();
    test_matrix_multiply();
    test_solve_gaussian();

    printf("All tests passed.\n");

    return 0;
}
