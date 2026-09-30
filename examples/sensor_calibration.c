#include "cnumkit.h"

#include <stdio.h>

int main(void) {
    /* Samples from three channels are interleaved with unrelated metadata. */
    const double samples[] = {2.0, 99.0, -1.0, 99.0, 2.0};
    const double baseline[] = {-1.0, 0.0, 1.0};
    const double calibration[] = {1.0, 1.0, 0.0, 0.0, 1.0, -1.0};
    double corrected[3];
    double output[2];
    double magnitude = 0.0;

    cnk_error_code status = cnk_f64_copy(
        cnk_f64_const_view_make(samples, 3, 2),
        cnk_f64_view_make(corrected, 3, 1));
    if (status == CNK_SUCCESS) {
        status = cnk_f64_axpy(1.0,
            cnk_f64_const_view_make(baseline, 3, 1),
            cnk_f64_view_make(corrected, 3, 1));
    }
    if (status == CNK_SUCCESS) {
        status = cnk_f64_matvec(
            cnk_f64_const_matrix_view_make(calibration, 2, 3, 3, 1),
            cnk_f64_const_view_make(corrected, 3, 1),
            cnk_f64_view_make(output, 2, 1));
    }
    if (status == CNK_SUCCESS) {
        status = cnk_f64_norm2(cnk_f64_const_view_make(output, 2, 1), &magnitude);
    }
    if (status != CNK_SUCCESS) {
        fprintf(stderr, "Sensor calibration failed: %s\n", cnk_error_string(status));
        return 1;
    }

    printf("Corrected channels: [%.1f, %.1f, %.1f]\n",
        corrected[0], corrected[1], corrected[2]);
    printf("Calibrated output: [%.1f, %.1f], magnitude %.1f\n",
        output[0], output[1], magnitude);
    return 0;
}
