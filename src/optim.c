#include "cnumkit.h"

#include <math.h>
#include <stddef.h>

#define CNK_OPTIM_EPSILON 1e-12

int cnk_optim_numerical_derivative(
    cnk_scalar_function f,
    double x,
    double h,
    double *result
) {
    if (!f || !result) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Null pointer passed to cnk_optim_numerical_derivative");
        return -1;
    }

    if (!isfinite(x) || !isfinite(h) || fabs(h) < CNK_OPTIM_EPSILON) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Derivative point and step must be finite, and step must be nonzero");
        return -1;
    }

    double fx_plus = f(x + h);
    double fx_minus = f(x - h);

    *result = (fx_plus - fx_minus) / (2.0 * h);
    if (!isfinite(*result)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Numerical derivative produced a non-finite result");
        return -1;
    }

    cnk_set_last_error(CNK_SUCCESS, NULL);
    return 0;
}

int cnk_optim_gradient_descent_1d(
    cnk_scalar_function f,
    double initial_x,
    double learning_rate,
    int iterations,
    double *result
) {
    if (!f || !result) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Null pointer passed to cnk_optim_gradient_descent_1d");
        return -1;
    }

    if (!isfinite(initial_x) || !isfinite(learning_rate) || learning_rate <= 0.0 || iterations <= 0) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Gradient descent parameters are invalid");
        return -1;
    }

    double x = initial_x;

    for (int i = 0; i < iterations; i++) {
        double gradient = 0.0;

        if (cnk_optim_numerical_derivative(f, x, 1e-6, &gradient) != 0) {
            return -1;
        }

        x = x - learning_rate * gradient;
        if (!isfinite(x)) {
            cnk_set_last_error(CNK_ERROR_MATH, "Gradient descent diverged to a non-finite value");
            return -1;
        }
    }

    *result = x;

    cnk_set_last_error(CNK_SUCCESS, NULL);
    return 0;
}
