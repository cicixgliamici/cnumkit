#include "cnumkit.h"

#include <float.h>

#define CNK_TEST_EPSILON 1e-3
#include "test_framework.h"

static double quadratic(double x) {
    /*
     * Minimum at x = 3.
     */
    return (x - 3.0) * (x - 3.0);
}

static double non_finite_function(double x) {
    (void)x;
    return NAN;
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

static void test_optim_nan_inf(void) {
    double result = 7.0;

    /* Numerical derivative with NAN */
    EXPECT_TRUE(cnk_optim_numerical_derivative(quadratic, NAN, 1e-6, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_ALMOST_EQ(result, 7.0);

    /* Gradient descent with INFINITY */
    EXPECT_TRUE(cnk_optim_gradient_descent_1d(quadratic, INFINITY, 0.1, 10, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_ALMOST_EQ(result, 7.0);

    EXPECT_TRUE(cnk_optim_numerical_derivative(non_finite_function, 1.0, 1e-6, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    EXPECT_ALMOST_EQ(result, 7.0);
}

static void test_derivative_step_and_divergence(void) {
    double result = 11.0;

    EXPECT_TRUE(cnk_optim_numerical_derivative(quadratic, 1.0, DBL_MIN, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_ALMOST_EQ(result, 11.0);

    EXPECT_TRUE(cnk_optim_gradient_descent_1d(quadratic, 1e200, 1.0, 10, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    EXPECT_ALMOST_EQ(result, 11.0);
}

TEST_SUITE("Optimization and Solvers")
    RUN_TEST(test_numerical_derivative);
    RUN_TEST(test_gradient_descent);
    RUN_TEST(test_optim_invalid_arguments);
    RUN_TEST(test_optim_nan_inf);
    RUN_TEST(test_derivative_step_and_divergence);
TEST_SUITE_END()
