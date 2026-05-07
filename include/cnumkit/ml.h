#ifndef CNUMKIT_ML_H
#define CNUMKIT_ML_H

#include <stddef.h>

#include "cnumkit/vector.h"

typedef struct {
    double weight;
    double bias;
} cnk_linear_regression_model;

/*
 * Computes prediction:
 *
 * y = weight * x + bias
 */
double cnk_ml_linear_regression_predict(
    const cnk_linear_regression_model *model,
    double x
);

/*
 * Fits a simple linear regression model using gradient descent.
 *
 * Model:
 *
 * y = weight * x + bias
 *
 * Loss:
 *
 * MSE = mean((prediction - target)^2)
 *
 * Returns 0 on success, -1 on error.
 */
int cnk_ml_linear_regression_fit(
    const cnk_vector *x,
    const cnk_vector *y,
    double learning_rate,
    int epochs,
    cnk_linear_regression_model *model
);

/*
 * Computes mean squared error for a fitted model.
 *
 * Returns 0 on success, -1 on error.
 */
int cnk_ml_mean_squared_error(
    const cnk_vector *x,
    const cnk_vector *y,
    const cnk_linear_regression_model *model,
    double *result
);

#endif
