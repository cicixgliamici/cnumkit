#include "cnumkit.h"

#include <stdio.h>

int main(void) {
    /*
     * Simulate a device that stores three sensor readings at every other slot.
     * The 99 values represent metadata and must not enter the calculation.
     * The logical readings are [2, -1, 2].
     *
     * First apply an additive baseline correction:
     * [2, -1, 2] + [-1, 0, 1] = [1, -1, 3].
     * Then apply the 2x3 calibration matrix:
     * [1, 1, 0] dot [1, -1, 3] = 0;
     * [0, 1, -1] dot [1, -1, 3] = -4.
     * The Euclidean magnitude is sqrt(0^2 + (-4)^2) = 4.
     * These small values make each stage easy to check by hand.
     */
    const double samples[] = {2.0, 99.0, -1.0, 99.0, 2.0};
    const double baseline[] = {-1.0, 0.0, 1.0};
    const double calibration[] = {1.0, 1.0, 0.0, 0.0, 1.0, -1.0};
    double corrected[3];
    double output[2];
    double magnitude = 0.0;

    /* A stride of two selects readings without copying the metadata. */
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
