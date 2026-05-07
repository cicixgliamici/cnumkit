#ifndef CNUMKIT_OPTIM_H
#define CNUMKIT_OPTIM_H

/*
 * Type for a scalar function f(x).
 */
typedef double (*cnk_scalar_function)(double x);

/*
 * Computes the numerical derivative of f at x using central difference.
 *
 * derivative ≈ (f(x + h) - f(x - h)) / (2h)
 *
 * Returns 0 on success, -1 on error.
 */
int cnk_optim_numerical_derivative(
    cnk_scalar_function f,
    double x,
    double h,
    double *result
);

/*
 * Performs gradient descent on a scalar function.
 *
 * Parameters:
 * - f: function to minimize
 * - initial_x: starting point
 * - learning_rate: step size
 * - iterations: number of iterations
 *
 * Returns 0 on success, -1 on error.
 */
int cnk_optim_gradient_descent_1d(
    cnk_scalar_function f,
    double initial_x,
    double learning_rate,
    int iterations,
    double *result
);

#endif
