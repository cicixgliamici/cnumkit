#include "cnumkit/ml.h"
#include "cnumkit/vector.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#define TEST_EPSILON 0.15

static int almost_equal(double a, double b) {
    return fabs(a - b) < TEST_EPSILON;
}

static void test_linear_regression_fit(void) {
    /*
     * Dataset:
     *
     * y = 2x + 1
     */

    cnk_vector *x = cnk_vector_create(5);
    cnk_vector *y = cnk_vector_create(5);

    assert(x != NULL);
    assert(y != NULL);

    cnk_vector_set(x, 0, 0.0);
    cnk_vector_set(x, 1, 1.0);
    cnk_vector_set(x, 2, 2.0);
    cnk_vector_set(x, 3, 3.0);
    cnk_vector_set(x, 4, 4.0);

    cnk_vector_set(y, 0, 1.0);
    cnk_vector_set(y, 1, 3.0);
    cnk_vector_set(y, 2, 5.0);
    cnk_vector_set(y, 3, 7.0);
    cnk_vector_set(y, 4, 9.0);

    cnk_linear_regression_model model;

    int status = cnk_ml_linear_regression_fit(
        x,
        y,
        0.01,
        3000,
        &model
    );

    assert(status == 0);

    assert(almost_equal(model.weight, 2.0));
    assert(almost_equal(model.bias, 1.0));

    double mse = 0.0;
    assert(cnk_ml_mean_squared_error(x, y, &model, &mse) == 0);
    assert(mse < 0.05);

    cnk_vector_free(x);
    cnk_vector_free(y);
}

int main(void) {
    test_linear_regression_fit();

    printf("All ML tests passed.\n");

    return 0;
}
