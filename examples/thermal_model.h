#ifndef CNUMKIT_EXAMPLE_THERMAL_MODEL_H
#define CNUMKIT_EXAMPLE_THERMAL_MODEL_H

/* This fixed-topology model belongs to the case study, not the public API. */
typedef struct {
    double boundary_temperatures[2]; /* Celsius: left, right. */
    double conductances[3];          /* W/K: left, middle, right connection. */
    double heat_sources[2];          /* W injected at each interior node. */
} thermal_problem;

typedef struct {
    double temperatures[2];          /* Celsius. */
    double heat_flows[3];            /* W, positive from left to right. */
    double relative_residual;        /* Dimensionless, using Frobenius/L2 norms. */
    double maximum_node_imbalance;   /* W. */
    double global_imbalance;         /* W: outgoing - incoming - sources. */
} thermal_result;

/* Return 0 on success; report cnumkit errors and preserve output on failure. */
int thermal_solve(const thermal_problem *problem, thermal_result *result);

#endif /* CNUMKIT_EXAMPLE_THERMAL_MODEL_H */
