#ifndef CNUMKIT_OPTIM_H
#define CNUMKIT_OPTIM_H

/**
 * @file optim.h
 * @brief Optimization algorithms and numerical derivatives.
 */

#include "cnumkit_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function pointer type for a generic scalar mathematical function f(x).
 */
typedef double (*cnk_scalar_function)(double x);

/**
 * @brief Computes the numerical derivative of a scalar function f at a given point x.
 * 
 * Uses the central difference method:
 * derivative ~= (f(x + h) - f(x - h)) / (2 * h)
 * 
 * @param f The scalar function to differentiate.
 * @param x The point at which to evaluate the derivative.
 * @param h A finite signed step with |h| >= 1e-12. Both x+h and x-h must
 *          differ from x in floating-point arithmetic. A small step (e.g.,
 *          1e-5) is typical, but its accuracy depends on the function and scale.
 * @param result Pointer to a double where the computed derivative will be stored.
 * @return 0 on success, or -1 on error.
 * @note result remains unchanged on failure. Non-finite inputs, a step below
 *       the floor, or a step that rounds away set CNK_ERROR_INVALID_ARGUMENT.
 *       Non-finite sample points, function evaluations, or the final derivative
 *       set CNK_ERROR_MATH. Both signs of h are supported.
 */
CNUMKIT_EXPORT int cnk_optim_numerical_derivative(
    cnk_scalar_function f,
    double x,
    double h,
    double *result
);

/**
 * @brief Finds a local minimum of a scalar function using gradient descent.
 * 
 * Iteratively steps in the opposite direction of the gradient to find a local minimum.
 * 
 * @param f The scalar function to minimize.
 * @param initial_x The starting point for the optimization.
 * @param learning_rate The step size multiplier (alpha) for each iteration.
 * @param iterations The maximum number of iterations to perform.
 * @param result Pointer to a double where the resulting x (the minimum) will be stored.
 * @return 0 on success, or -1 on error.
 * @note result remains unchanged on failure. The learning rate must be positive,
 *       iterations must be positive, and all intermediate values must be finite.
 *       The fixed derivative step of 1e-6 must move both sample points at each
 *       iterate; otherwise the derivative reports CNK_ERROR_INVALID_ARGUMENT.
 */
CNUMKIT_EXPORT int cnk_optim_gradient_descent_1d(
    cnk_scalar_function f,
    double initial_x,
    double learning_rate,
    int iterations,
    double *result
);

#ifdef __cplusplus
}
#endif

#endif /* CNUMKIT_OPTIM_H */
