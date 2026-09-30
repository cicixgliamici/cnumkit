#include "cnumkit.h"

#include <stdio.h>

int main(void) {
    /* Steady temperatures between boundaries at 30 and 0 degrees. */
    const double coefficients[] = {2.0, -1.0, -1.0, 2.0};
    const double boundary[] = {30.0, 0.0};
    cnk_matrix *system = cnk_matrix_create(2, 2);
    cnk_vector *rhs = cnk_vector_create(2);
    cnk_vector *temperatures = NULL;
    int result = 1;

    if (!system || !rhs) {
        fprintf(stderr, "Allocation failed: %s\n", cnk_get_last_error_message());
        goto cleanup;
    }
    for (size_t row = 0; row < 2; ++row) {
        for (size_t column = 0; column < 2; ++column) {
            if (cnk_matrix_set(system, row, column,
                coefficients[row * 2 + column]) != 0) {
                fprintf(stderr, "Matrix setup failed: %s\n", cnk_get_last_error_message());
                goto cleanup;
            }
        }
        if (cnk_vector_set(rhs, row, boundary[row]) != 0) {
            fprintf(stderr, "Boundary setup failed: %s\n", cnk_get_last_error_message());
            goto cleanup;
        }
    }

    temperatures = cnk_linalg_solve_gaussian(system, rhs);
    if (!temperatures) {
        fprintf(stderr, "Thermal solve failed: %s\n", cnk_get_last_error_message());
        goto cleanup;
    }
    printf("Interior temperatures: [%.1f, %.1f]\n",
        cnk_vector_get(temperatures, 0), cnk_vector_get(temperatures, 1));
    result = 0;

cleanup:
    cnk_vector_free(temperatures);
    cnk_vector_free(rhs);
    cnk_matrix_free(system);
    return result;
}
