#include "cnumkit.h"

#include <float.h>

#define CNK_TEST_EPSILON 0.15
#include "test_framework.h"

static void test_linear_regression_fit(void) {
    /*
     * Dataset:
     *
     * y = 2x + 1
     */

    cnk_vector *x = cnk_vector_create(5);
    cnk_vector *y = cnk_vector_create(5);

    EXPECT_NOT_NULL(x);
    EXPECT_NOT_NULL(y);

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

    EXPECT_TRUE(status == 0);

    EXPECT_ALMOST_EQ(model.weight, 2.0);
    EXPECT_ALMOST_EQ(model.bias, 1.0);

    double mse = 0.0;
    EXPECT_TRUE(cnk_ml_mean_squared_error(x, y, &model, &mse) == 0);
    EXPECT_TRUE(mse < 0.05);

    cnk_vector_free(x);
    cnk_vector_free(y);
}

static void test_linear_regression_invalid_arguments(void) {
    cnk_vector *x = cnk_vector_create(2);
    cnk_vector *y = cnk_vector_create(3);
    cnk_linear_regression_model model = {3.0, 4.0};
    double mse = 9.0;

    EXPECT_NOT_NULL(x);
    EXPECT_NOT_NULL(y);

    EXPECT_TRUE(cnk_ml_linear_regression_fit(NULL, y, 0.01, 10, &model) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_ALMOST_EQ(model.weight, 3.0);
    EXPECT_ALMOST_EQ(model.bias, 4.0);

    EXPECT_TRUE(cnk_ml_linear_regression_fit(x, y, 0.01, 10, &model) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_DIMENSION_MISMATCH);

    EXPECT_TRUE(cnk_ml_linear_regression_fit(x, x, 0.0, 10, &model) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    EXPECT_TRUE(cnk_ml_mean_squared_error(x, y, &model, &mse) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_DIMENSION_MISMATCH);
    EXPECT_ALMOST_EQ(mse, 9.0);

    cnk_vector_free(x);
    cnk_vector_free(y);
}

static void test_ml_nan_inf(void) {
    cnk_vector *x = cnk_vector_create(1);
    cnk_vector *y = cnk_vector_create(1);
    cnk_linear_regression_model model;

    EXPECT_NOT_NULL(x);
    EXPECT_NOT_NULL(y);

    cnk_vector_set(x, 0, 1.0);
    cnk_vector_set(y, 0, 2.0);

    /* Train with NAN learning rate */
    EXPECT_TRUE(cnk_ml_linear_regression_fit(x, y, NAN, 10, &model) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    /* Train with INFINITY learning rate */
    EXPECT_TRUE(cnk_ml_linear_regression_fit(x, y, INFINITY, 10, &model) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    model.weight = INFINITY;
    model.bias = 0.0;
    EXPECT_ALMOST_EQ(cnk_ml_linear_regression_predict(&model, 1.0), 0.0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);

    model.weight = 1.0;
    EXPECT_ALMOST_EQ(cnk_ml_linear_regression_predict(&model, 1.0), 1.0);
    EXPECT_TRUE(cnk_get_last_error() == CNK_SUCCESS);

    cnk_vector_free(x);
    cnk_vector_free(y);
}

static void test_ml_scaled_data_and_overflow(void) {
    cnk_vector *x = cnk_vector_create(1);
    cnk_vector *y = cnk_vector_create(1);
    cnk_linear_regression_model model = {2.0, 0.0};
    double mse = 13.0;

    cnk_vector_set(x, 0, 1e100);
    cnk_vector_set(y, 0, 2e100);
    EXPECT_TRUE(cnk_ml_mean_squared_error(x, y, &model, &mse) == 0);
    EXPECT_ALMOST_EQ(mse, 0.0);

    model.weight = DBL_MAX;
    EXPECT_TRUE(cnk_ml_mean_squared_error(x, y, &model, &mse) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    EXPECT_ALMOST_EQ(mse, 0.0);

    model.weight = 7.0;
    model.bias = 8.0;
    x->data[0] = NAN;
    EXPECT_TRUE(cnk_ml_linear_regression_fit(x, y, 0.01, 10, &model) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_MATH);
    EXPECT_ALMOST_EQ(model.weight, 7.0);
    EXPECT_ALMOST_EQ(model.bias, 8.0);

    cnk_vector_free(x);
    cnk_vector_free(y);
}

TEST_SUITE("Machine Learning")
    RUN_TEST(test_linear_regression_fit);
    RUN_TEST(test_linear_regression_invalid_arguments);
    RUN_TEST(test_ml_nan_inf);
    RUN_TEST(test_ml_scaled_data_and_overflow);
TEST_SUITE_END()
