#include "cnumkit.h"

#define CNK_TEST_EPSILON 1e-3
#include "test_framework.h"

static double quadratic(double x) {
    /*
     * Minimum at x = 3.
     */
    return (x - 3.0) * (x - 3.0);
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

    EXPECT_TRUE(status == 0);
    EXPECT_ALMOST_EQ(derivative, 4.0);
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

    EXPECT_TRUE(status == 0);
    EXPECT_ALMOST_EQ(result, 3.0);
}

static void test_optim_invalid_arguments(void) {
    double result = 0.0;

    EXPECT_TRUE(cnk_optim_numerical_derivative(NULL, 1.0, 1e-6, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_TRUE(cnk_optim_numerical_derivative(quadratic, 1.0, 0.0, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_TRUE(cnk_optim_gradient_descent_1d(quadratic, 1.0, -0.1, 10, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
}

TEST_SUITE("Optimization and Solvers")
    RUN_TEST(test_numerical_derivative);
    RUN_TEST(test_gradient_descent);
    RUN_TEST(test_optim_invalid_arguments);
TEST_SUITE_END()
