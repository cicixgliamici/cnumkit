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
        return -1;
    }

    if (fabs(h) < CNK_OPTIM_EPSILON) {
        return -1;
    }

    double fx_plus = f(x + h);
    double fx_minus = f(x - h);

    *result = (fx_plus - fx_minus) / (2.0 * h);

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
        return -1;
    }

    if (learning_rate <= 0.0 || iterations <= 0) {
        return -1;
    }

    double x = initial_x;

    for (int i = 0; i < iterations; i++) {
        double gradient = 0.0;

        if (cnk_optim_numerical_derivative(f, x, 1e-6, &gradient) != 0) {
            return -1;
        }

        x = x - learning_rate * gradient;
    }

    *result = x;

    return 0;
}
