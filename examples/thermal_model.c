#include "thermal_model.h"
#include "cnumkit.h"

#include <math.h>
#include <stdio.h>

static int validate_problem(const thermal_problem *problem) {
    for (size_t node = 0; node < 2; ++node) {
        if (!isfinite(problem->boundary_temperatures[node])
            || !isfinite(problem->heat_sources[node])) {
            cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Thermal temperatures and sources must be finite");
            return -1;
        }
    }
    for (size_t edge = 0; edge < 3; ++edge) {
        if (!isfinite(problem->conductances[edge]) || problem->conductances[edge] <= 0.0) {
            cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Thermal conductances must be finite and positive");
            return -1;
        }
    }
    return 0;
}

static int assemble_system(const thermal_problem *problem, cnk_matrix *matrix, cnk_vector *rhs) {
    const double *g = problem->conductances;
    /* Incoming heat plus generation equals outgoing heat at each node. */
    const double coefficients[] = {g[0] + g[1], -g[1], -g[1], g[1] + g[2]};
    const double loads[] = {
        g[0] * problem->boundary_temperatures[0] + problem->heat_sources[0],
        g[2] * problem->boundary_temperatures[1] + problem->heat_sources[1]
    };
    for (size_t row = 0; row < 2; ++row) {
        for (size_t column = 0; column < 2; ++column) {
            if (cnk_matrix_set(matrix, row, column, coefficients[2 * row + column]) != 0) {
                return -1;
            }
        }
        if (cnk_vector_set(rhs, row, loads[row]) != 0) {
            return -1;
        }
    }
    return 0;
}

static int evaluate_residual(const cnk_matrix *matrix, const cnk_vector *rhs,
    const cnk_vector *temperatures, double *relative_residual) {
    cnk_f64_const_matrix_view matrix_view;
    cnk_f64_const_view temperature_view;
    double residual[2];
    double residual_norm, matrix_norm, temperature_norm, rhs_norm;
    if (cnk_matrix_as_const_view(matrix, &matrix_view) != CNK_SUCCESS
        || cnk_vector_as_const_view(temperatures, &temperature_view) != CNK_SUCCESS
        || cnk_f64_matvec(matrix_view, temperature_view,
            cnk_f64_view_make(residual, 2, 1)) != CNK_SUCCESS
        || cnk_f64_axpy(-1.0, cnk_f64_const_view_make(rhs->data, 2, 1),
            cnk_f64_view_make(residual, 2, 1)) != CNK_SUCCESS
        || cnk_f64_norm2(cnk_f64_const_view_make(residual, 2, 1), &residual_norm) != CNK_SUCCESS
        || cnk_f64_norm2(cnk_f64_const_view_make(matrix->data, 4, 1), &matrix_norm) != CNK_SUCCESS
        || cnk_vector_norm2(temperatures, &temperature_norm) != 0
        || cnk_vector_norm2(rhs, &rhs_norm) != 0) {
        return -1;
    }
    double scale = matrix_norm * temperature_norm + rhs_norm;
    if (!isfinite(scale)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Thermal residual normalization overflowed");
        return -1;
    }
    /* The all-zero temperature/load solution has an exactly zero residual. */
    *relative_residual = scale == 0.0 ? 0.0 : residual_norm / scale;
    return 0;
}

static int evaluate_heat_balance(const thermal_problem *problem, thermal_result *result) {
    const double *temperature = result->temperatures;
    double *flow = result->heat_flows;
    flow[0] = problem->conductances[0] * (problem->boundary_temperatures[0] - temperature[0]);
    flow[1] = problem->conductances[1] * (temperature[0] - temperature[1]);
    flow[2] = problem->conductances[2] * (temperature[1] - problem->boundary_temperatures[1]);
    double first_balance = flow[0] + problem->heat_sources[0] - flow[1];
    double second_balance = flow[1] + problem->heat_sources[1] - flow[2];
    result->global_imbalance = flow[2] - flow[0]
        - problem->heat_sources[0] - problem->heat_sources[1];
    for (size_t edge = 0; edge < 3; ++edge) {
        if (!isfinite(flow[edge])) {
            cnk_set_last_error(CNK_ERROR_MATH, "Thermal heat flow overflowed");
            return -1;
        }
    }
    if (!isfinite(first_balance) || !isfinite(second_balance) || !isfinite(result->global_imbalance)) {
        cnk_set_last_error(CNK_ERROR_MATH, "Thermal heat balance overflowed");
        return -1;
    }
    result->maximum_node_imbalance = fmax(fabs(first_balance), fabs(second_balance));
    return 0;
}

static int solve_system(const thermal_problem *problem, cnk_matrix *matrix,
    cnk_vector *rhs, thermal_result *result) {
    if (assemble_system(problem, matrix, rhs) != 0) {
        return -1;
    }
    cnk_vector *temperatures = cnk_linalg_solve_gaussian(matrix, rhs);
    if (!temperatures) {
        return -1;
    }
    int status = cnk_vector_try_get(temperatures, 0, &result->temperatures[0]);
    if (status == 0) {
        status = cnk_vector_try_get(temperatures, 1, &result->temperatures[1]);
    }
    if (status == 0) {
        status = evaluate_residual(matrix, rhs, temperatures, &result->relative_residual);
    }
    /* Deallocation clears error state, so preserve diagnostics before cleanup. */
    cnk_error_code error = cnk_get_last_error();
    char error_message[256];
    (void)snprintf(error_message, sizeof(error_message), "%s", cnk_get_last_error_message());
    cnk_vector_free(temperatures);
    if (status != 0) {
        cnk_set_last_error(error, error_message);
        return -1;
    }
    return evaluate_heat_balance(problem, result);
}

int thermal_solve(const thermal_problem *problem, thermal_result *result) {
    if (!problem || !result) {
        cnk_set_last_error(CNK_ERROR_INVALID_ARGUMENT, "Thermal problem and output are required");
        return -1;
    }
    if (validate_problem(problem) != 0) {
        return -1;
    }
    cnk_matrix *matrix = cnk_matrix_create(2, 2);
    if (!matrix) {
        return -1;
    }
    cnk_vector *rhs = cnk_vector_create(2);
    thermal_result candidate = {0};
    int status = rhs ? solve_system(problem, matrix, rhs, &candidate) : -1;
    /* Copy the borrowed message before freeing either owned input object. */
    cnk_error_code error = cnk_get_last_error();
    char error_message[256];
    (void)snprintf(error_message, sizeof(error_message), "%s", cnk_get_last_error_message());
    cnk_vector_free(rhs);
    cnk_matrix_free(matrix);
    if (status != 0) {
        cnk_set_last_error(error, error_message);
        return -1;
    }
    /* Publish a complete result only after the solve and diagnostics succeed. */
    *result = candidate;
    return 0;
}
