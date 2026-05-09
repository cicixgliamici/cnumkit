#include "cnumkit.h"

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

TEST_SUITE("Machine Learning")
    RUN_TEST(test_linear_regression_fit);
TEST_SUITE_END()
