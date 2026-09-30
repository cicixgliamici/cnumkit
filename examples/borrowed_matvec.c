#include "cnumkit/kernels.h"

#include <stdio.h>

int main(void) {
    /* Borrow engine-owned buffers, leaving the padding outside the logical matrix. */
    const double coefficients[] = {1, 2, 3, 0, 4, 5, 6, 0};
    const double state[] = {1, 2, 3};
    double derivative[2];
    cnk_f64_const_matrix_view matrix = cnk_f64_const_matrix_view_make(
        coefficients, 2, 3, 4, 1);
    cnk_error_code status = cnk_f64_matvec(matrix,
        cnk_f64_const_view_make(state, 3, 1),
        cnk_f64_view_make(derivative, 2, 1));
    if (status != CNK_SUCCESS) {
        fprintf(stderr, "%s\n", cnk_error_string(status));
        return 1;
    }
    printf("A * state = [%.1f, %.1f]\n", derivative[0], derivative[1]);
    return 0;
}
