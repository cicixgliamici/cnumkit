#include "cnumkit.h"

#include <stdio.h>

static int populate_system(cnk_matrix *matrix, cnk_vector *rhs) {
    const double coefficients[3][3] = {
        {3.0, 2.0, -1.0},
        {2.0, -2.0, 4.0},
        {-1.0, 0.5, -1.0}
    };
    const double values[3] = {1.0, -2.0, 0.0};

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < 3; col++) {
            if (cnk_matrix_set(matrix, row, col, coefficients[row][col]) != 0) {
                return -1;
            }
        }
        if (cnk_vector_set(rhs, row, values[row]) != 0) {
            return -1;
        }
    }

    return 0;
}

int main(void) {
    /*
     * Example system:
     *
     * 3x + 2y -  z = 1
     * 2x - 2y + 4z = -2
     * -x + 0.5y - z = 0
     *
     * Expected solution:
     * x = 1
     * y = -2
     * z = -2
     */

    cnk_matrix *A = cnk_matrix_create(3, 3);
    cnk_vector *b = cnk_vector_create(3);

    if (!A || !b) {
        fprintf(stderr, "Could not allocate the system: %s\n", cnk_get_last_error_message());
        cnk_matrix_free(A);
        cnk_vector_free(b);
        return 1;
    }

    if (populate_system(A, b) != 0) {
        fprintf(stderr, "Could not initialize the system: %s\n", cnk_get_last_error_message());
        cnk_matrix_free(A);
        cnk_vector_free(b);
        return 1;
    }

    printf("Matrix A:\n");
    cnk_matrix_print(A);

    printf("\nVector b:\n");
    cnk_vector_print(b);

    cnk_vector *x = cnk_linalg_solve_gaussian(A, b);

    if (!x) {
        fprintf(stderr, "Could not solve the system: %s\n", cnk_get_last_error_message());
        cnk_matrix_free(A);
        cnk_vector_free(b);
        return 1;
    }

    printf("\nSolution x:\n");
    cnk_vector_print(x);

    cnk_matrix_free(A);
    cnk_vector_free(b);
    cnk_vector_free(x);

    return 0;
}
