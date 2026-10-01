#include <cnumkit.h>

#include <math.h>
#include <stdio.h>

int main(void) {
    const double coefficients[] = {2.0, 1.0, 1.0, 3.0};
    const double values[] = {4.0, -1.0};
    double output[] = {0.0, 0.0};
    double length = 0.0;
    double prediction = 0.0;
    cnk_linear_regression_model model = {2.0, -2.0};

    /* Check that installed headers, exported symbols, and math linkage work. */
    cnk_error_code status = cnk_f64_matvec(
        cnk_f64_const_matrix_view_make(coefficients, 2, 2, 2, 1),
        cnk_f64_const_view_make(values, 2, 1),
        cnk_f64_view_make(output, 2, 1));
    if (status == CNK_SUCCESS) {
        status = cnk_f64_norm2(cnk_f64_const_view_make(output, 2, 1), &length);
    }
    if (status != CNK_SUCCESS) {
        fprintf(stderr, "cnumkit consumer failed: %s\n", cnk_error_string(status));
        return 1;
    }
    if (cnk_ml_linear_regression_try_predict(&model, 1.0, &prediction) != 0) {
        fprintf(stderr, "cnumkit prediction failed: %s\n", cnk_get_last_error_message());
        return 1;
    }
    /* A*[4, -1] = [7, 1], whose norm is sqrt(50). */
    if (output[0] != 7.0 || output[1] != 1.0
        || fabs(length - sqrt(50.0)) > 1e-12 || prediction != 0.0) {
        fprintf(stderr, "cnumkit consumer returned unexpected values\n");
        return 1;
    }
    printf("External consumer: [%.1f, %.1f], norm %.6f\n",
        output[0], output[1], length);
    return 0;
}
