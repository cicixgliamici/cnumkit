#include "cnumkit/ml.h"

#include <stddef.h>

double cnk_ml_linear_regression_predict(
    const cnk_linear_regression_model *model,
    double x
) {
    if (!model) {
        return 0.0;
    }

    return model->weight * x + model->bias;
}

int cnk_ml_linear_regression_fit(
    const cnk_vector *x,
    const cnk_vector *y,
    double learning_rate,
    int epochs,
    cnk_linear_regression_model *model
) {
    if (!x || !y || !model || !x->data || !y->data) {
        return -1;
    }

    if (x->size == 0 || x->size != y->size) {
        return -1;
    }

    if (learning_rate <= 0.0 || epochs <= 0) {
        return -1;
    }

    double weight = 0.0;
    double bias = 0.0;
    double n = (double)x->size;

    for (int epoch = 0; epoch < epochs; epoch++) {
        double d_weight = 0.0;
        double d_bias = 0.0;

        for (size_t i = 0; i < x->size; i++) {
            double xi = cnk_vector_get(x, i);
            double yi = cnk_vector_get(y, i);

            double prediction = weight * xi + bias;
            double error = prediction - yi;

            d_weight += error * xi;
            d_bias += error;
        }

        d_weight = (2.0 / n) * d_weight;
        d_bias = (2.0 / n) * d_bias;

        weight = weight - learning_rate * d_weight;
        bias = bias - learning_rate * d_bias;
    }

    model->weight = weight;
    model->bias = bias;

    return 0;
}

int cnk_ml_mean_squared_error(
    const cnk_vector *x,
    const cnk_vector *y,
    const cnk_linear_regression_model *model,
    double *result
) {
    if (!x || !y || !model || !result || !x->data || !y->data) {
        return -1;
    }

    if (x->size == 0 || x->size != y->size) {
        return -1;
    }

    double sum = 0.0;

    for (size_t i = 0; i < x->size; i++) {
        double xi = cnk_vector_get(x, i);
        double yi = cnk_vector_get(y, i);

        double prediction = cnk_ml_linear_regression_predict(model, xi);
        double error = prediction - yi;

        sum += error * error;
    }

    *result = sum / (double)x->size;

    return 0;
}
