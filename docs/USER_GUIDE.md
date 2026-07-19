# cnumkit User Guide

`cnumkit` is a compact educational C11 scientific-computing library. It prioritizes explicit ownership, recoverable failures, deterministic scalar behavior, and readable algorithms.

This guide describes behavior visible to callers. Internal design is documented in `docs/LIBRARY_ARCHITECTURE.md`; floating-point decisions are documented in `docs/NUMERICAL_NOTES.md`.

## Include And Link

Include the umbrella header:

```c
#include "cnumkit.h"
```

An in-tree CMake target links with:

```cmake
target_link_libraries(my_program PRIVATE cnumkit::cnumkit)
```

The installed `find_package` workflow is planned but not yet complete. See `docs/PROJECT_STATUS.md` before depending on installation behavior.

## Data Ownership

Vectors and matrices own contiguous `double` buffers allocated by the library:

- `cnk_vector_create(size)` creates `size` zero-initialized values.
- `cnk_matrix_create(rows, cols)` creates a zero-initialized row-major matrix.
- Creation rejects zero dimensions and allocation-size overflow.
- The caller owns successful results and must call the matching free function.
- `cnk_vector_free(NULL)` and `cnk_matrix_free(NULL)` are valid successful operations.

The structures are currently public for educational clarity. Direct field mutation can violate library invariants and should be avoided in normal user code.

## Error Policy

Public functions validate runtime inputs. On failure they return `NULL`, `-1`, or a documented neutral scalar and set a thread-local error code and message.

The policy is consistent across modules:

- Successful allocation, computation, access, mutation, printing, and deallocation set `CNK_SUCCESS`.
- Error-inspection functions do not change the current state.
- Caller-provided outputs and models remain unchanged on failure.
- Vector and matrix setters reject NaN and infinity with `CNK_ERROR_MATH`.
- Numerical inputs, intermediate values, and results must be finite unless documented otherwise.
- Error messages are borrowed, read-only, and valid until the next error update in the same thread.

Inspect a failure immediately, before another public operation changes the state:

```c
double dot = 0.0;
if (cnk_vector_dot(a, b, &dot) != 0) {
    fprintf(stderr, "dot product failed: %s\n", cnk_get_last_error_message());
}
```

Error categories:

- `CNK_ERROR_INVALID_ARGUMENT`: null pointers, zero dimensions, invalid parameters, or invalid model state.
- `CNK_ERROR_DIMENSION_MISMATCH`: incompatible vectors, matrices, or linear systems.
- `CNK_ERROR_SINGULAR_MATRIX`: singular or numerically near-singular systems.
- `CNK_ERROR_ALLOCATION`: failed allocation.
- `CNK_ERROR_MATH`: non-finite stored values, divergence, overflow, or non-finite results.

`cnk_vector_get`, `cnk_matrix_get`, and `cnk_ml_linear_regression_predict` return `0.0` on failure. Because zero is also valid, inspect `cnk_get_last_error()` immediately when the distinction matters.

## Vector And Matrix Use

Always check construction and mutation before consuming an object:

```c
cnk_vector *vector = cnk_vector_create(2);
if (!vector) {
    fprintf(stderr, "%s\n", cnk_get_last_error_message());
    return 1;
}

if (cnk_vector_set(vector, 0, 3.0) != 0 ||
    cnk_vector_set(vector, 1, 4.0) != 0) {
    fprintf(stderr, "%s\n", cnk_get_last_error_message());
    cnk_vector_free(vector);
    return 1;
}

double norm = 0.0;
if (cnk_vector_norm2(vector, &norm) == 0) {
    printf("norm = %.6f\n", norm);
}

cnk_vector_free(vector);
```

Matrix multiplication returns a new caller-owned matrix. It requires `left->cols == right->rows` and reports non-finite arithmetic as `CNK_ERROR_MATH`.

## Linear Systems

`cnk_linalg_solve_gaussian(A, b)` requires a finite square matrix and a finite right-hand-side vector with matching size. It returns a new caller-owned solution vector.

The solver uses partial pivoting and a scale-relative small-pivot test. `CNK_ERROR_SINGULAR_MATRIX` means that the implementation cannot safely continue; it is not a complete condition-number estimate. See `docs/NUMERICAL_NOTES.md` before interpreting difficult systems.

## Optimization And ML

The optimization and ML modules are intentionally small teaching examples:

- Numerical derivative uses a central difference and a caller-provided step.
- Gradient descent uses a fixed internal derivative step and fixed learning rate.
- Linear regression fits one input dimension with deterministic zero initialization.
- MSE validates datasets, model parameters, accumulation, and output finiteness.

These routines do not provide adaptive step selection, convergence reports, feature normalization, regularization, or production-scale numerical safeguards.

## Contracts

Runtime contracts are an opt-in development aid:

```bash
cmake -S . -B build-contracts \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCNUMKIT_ENABLE_CONTRACTS=ON
cmake --build build-contracts --parallel
```

Contracts terminate the process when a programmer violates a precondition. They do not replace recoverable runtime validation. Current negative tests run with contracts disabled; dedicated death tests are required to validate intentional termination.

## Testing

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

New algorithms should include an analytic reference result, relevant boundary conditions, documented tolerances, and failure-path tests. Maintainers should follow `docs/DEVELOPMENT_GUIDE.md`.
