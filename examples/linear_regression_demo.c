#include "cnumkit.h"

#include <stdio.h>

static int populate_dataset(cnk_vector *x, cnk_vector *y) {
    for (size_t i = 0; i < x->size; i++) {
        double input = (double)i;
        if (cnk_vector_set(x, i, input) != 0 ||
            cnk_vector_set(y, i, 2.0 * input + 1.0) != 0) {
            return -1;
        }
    }

    return 0;
}

int main(void) {
    /*
     * Simple synthetic dataset:
     *
     * y = 2x + 1
     */

    cnk_vector *x = cnk_vector_create(6);
    cnk_vector *y = cnk_vector_create(6);

    if (!x || !y) {
        fprintf(stderr, "Could not allocate the dataset: %s\n", cnk_get_last_error_message());
        cnk_vector_free(x);
        cnk_vector_free(y);
        return 1;
    }

    if (populate_dataset(x, y) != 0) {
        fprintf(stderr, "Could not initialize the dataset: %s\n", cnk_get_last_error_message());
        cnk_vector_free(x);
        cnk_vector_free(y);
        return 1;
    }

    cnk_linear_regression_model model;

    int status = cnk_ml_linear_regression_fit(
        x,
        y,
        0.01,
        5000,
        &model
    );

    if (status != 0) {
        fprintf(stderr, "Could not fit the model: %s\n", cnk_get_last_error_message());
        cnk_vector_free(x);
        cnk_vector_free(y);
        return 1;
    }

    double mse = 0.0;
    if (cnk_ml_mean_squared_error(x, y, &model, &mse) != 0) {
        fprintf(stderr, "Could not evaluate the model: %s\n", cnk_get_last_error_message());
        cnk_vector_free(x);
        cnk_vector_free(y);
        return 1;
    }

    printf("Fitted model:\n");
    printf("y = %.6f * x + %.6f\n", model.weight, model.bias);
    printf("MSE = %.10f\n", mse);

    printf("\nPredictions:\n");

    for (double input = 0.0; input <= 6.0; input += 1.0) {
        double prediction = cnk_ml_linear_regression_predict(&model, input);
        if (cnk_get_last_error() != CNK_SUCCESS) {
            fprintf(stderr, "Could not predict a value: %s\n", cnk_get_last_error_message());
            cnk_vector_free(x);
            cnk_vector_free(y);
            return 1;
        }
        printf("x = %.2f -> y = %.6f\n", input, prediction);
    }

    cnk_vector_free(x);
    cnk_vector_free(y);

    return 0;
}
