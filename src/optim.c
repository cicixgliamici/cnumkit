#include "cnumkit.h"

#include <float.h>
#include <math.h>
#include <stddef.h>

#define CNK_OPTIM_EPSILON 1e-12

static int validate_sample_points(double x, double plus, double minus) {
    /* Validate before callbacks: a bounded function could hide infinite inputs. */
    if (!isfinite(plus) || !isfinite(minus)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Derivative sample point is non-finite");
        return -1;
    }
    if (plus == x || minus == x) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Derivative step rounds away at the evaluation point");
        return -1;
    }
    return 0;
}

static double central_difference(double plus, double minus, double step) {
    double difference = plus - minus;

    /* Halve first only when subtraction or doubling the step would overflow. */
    if (!isfinite(difference) || fabs(step) > DBL_MAX / 2.0) {
        return (0.5 * plus - 0.5 * minus) / step;
    }
    /* The ordinary form preserves tiny differences that halving could erase. */
    return difference / (2.0 * step);
}

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

    double sample_plus = x + h;
    double sample_minus = x - h;
    if (validate_sample_points(x, sample_plus, sample_minus) != 0) {
        return -1;
    }

    double fx_plus = f(sample_plus);
    double fx_minus = f(sample_minus);
    if (!isfinite(fx_plus) || !isfinite(fx_minus)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Derivative function evaluation is non-finite");
        return -1;
    }

    double derivative = central_difference(fx_plus, fx_minus, h);
    if (!isfinite(derivative)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Numerical derivative produced a non-finite result");
        return -1;
    }

    *result = derivative;
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
