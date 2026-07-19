# cnumkit

`cnumkit` is a small educational C11 scientific-computing library. It demonstrates predictable APIs, explicit ownership and error handling, numerical reasoning, strict tests, and portable build practices. It is a scalar reference project, not a replacement for production libraries such as BLAS or LAPACK.

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
cmake -S . -B build-contracts -DCMAKE_BUILD_TYPE=Debug -DCNUMKIT_ENABLE_CONTRACTS=ON
cmake -S . -B build-sanitize -DCMAKE_BUILD_TYPE=Debug -DCNUMKIT_ENABLE_SANITIZERS=ON
cmake -S . -B build-static -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
cmake -S . -B build-strict -DCMAKE_BUILD_TYPE=Debug -DCNUMKIT_WARNINGS_AS_ERRORS=ON
```

Each build directory is independent. Build and test a selected configuration with:

```bash
cmake --build build-strict --config Debug --parallel
ctest --test-dir build-strict -C Debug --output-on-failure
```

GitHub Actions is configured to repeat strict Debug and Release builds with GCC, Clang, and MSVC. The matrix covers static and shared libraries, opt-in contracts, and a Clang sanitizer build.

Contract-enabled builds are compiled separately because contracts intentionally abort on violated preconditions, while the current negative tests verify recoverable runtime error handling in-process. Dedicated contract death tests are planned before contract-enabled CTest execution is enabled.

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

- `docs/USER_GUIDE.md`: normal use, ownership, and failure handling.
- `docs/LIBRARY_ARCHITECTURE.md`: internal modules and data flow.
- `docs/NUMERICAL_NOTES.md`: tolerances, stability checks, residuals, and limits.
- `docs/DEVELOPMENT_GUIDE.md`: reading order, invariants, workflow, and definition of done.
- `docs/PROJECT_STATUS.md`: verified baseline and maturity assessment.
- `docs/REVIEWER_ROADMAP.md`: ordered reviewer-readiness work.
- `docs/RISCV.md`: unverified scalar RISC-V cross-build path.
