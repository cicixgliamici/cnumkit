#include "cnumkit.h"
#include "test_framework.h"

#include <string.h>

static void test_sensor_calibration_pipeline(void) {
    /* Interleaved channels include padding that must not enter the calculation. */
    double samples[] = {2.0, 99.0, -1.0, 99.0, 2.0};
    double baseline[] = {-1.0, 0.0, 1.0};
    double corrected[] = {0.0, 0.0, 0.0};
    double calibration[] = {1.0, 1.0, 0.0, 0.0, 1.0, -1.0};
    double output[] = {0.0, 0.0};
    double energy = -1.0;
    double length = -1.0;

    EXPECT_TRUE(cnk_f64_copy(cnk_f64_const_view_make(samples, 3, 2),
        cnk_f64_view_make(corrected, 3, 1)) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_f64_axpy(1.0, cnk_f64_const_view_make(baseline, 3, 1),
        cnk_f64_view_make(corrected, 3, 1)) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_f64_matvec(
        cnk_f64_const_matrix_view_make(calibration, 2, 3, 3, 1),
        cnk_f64_const_view_make(corrected, 3, 1),
        cnk_f64_view_make(output, 2, 1)) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(output[0], 0.0);
    EXPECT_ALMOST_EQ(output[1], -4.0);
    EXPECT_TRUE(cnk_f64_dot(cnk_f64_const_view_make(output, 2, 1),
        cnk_f64_const_view_make(output, 2, 1), &energy) == CNK_SUCCESS);
    EXPECT_TRUE(cnk_f64_norm2(cnk_f64_const_view_make(output, 2, 1),
        &length) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(energy, 16.0);
    EXPECT_ALMOST_EQ(length, 4.0);
    EXPECT_ALMOST_EQ(samples[1], 99.0);
    EXPECT_ALMOST_EQ(samples[3], 99.0);
}

static void test_matrix_product_and_identity(void) {
    /* A is 2x3 and B is 3x2, so this also checks rectangular indexing. */
    double left[] = {1.0, -2.0, 3.0, 0.0, 4.0, -1.0};
    double right[] = {2.0, 1.0, -1.0, 3.0, 4.0, 0.0};
    double product[4] = {0.0};
    double identity[] = {1.0, 0.0, 0.0, 1.0};
    double restored[4] = {0.0};

    EXPECT_TRUE(cnk_f64_matmul(cnk_f64_const_matrix_view_make(left, 2, 3, 3, 1),
        cnk_f64_const_matrix_view_make(right, 3, 2, 2, 1),
        cnk_f64_matrix_view_make(product, 2, 2, 2, 1)) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(product[0], 16.0);
    EXPECT_ALMOST_EQ(product[1], -5.0);
    EXPECT_ALMOST_EQ(product[2], -8.0);
    EXPECT_ALMOST_EQ(product[3], 12.0);
    EXPECT_TRUE(cnk_f64_matmul(cnk_f64_const_matrix_view_make(product, 2, 2, 2, 1),
        cnk_f64_const_matrix_view_make(identity, 2, 2, 2, 1),
        cnk_f64_matrix_view_make(restored, 2, 2, 2, 1)) == CNK_SUCCESS);
    EXPECT_TRUE(memcmp(product, restored, sizeof(product)) == 0);
}

static void test_owned_objects_and_borrowed_views_agree(void) {
    cnk_matrix *matrix = cnk_matrix_create(2, 2);
    cnk_vector *vector = cnk_vector_create(2);
    cnk_vector *rhs = cnk_vector_create(2);
    EXPECT_NOT_NULL(matrix);
    EXPECT_NOT_NULL(vector);
    EXPECT_NOT_NULL(rhs);
    if (!matrix || !vector || !rhs) {
        cnk_matrix_free(matrix);
        cnk_vector_free(vector);
        cnk_vector_free(rhs);
        return;
    }

    /* The known solution [2, -1] also requires a row swap at the first pivot. */
    const double coefficients[] = {0.0, 2.0, 3.0, 1.0};
    for (size_t row = 0; row < 2; ++row) {
        for (size_t column = 0; column < 2; ++column) {
            EXPECT_TRUE(cnk_matrix_set(matrix, row, column,
                coefficients[row * 2 + column]) == 0);
        }
    }
    EXPECT_TRUE(cnk_vector_set(rhs, 0, -2.0) == 0);
    EXPECT_TRUE(cnk_vector_set(rhs, 1, 5.0) == 0);
    cnk_vector *solution = cnk_linalg_solve_gaussian(matrix, rhs);
    EXPECT_NOT_NULL(solution);
    if (solution) {
        cnk_f64_const_matrix_view matrix_view;
        cnk_f64_const_view solution_view;
        double reconstructed[] = {0.0, 0.0};
        EXPECT_TRUE(cnk_matrix_as_const_view(matrix, &matrix_view) == CNK_SUCCESS);
        EXPECT_TRUE(cnk_vector_as_const_view(solution, &solution_view) == CNK_SUCCESS);
        EXPECT_TRUE(cnk_f64_matvec(matrix_view, solution_view,
            cnk_f64_view_make(reconstructed, 2, 1)) == CNK_SUCCESS);
        EXPECT_ALMOST_EQ(cnk_vector_get(solution, 0), 2.0);
        EXPECT_ALMOST_EQ(cnk_vector_get(solution, 1), -1.0);
        EXPECT_ALMOST_EQ(reconstructed[0], -2.0);
        EXPECT_ALMOST_EQ(reconstructed[1], 5.0);
        EXPECT_ALMOST_EQ(cnk_matrix_get(matrix, 1, 0), 3.0);
    }

    EXPECT_TRUE(cnk_vector_set(vector, 0, 3.0) == 0);
    EXPECT_TRUE(cnk_vector_set(vector, 1, 4.0) == 0);
    double owned_norm = -1.0;
    double borrowed_norm = -1.0;
    EXPECT_TRUE(cnk_vector_norm2(vector, &owned_norm) == 0);
    EXPECT_TRUE(cnk_f64_norm2(cnk_f64_const_view_make(vector->data, 2, 1),
        &borrowed_norm) == CNK_SUCCESS);
    EXPECT_ALMOST_EQ(owned_norm, 5.0);
    EXPECT_ALMOST_EQ(borrowed_norm, owned_norm);

    cnk_matrix_free(matrix);
    cnk_vector_free(vector);
    cnk_vector_free(rhs);
    cnk_vector_free(solution);
}

TEST_SUITE("Mathematical Use Scenarios")
    RUN_TEST(test_sensor_calibration_pipeline);
    RUN_TEST(test_matrix_product_and_identity);
    RUN_TEST(test_owned_objects_and_borrowed_views_agree);
TEST_SUITE_END()
