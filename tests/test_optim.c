#include "cnumkit/optim.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#define TEST_EPSILON 1e-3

static double quadratic(double x) {
    /*
     * Minimum at x = 3.
     */
    return (x - 3.0) * (x - 3.0);
}

static int almost_equal(double a, double b) {
    return fabs(a - b) < TEST_EPSILON;
}

static void test_numerical_derivative(void) {
    /*
     * f(x) = (x - 3)^2
     * f'(x) = 2(x - 3)
     *
     * At x = 5:
     * f'(5) = 4
     */

    double derivative = 0.0;

    int status = cnk_optim_numerical_derivative(
        quadratic,
        5.0,
        1e-6,
        &derivative
    );

    assert(status == 0);
    assert(almost_equal(derivative, 4.0));
}

static void test_gradient_descent(void) {
    double result = 0.0;

    int status = cnk_optim_gradient_descent_1d(
        quadratic,
        10.0,
        0.1,
        100,
        &result
    );

    assert(status == 0);
    assert(almost_equal(result, 3.0));
}

int main(void) {
    test_numerical_derivative();
    test_gradient_descent();

    printf("All optimization tests passed.\n");

    return 0;
}
