# How cnumkit Works

This document explains the internal shape of `cnumkit`: how the library is organized, how data moves through it, how failures are reported, and where future optimized backends such as RISC-V RVV fit.

## Public Surface

Users normally include:

```c
#include "cnumkit.h"
```

That umbrella header exposes the public modules:

- `vector.h` for dynamic dense vectors.
- `matrix.h` for dynamic dense matrices.
- `linalg.h` for linear algebra solvers.
- `optim.h` for scalar numerical optimization.
- `ml.h` for small machine-learning utilities.
- `error.h` for thread-local error reporting.
- `contracts.h` for optional development-time contract checks.

The implementation lives in `src/`, one C file per module. Public types and functions are declared in `include/cnumkit/`.

## Data Ownership

The library owns the memory for objects created by `cnk_vector_create` and `cnk_matrix_create`.

Vectors store:

```c
typedef struct {
    size_t size;
    double *data;
} cnk_vector;
```

Matrices store:

```c
typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} cnk_matrix;
```

Matrix data is row-major, so element `(row, col)` is stored at `row * cols + col`. This layout is simple, cache-friendly, easy to test, and suitable as a scalar reference for future SIMD or RVV implementations.

Every successful create call must be paired with the matching free call:

- `cnk_vector_free`
- `cnk_matrix_free`

Passing `NULL` to a free function is valid. Like every successful public operation, it clears the thread-local error state.

## Error Model

Most public functions report failure in two ways:

- They return `NULL`, `-1`, or a documented neutral value.
- They set a thread-local error code and message.

Callers can inspect the last error with:

```c
cnk_error_code code = cnk_get_last_error();
const char *message = cnk_get_last_error_message();
```

The main error categories are:

- `CNK_ERROR_INVALID_ARGUMENT`: null pointers, zero sizes, invalid parameters, invalid model state, or dimensions that overflow allocation sizes.
- `CNK_ERROR_DIMENSION_MISMATCH`: incompatible vector or matrix dimensions.
- `CNK_ERROR_SINGULAR_MATRIX`: singular or nearly singular linear systems.
- `CNK_ERROR_ALLOCATION`: failed memory allocation.
- `CNK_ERROR_MATH`: rejected non-finite stored values, numerical divergence, or non-finite computed results.

Successful public operations clear the error state. Error-inspection functions leave it unchanged, and caller-provided outputs remain unchanged on failure. This provides recoverable runtime behavior while keeping the educational API small.

Custom error messages are copied into a bounded thread-local buffer. The string returned by `cnk_get_last_error_message` is borrowed and remains valid until the next error-state update in the same thread.

## Contracts

`contracts.h` defines:

```c
CNK_REQUIRES(condition)
CNK_ENSURES(condition)
```

These checks are compiled in only when `CNUMKIT_ENABLE_CONTRACTS` is defined. They are meant for development and debugging. Public functions still perform runtime validation even when contracts are disabled.

This split matters:

- API validation protects library users.
- Contracts catch programmer mistakes early during development.
- Release builds can avoid contract overhead without losing normal error handling.

## Module Flow

Vector operations are the lowest-level numeric building blocks. Matrix operations build on direct row-major indexing. Linear algebra routines use vectors and matrices as inputs and allocate result objects for the caller.

For example, Gaussian solve works like this:

1. Validate that `A`, `b`, and their data buffers are not null.
2. Validate that `A` is square and that `b->size == A->rows`.
3. Allocate an augmented matrix `[A | b]`.
4. Run forward elimination with partial pivoting.
5. Run back substitution into a newly allocated result vector.
6. Return the solution vector or report a specific error.

Optimization and ML routines are currently scalar and deterministic. The linear regression helper uses gradient descent over 1D input data and writes learned parameters into a caller-provided model struct.

## Build Model

CMake builds the library target `cnumkit` and exposes the alias `cnumkit::cnumkit`.

Useful options:

```bash
-DBUILD_SHARED_LIBS=ON
-DCNUMKIT_BUILD_TESTS=ON
-DCNUMKIT_BUILD_EXAMPLES=ON
-DCNUMKIT_ENABLE_CONTRACTS=ON
-DCNUMKIT_ENABLE_SANITIZERS=ON
-DCNUMKIT_WARNINGS_AS_ERRORS=ON
```

Tests are registered through CTest when `BUILD_TESTING` and `CNUMKIT_BUILD_TESTS` are enabled.

## RISC-V Path

RISC-V support should begin with the scalar implementation:

1. Cross-compile with the RISC-V toolchain file.
2. Run smoke tests with QEMU.
3. Treat the scalar results as the correctness reference.
4. Add RVV acceleration later behind explicit feature detection or a CMake option.

The important rule is that optimized backends must not change the public API or silently change numerical behavior outside documented tolerances.

## Reviewer Checklist

Before review, the library should be able to show:

- Warning-clean builds.
- Passing CTest tests.
- Negative tests for invalid API usage.
- Documented ownership and error behavior.
- Scalar correctness before optimized backend work.
- RISC-V cross-build instructions, even before RVV acceleration exists.

For the complete maintenance workflow and project invariants, see `docs/DEVELOPMENT_GUIDE.md`. For executed versus planned evidence, see `docs/PROJECT_STATUS.md`.
