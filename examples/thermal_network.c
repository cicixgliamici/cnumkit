#include "cnumkit.h"
#include "thermal_model.h"

#include <stdio.h>

static int run_case(const char *name, const thermal_problem *problem) {
    thermal_result result;
    if (thermal_solve(problem, &result) != 0) {
        fprintf(stderr, "%s failed: %s\n", name, cnk_get_last_error_message());
        return -1;
    }
    printf("%s\n", name);
    printf("  Interior temperatures (deg C): [%.6f, %.6f]\n",
        result.temperatures[0], result.temperatures[1]);
    printf("  Heat flows left to right (W): [%.6f, %.6f, %.6f]\n",
        result.heat_flows[0], result.heat_flows[1], result.heat_flows[2]);
    printf("  Relative residual: %.3e\n", result.relative_residual);
    printf("  Maximum node imbalance (W): %.3e\n", result.maximum_node_imbalance);
    printf("  Global imbalance (W): %.3e\n", result.global_imbalance);
    return 0;
}

int main(void) {
    const thermal_problem reference = {{30.0, 0.0}, {1.0, 1.0, 1.0}, {0.0, 0.0}};
    const thermal_problem heated = {{30.0, 0.0}, {2.0, 1.0, 3.0}, {4.0, 2.0}};
    thermal_problem warmer_boundary = heated;
    /* Change one physical input to expose the model's boundary sensitivity. */
    warmer_boundary.boundary_temperatures[0] = 40.0;
    if (run_case("Uniform reference", &reference) != 0
        || run_case("Unequal conductances with internal heating", &heated) != 0
        || run_case("Left boundary raised by 10 K", &warmer_boundary) != 0) {
        return 1;
    }
    return 0;
}
