# cnumkit

`cnumkit` is a small C11 library for scientific computing. The current focus is a reviewer-ready scalar core: predictable APIs, explicit error handling, strong tests, and a clean path toward architecture-specific acceleration such as RISC-V Vector Extension support.

## Current Modules

- Vectors and dense row-major matrices.
- Basic linear algebra, including Gaussian elimination with partial pivoting.
- Scalar numerical optimization helpers.
- Simple 1D machine-learning utilities.

## Safety Model

Public functions validate runtime inputs and report failures through return values plus thread-local error state:

- Pointer or parameter errors return `NULL` or `-1` and set `CNK_ERROR_INVALID_ARGUMENT`.
- Dimension mismatches set `CNK_ERROR_DIMENSION_MISMATCH`.
- Singular systems set `CNK_ERROR_SINGULAR_MATRIX`.
- Allocation failures set `CNK_ERROR_ALLOCATION`.
- Non-finite numerical results set `CNK_ERROR_MATH`.

Runtime contract checks are available as an opt-in development aid with `CNUMKIT_ENABLE_CONTRACTS=ON`. They are not required for normal API safety checks.

## Build And Test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Useful reviewer configurations:

```bash
cmake -S . -B build-contracts -DCNUMKIT_ENABLE_CONTRACTS=ON
cmake -S . -B build-sanitize -DCNUMKIT_ENABLE_SANITIZERS=ON
cmake -S . -B build-static -DBUILD_SHARED_LIBS=OFF
```

## Minimal Example

```c
#include <stdio.h>
#include "cnumkit.h"

int main(void) {
    cnk_vector *a = cnk_vector_create(3);
    cnk_vector *b = cnk_vector_create(3);
    double result = 0.0;

    if (!a || !b) {
        fprintf(stderr, "%s\n", cnk_get_last_error_message());
        cnk_vector_free(a);
        cnk_vector_free(b);
        return 1;
    }

    cnk_vector_set(a, 0, 1.0);
    cnk_vector_set(a, 1, 2.0);
    cnk_vector_set(a, 2, 3.0);
    cnk_vector_set(b, 0, 4.0);
    cnk_vector_set(b, 1, 5.0);
    cnk_vector_set(b, 2, 6.0);

    if (cnk_vector_dot(a, b, &result) != 0) {
        fprintf(stderr, "%s\n", cnk_get_last_error_message());
    } else {
        printf("dot = %.6f\n", result);
    }

    cnk_vector_free(a);
    cnk_vector_free(b);
    return 0;
}
```

## RISC-V Direction

RISC-V support starts with cross-compilation and smoke tests against the scalar implementation. RVV acceleration should be added later as an optional backend with the scalar C implementation kept as the reference fallback.

## Documentation

- `docs/USER_GUIDE.md` explains normal usage and safety behavior.
- `docs/LIBRARY_ARCHITECTURE.md` explains how the library is organized internally.
- `docs/PROJECT_STATUS.md` summarizes the current implementation state.
- `docs/REVIEWER_ROADMAP.md` tracks the remaining reviewer-readiness work.
- `docs/RISCV.md` explains the first RISC-V cross-build path.
