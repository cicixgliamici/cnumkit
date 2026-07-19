#ifndef CNUMKIT_ML_H
#define CNUMKIT_ML_H

/**
 * @file ml.h
 * @brief Basic Machine Learning utilities and models.
 */

#include <stddef.h>
#include "cnumkit/vector.h"
#include "cnumkit_export.h"

/**
 * @brief Represents a simple 1D linear regression model.
 * 
 * The model equation is: y = weight * x + bias
 */
typedef struct {
    double weight; /**< The slope (coefficient) of the linear model */
    double bias;   /**< The y-intercept of the linear model */
} cnk_linear_regression_model;

/**
 * @brief Predicts an output value 'y' for a given input 'x' using a trained model.
 * 
 * @param model Pointer to the trained linear regression model.
 * @param x The input feature value.
 * @return The predicted output, or 0.0 on invalid/non-finite input or math failure.
 * @note Check cnk_get_last_error() to distinguish an error from a valid 0.0 prediction.
 */
CNUMKIT_EXPORT double cnk_ml_linear_regression_predict(
    const cnk_linear_regression_model *model,
    double x
);

/**
 * @brief Fits a 1D linear regression model to a dataset using gradient descent.
 * 
 * The algorithm minimizes the Mean Squared Error (MSE) between the model predictions
 * and the target labels 'y'.
 * 
 * @param x Vector of input features.
 * @param y Vector of target values (labels).
 * @param learning_rate The step size for the gradient descent optimizer.
 * @param epochs The number of training iterations.
 * @param model Pointer to the model structure to populate with optimized weights.
 * @return 0 on success, or -1 if the vectors mismatch in size or are NULL.
 * @note model remains unchanged on failure. Inputs and learned parameters must be finite.
 */
CNUMKIT_EXPORT int cnk_ml_linear_regression_fit(
    const cnk_vector *x,
    const cnk_vector *y,
    double learning_rate,
    int epochs,
    cnk_linear_regression_model *model
);

/**
 * @brief Computes the Mean Squared Error (MSE) for a given model and dataset.
 * 
 * MSE = mean( (predicted_y - actual_y)^2 )
 * 
 * @param x Vector of input features.
 * @param y Vector of actual target values.
 * @param model Pointer to the linear regression model to evaluate.
 * @param result Pointer to a double where the computed MSE will be stored.
 * @return 0 on success, or -1 if the vectors mismatch in size or are NULL.
 * @note result remains unchanged on failure. Inputs and model parameters must be finite.
 */
CNUMKIT_EXPORT int cnk_ml_mean_squared_error(
    const cnk_vector *x,
    const cnk_vector *y,
    const cnk_linear_regression_model *model,
    double *result
);

#endif /* CNUMKIT_ML_H */
