// Include individual headers before the umbrella to verify their own C linkage.
#include "cnumkit/view.h"
#include "cnumkit/error.h"
#include "cnumkit/vector.h"
#include "cnumkit/matrix.h"
#include "cnumkit/kernels.h"
#include "cnumkit/adapters.h"
#include "cnumkit/linalg.h"
#include "cnumkit/optim.h"
#include "cnumkit/ml.h"
#include "cnumkit.h"

int main() {
    double values[] = {3.0, 4.0};
    cnk_vector owner = {2, values};
    cnk_f64_const_view view;
    double norm = 0.0;
    if (cnk_vector_as_const_view(&owner, &view) != CNK_SUCCESS
        || cnk_f64_norm2(view, &norm) != CNK_SUCCESS || norm != 5.0) {
        return 1;
    }
    cnk_matrix *identity = cnk_matrix_identity(2);
    if (!identity) {
        return 1;
    }
    cnk_f64_const_matrix_view matrix;
    double output[2];
    cnk_error_code status = cnk_matrix_as_const_view(identity, &matrix);
    if (status == CNK_SUCCESS) {
        status = cnk_f64_matvec(matrix, view, cnk_f64_view_make(output, 2, 1));
    }
    cnk_matrix_free(identity);
    cnk_linear_regression_model model = {2.0, 1.0};
    double prediction = 0.0;
    return status == CNK_SUCCESS && output[0] == 3.0 && output[1] == 4.0
        && cnk_ml_linear_regression_try_predict(&model, 3.0, &prediction) == 0
        && prediction == 7.0 ? 0 : 1;
}
