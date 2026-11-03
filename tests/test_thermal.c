#include "cnumkit.h"
#include "thermal_model.h"
#include "test_framework.h"

#include <float.h>
#include <string.h>

static void expect_solution(const thermal_problem *problem,
    const double expected_temperatures[2], const double expected_flows[3]) {
    thermal_result result;
    int status = thermal_solve(problem, &result);
    EXPECT_TRUE(status == 0);
    if (status != 0) {
        return;
    }
    EXPECT_TRUE(cnk_get_last_error() == CNK_SUCCESS);
    double heat_scale = 1.0;
    for (size_t node = 0; node < 2; ++node) {
        EXPECT_ALMOST_EQ(result.temperatures[node], expected_temperatures[node]);
    }
    for (size_t edge = 0; edge < 3; ++edge) {
        EXPECT_ALMOST_EQ(result.heat_flows[edge], expected_flows[edge]);
        heat_scale = fmax(heat_scale, fabs(expected_flows[edge]));
    }
    EXPECT_TRUE(result.relative_residual <= 1e-14);
    EXPECT_TRUE(result.maximum_node_imbalance <= 1e-12 * heat_scale);
    EXPECT_TRUE(fabs(result.global_imbalance) <= 1e-12 * heat_scale);
}

static void test_uniform_reference(void) {
    const thermal_problem problem = {{30.0, 0.0}, {1.0, 1.0, 1.0}, {0.0, 0.0}};
    /* Three equal series resistances divide the boundary drop equally. */
    const double temperatures[] = {20.0, 10.0};
    const double flows[] = {10.0, 10.0, 10.0};
    expect_solution(&problem, temperatures, flows);
}

static void test_unequal_series_resistances(void) {
    const thermal_problem problem = {{30.0, 0.0}, {2.0, 1.0, 3.0}, {0.0, 0.0}};
    /* Independent reference: q = 30 / (1/2 + 1 + 1/3) = 180/11 W. */
    const double temperatures[] = {240.0 / 11.0, 60.0 / 11.0};
    const double flows[] = {180.0 / 11.0, 180.0 / 11.0, 180.0 / 11.0};
    expect_solution(&problem, temperatures, flows);
}

static void test_internal_heating_and_boundary_sensitivity(void) {
    thermal_problem problem = {{30.0, 0.0}, {2.0, 1.0, 3.0}, {4.0, 2.0}};
    /* Hand elimination of 3*T1-T2=64 and -T1+4*T2=2 gives these fractions. */
    const double temperatures[] = {258.0 / 11.0, 70.0 / 11.0};
    const double flows[] = {144.0 / 11.0, 188.0 / 11.0, 210.0 / 11.0};
    expect_solution(&problem, temperatures, flows);

    /* Raising the left boundary by 10 K adds [80/11, 20/11] K internally. */
    problem.boundary_temperatures[0] = 40.0;
    const double warmer_temperatures[] = {338.0 / 11.0, 90.0 / 11.0};
    const double warmer_flows[] = {204.0 / 11.0, 248.0 / 11.0, 270.0 / 11.0};
    expect_solution(&problem, warmer_temperatures, warmer_flows);
}

static void test_reverse_and_equilibrium(void) {
    thermal_problem problem = {{0.0, 30.0}, {1.0, 1.0, 1.0}, {0.0, 0.0}};
    const double reversed_temperatures[] = {10.0, 20.0};
    const double reversed_flows[] = {-10.0, -10.0, -10.0};
    expect_solution(&problem, reversed_temperatures, reversed_flows);

    /* Equal nonzero boundaries check that Celsius offsets do not drive heat. */
    problem.boundary_temperatures[0] = 30.0;
    const double equilibrium_temperatures[] = {30.0, 30.0};
    const double zero_flows[] = {0.0, 0.0, 0.0};
    expect_solution(&problem, equilibrium_temperatures, zero_flows);
    problem.boundary_temperatures[0] = 0.0;
    problem.boundary_temperatures[1] = 0.0;
    const double zero_temperatures[] = {0.0, 0.0};
    expect_solution(&problem, zero_temperatures, zero_flows);
}

static void test_uniform_power_rescaling(void) {
    const double scales[] = {1e-6, 1e6};
    const double temperatures[] = {258.0 / 11.0, 70.0 / 11.0};
    for (size_t index = 0; index < 2; ++index) {
        double scale = scales[index];
        /* Scaling every conductance and source preserves temperatures, not W. */
        thermal_problem problem = {{30.0, 0.0},
            {2.0 * scale, scale, 3.0 * scale}, {4.0 * scale, 2.0 * scale}};
        const double flows[] = {144.0 / 11.0 * scale,
            188.0 / 11.0 * scale, 210.0 / 11.0 * scale};
        expect_solution(&problem, temperatures, flows);
    }
}

static void expect_failure(const thermal_problem *problem, cnk_error_code expected_error) {
    thermal_result result = {{99.0, 98.0}, {97.0, 96.0, 95.0}, 94.0, 93.0, 92.0};
    unsigned char original[sizeof(result)];
    /* Compare bytes copied before the call, including any structure padding. */
    memcpy(original, &result, sizeof(result));
    EXPECT_TRUE(thermal_solve(problem, &result) == -1);
    EXPECT_TRUE(cnk_get_last_error() == expected_error);
    EXPECT_TRUE(memcmp(original, &result, sizeof(result)) == 0);
}

static void test_invalid_physics_and_overflow(void) {
    thermal_problem problem = {{30.0, 0.0}, {1.0, 1.0, 1.0}, {0.0, 0.0}};
    expect_failure(NULL, CNK_ERROR_INVALID_ARGUMENT);
    EXPECT_TRUE(thermal_solve(&problem, NULL) == -1);
    EXPECT_TRUE(cnk_get_last_error() == CNK_ERROR_INVALID_ARGUMENT);
    problem.conductances[0] = 0.0;
    expect_failure(&problem, CNK_ERROR_INVALID_ARGUMENT);
    problem.conductances[0] = -1.0;
    expect_failure(&problem, CNK_ERROR_INVALID_ARGUMENT);
    problem.conductances[0] = INFINITY;
    expect_failure(&problem, CNK_ERROR_INVALID_ARGUMENT);
    problem.conductances[0] = 1.0;
    problem.heat_sources[0] = NAN;
    expect_failure(&problem, CNK_ERROR_INVALID_ARGUMENT);
    problem.heat_sources[0] = 0.0;
    problem.boundary_temperatures[0] = INFINITY;
    expect_failure(&problem, CNK_ERROR_INVALID_ARGUMENT);
    /* Valid finite inputs can still overflow the boundary load during assembly. */
    problem.boundary_temperatures[0] = DBL_MAX;
    problem.conductances[0] = 2.0;
    expect_failure(&problem, CNK_ERROR_MATH);

    /* Mathematical uniqueness does not override the solver's pivot safeguard. */
    const thermal_problem ill_conditioned = {{30.0, 0.0},
        {1e-20, 1.0, 1e-20}, {0.0, 0.0}};
    expect_failure(&ill_conditioned, CNK_ERROR_SINGULAR_MATRIX);
}

TEST_SUITE("Thermal Network Case Study")
    RUN_TEST(test_uniform_reference);
    RUN_TEST(test_unequal_series_resistances);
    RUN_TEST(test_internal_heating_and_boundary_sensitivity);
    RUN_TEST(test_reverse_and_equilibrium);
    RUN_TEST(test_uniform_power_rescaling);
    RUN_TEST(test_invalid_physics_and_overflow);
TEST_SUITE_END()
