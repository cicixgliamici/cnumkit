# cnumkit User Guide

`cnumkit` is currently a compact scalar C11 library for scientific computing. Its design priority is trust first: explicit ownership, explicit failure, deterministic tests, and a scalar implementation that can later serve as the reference for optimized backends.

For a deeper explanation of how the modules fit together internally, see `docs/LIBRARY_ARCHITECTURE.md`.

## Data Model

Vectors and matrices own contiguous `double` storage allocated by the library.

- `cnk_vector` stores `size` elements.
- `cnk_matrix` stores `rows * cols` elements in row-major order.
- Objects created with `cnk_vector_create` or `cnk_matrix_create` must be released with the matching free function.

## Error Handling

Public functions validate runtime inputs. On failure they return `NULL`, `-1`, or a documented neutral value, and update the thread-local error state.

Use:

```c
cnk_error_code code = cnk_get_last_error();
const char *message = cnk_get_last_error_message();
```

Expected error categories:

- `CNK_ERROR_INVALID_ARGUMENT` for null pointers, zero dimensions, invalid parameters, or overflowed dimensions.
- `CNK_ERROR_DIMENSION_MISMATCH` for incompatible vectors or matrices.
- `CNK_ERROR_SINGULAR_MATRIX` for singular or nearly singular linear systems.
- `CNK_ERROR_ALLOCATION` for allocation failures.
- `CNK_ERROR_MATH` for non-finite values or numerical divergence.

## Contracts

The headers include contract-style documentation and optional runtime checks. Runtime contract checks are enabled with:

```bash
cmake -S . -B build-contracts -DCNUMKIT_ENABLE_CONTRACTS=ON
```

Contracts are a development aid. They do not replace normal API validation.

## Testing

The CMake build registers tests with CTest:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The current tests cover core numeric results and negative API behavior. New algorithms should add both reference-result tests and failure-path tests.

## RISC-V

The first RISC-V target is scalar correctness under cross-compilation and QEMU smoke tests. See `docs/RISCV.md`. RVV acceleration should be optional and compared against scalar results before it is considered production-ready.
