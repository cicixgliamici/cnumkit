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

static double cubic(double x) {
    return x * x * x - 2.0 * x;
}

static double shifted_quadratic(double x) {
    return (x + 4.0) * (x + 4.0) + 7.0;
}

static double identity(double x) {
    return x;
}

static double scaled_identity(double x) {
    return x / DBL_MAX;
}

static double doubled_identity(double x) {
    return 2.0 * x;
}

static double tiny_step_function(double x) {
    /* A one-subnormal difference must survive until division by the step. */
    return x > 0.0 ? DBL_TRUE_MIN : 0.0;
}

static int derivative_callback_calls = 0;

static double counted_constant(double x) {
    (void)x;
    derivative_callback_calls++;
    return 1.0;
}

static void test_derivative_extreme_finite_steps(void) {
    double result = 11.0;

    /* Both samples are finite, but forming 2*h would overflow to infinity. */
    EXPECT_TRUE(cnk_optim_numerical_derivative(scaled_identity, 0.0,
        DBL_MAX, &result) == 0);
    EXPECT_TRUE(result > 0.0);
    EXPECT_TRUE(fabs(result * DBL_MAX - 1.0) < 1e-12);
    EXPECT_TRUE(cnk_get_last_error() == CNK_SUCCESS);

    /* The analytic slope is one even when subtracting samples overflows. */
    EXPECT_TRUE(cnk_optim_numerical_derivative(identity, 0.0,
        DBL_MAX, &result) == 0);
    EXPECT_TRUE(result == 1.0);
    EXPECT_TRUE(cnk_optim_numerical_derivative(doubled_identity, 0.0,
        DBL_MAX / 2.0, &result) == 0);
    EXPECT_TRUE(result == 2.0);
    EXPECT_TRUE(cnk_optim_numerical_derivative(tiny_step_function, 0.0,
        1e-12, &result) == 0);
    EXPECT_TRUE(result > 0.0);
    EXPECT_TRUE(cnk_optim_numerical_derivative(identity, 0.0,
        -DBL_MAX, &result) == 0);
    EXPECT_TRUE(result == 1.0);
}

static void test_derivative_invalid_sample_points(void) {
    double result = 11.0;
    derivative_callback_calls = 0;

    /* Reject overflow before calling a function that could conceal it. */
    EXPECT_TRUE(cnk_optim_numerical_derivative(counted_constant, DBL_MAX,
        DBL_MAX, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    EXPECT_TRUE(result == 11.0);
    EXPECT_TRUE(derivative_callback_calls == 0);

    /* A finite step can round away at a large point, producing a false zero. */
    EXPECT_TRUE(cnk_optim_numerical_derivative(counted_constant, 1e20,
        1.0, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(result == 11.0);
    EXPECT_TRUE(derivative_callback_calls == 0);
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
    /* The fixed step is unrepresentable here, before any function evaluation. */
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_ALMOST_EQ(result, 11.0);

    EXPECT_TRUE(cnk_optim_gradient_descent_1d(quadratic, 10.0, DBL_MAX, 10, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    EXPECT_ALMOST_EQ(result, 11.0);
}

static void test_derivative_at_negative_and_stationary_points(void) {
    double derivative = 99.0;

    /* The analytic derivative is 3x^2 - 2, including on the negative axis. */
    EXPECT_TRUE(cnk_optim_numerical_derivative(cubic, -2.0, 1e-5, &derivative) == 0);
    EXPECT_TRUE(fabs(derivative - 10.0) < 1e-8);
    EXPECT_TRUE(cnk_optim_numerical_derivative(shifted_quadratic, -4.0,
        1e-5, &derivative) == 0);
    EXPECT_TRUE(fabs(derivative) < 1e-8);
}

static void test_gradient_descent_on_shifted_minimum(void) {
    double minimum = 99.0;

    EXPECT_TRUE(cnk_optim_gradient_descent_1d(shifted_quadratic,
        6.0, 0.2, 80, &minimum) == 0);
    EXPECT_TRUE(fabs(minimum + 4.0) < 1e-6);
    EXPECT_TRUE(fabs(shifted_quadratic(minimum) - 7.0) < 1e-10);
}

TEST_SUITE("Optimization and Solvers")
    RUN_TEST(test_numerical_derivative);
    RUN_TEST(test_gradient_descent);
    RUN_TEST(test_optim_invalid_arguments);
    RUN_TEST(test_optim_nan_inf);
    RUN_TEST(test_derivative_step_and_divergence);
    RUN_TEST(test_derivative_at_negative_and_stationary_points);
    RUN_TEST(test_gradient_descent_on_shifted_minimum);
    RUN_TEST(test_derivative_extreme_finite_steps);
    RUN_TEST(test_derivative_invalid_sample_points);
TEST_SUITE_END()
