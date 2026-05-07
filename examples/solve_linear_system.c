#include "cnumkit/linalg.h"
#include "cnumkit/matrix.h"
#include "cnumkit/vector.h"

#include <stdio.h>

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
        fprintf(stderr, "Allocation error.\n");
        cnk_matrix_free(A);
        cnk_vector_free(b);
        return 1;
    }

    cnk_matrix_set(A, 0, 0, 3.0);
    cnk_matrix_set(A, 0, 1, 2.0);
    cnk_matrix_set(A, 0, 2, -1.0);

    cnk_matrix_set(A, 1, 0, 2.0);
    cnk_matrix_set(A, 1, 1, -2.0);
    cnk_matrix_set(A, 1, 2, 4.0);

    cnk_matrix_set(A, 2, 0, -1.0);
    cnk_matrix_set(A, 2, 1, 0.5);
    cnk_matrix_set(A, 2, 2, -1.0);

    cnk_vector_set(b, 0, 1.0);
    cnk_vector_set(b, 1, -2.0);
    cnk_vector_set(b, 2, 0.0);

    printf("Matrix A:\n");
    cnk_matrix_print(A);

    printf("\nVector b:\n");
    cnk_vector_print(b);

    cnk_vector *x = cnk_linalg_solve_gaussian(A, b);

    if (!x) {
        fprintf(stderr, "Could not solve the system.\n");
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
