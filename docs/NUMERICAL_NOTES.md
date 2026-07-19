# Numerical Notes

This document records the numerical assumptions behind the scalar reference implementation. The algorithms favor clarity and explicit failure over production-grade performance.

## Floating-Point Comparisons

Tests use a combined absolute and relative comparison:

```text
|actual - expected| <= absolute_tolerance + relative_tolerance * max(|actual|, |expected|)
```

The absolute term protects comparisons near zero. The relative term makes the same reference test meaningful across different numerical scales. Each suite may override both tolerances when its algorithm has different convergence accuracy.

## Gaussian Elimination

`cnk_linalg_solve_gaussian` uses partial pivoting. At each elimination step it selects the remaining entry with the largest absolute value in the active column.

A pivot is classified as numerically too small by comparing it with the largest absolute coefficient in the original matrix:

```text
|pivot| / matrix_scale <= 32 * DBL_EPSILON * matrix_size
```

Using a ratio makes the decision invariant under uniform rescaling of both `A` and `b`. The factor 32 is a conservative teaching-oriented margin for accumulated floating-point error; it is not an estimate of the matrix condition number.

The tests cover:

- an analytic 3x3 reference system;
- normalized residual verification;
- a 1x1 system;
- a system that requires a row swap;
- uniformly scaled systems at `1e-150` and `1e150`;
- singular and nearly singular systems;
- dimension mismatches and non-finite input data.

The normalized residual used by the tests is:

```text
||A*x - b|| / (||A|| * ||x|| + ||b||)
```

A small residual confirms that the computed solution satisfies the supplied system. It does not prove that the solution is close to the exact solution when the matrix is ill-conditioned.

## Optimization And Regression

The numerical derivative uses a central difference and rejects non-finite inputs, steps smaller than the documented internal floor, and non-finite function results. Gradient descent reports a math error when an intermediate iterate or derivative becomes non-finite.

Linear regression and MSE require finite datasets and model parameters. The implementation detects non-finite predictions, gradients, parameters, and accumulated errors. It intentionally remains a small educational example: it does not normalize features, adapt the learning rate, or provide a convergence report.

## Intentional Limits

- The Gaussian solver does not estimate a condition number.
- The solver does not use scaled partial pivoting, equilibration, iterative refinement, LU reuse, or higher precision.
- The fixed derivative step inside gradient descent is educational rather than adaptive.
- Passing tests at extreme uniform scales does not imply robustness for every badly scaled or ill-conditioned problem.
- For production scientific work, established BLAS/LAPACK implementations remain the appropriate reference.
