#include "cnumkit.h"

#include <stdio.h>

int main(void) {
    /*
     * Simple synthetic dataset:
     *
     * y = 2x + 1
     */

    cnk_vector *x = cnk_vector_create(6);
    cnk_vector *y = cnk_vector_create(6);

    if (!x || !y) {
        fprintf(stderr, "Allocation error.\n");
        cnk_vector_free(x);
        cnk_vector_free(y);
        return 1;
    }

    cnk_vector_set(x, 0, 0.0);
    cnk_vector_set(x, 1, 1.0);
    cnk_vector_set(x, 2, 2.0);
    cnk_vector_set(x, 3, 3.0);
    cnk_vector_set(x, 4, 4.0);
    cnk_vector_set(x, 5, 5.0);

    cnk_vector_set(y, 0, 1.0);
    cnk_vector_set(y, 1, 3.0);
    cnk_vector_set(y, 2, 5.0);
    cnk_vector_set(y, 3, 7.0);
    cnk_vector_set(y, 4, 9.0);
    cnk_vector_set(y, 5, 11.0);

    cnk_linear_regression_model model;

    int status = cnk_ml_linear_regression_fit(
        x,
        y,
        0.01,
        5000,
        &model
    );

    if (status != 0) {
        fprintf(stderr, "Could not fit linear regression model.\n");
        cnk_vector_free(x);
        cnk_vector_free(y);
        return 1;
    }

    double mse = 0.0;
    cnk_ml_mean_squared_error(x, y, &model, &mse);

    printf("Fitted model:\n");
    printf("y = %.6f * x + %.6f\n", model.weight, model.bias);
    printf("MSE = %.10f\n", mse);

    printf("\nPredictions:\n");

    for (double input = 0.0; input <= 6.0; input += 1.0) {
        double prediction = cnk_ml_linear_regression_predict(&model, input);
        printf("x = %.2f -> y = %.6f\n", input, prediction);
    }

    cnk_vector_free(x);
    cnk_vector_free(y);

    return 0;
}
