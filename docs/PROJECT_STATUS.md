# Project Status

This document is the current, evidence-based snapshot of `cnumkit`. Historical work and previous test counts remain recorded in `local/LOCAL_CHANGELOG.md`.

## Project Goal

`cnumkit` is an educational C11 scientific-computing library. Its purpose is to demonstrate readable API design, explicit memory ownership, recoverable error handling, numerical reasoning, testing, and portable builds. It is not intended to replace production libraries such as BLAS or LAPACK.

The development strategy is deliberately scalar-first: establish a small, trustworthy reference implementation before adding more algorithms, packaging, benchmarks, or architecture-specific acceleration.

## Implemented Modules

- Dynamic dense vectors with creation, indexed access, mutation, dot product, and L2 norm.
- Dynamic dense row-major matrices with creation, indexed access, identity construction, and multiplication.
- Gaussian elimination with partial pivoting and a scale-relative pivot threshold.
- Central-difference numerical derivative and one-dimensional gradient descent.
- Educational one-dimensional linear regression and mean squared error.
- Thread-local error codes and bounded copied error messages.
- Optional fail-fast runtime contracts.

## Established Guarantees

- Public operations validate runtime inputs even when contracts are disabled.
- Vector and matrix allocation checks reject zero sizes and `size_t` overflow.
- Successful public operations clear the thread-local error state.
- Error inspection does not modify the current error state.
- Caller-provided outputs and models remain unchanged on failure.
- Setters and numerical algorithms reject non-finite inputs or results.
- Created vectors and matrices are owned by the caller and released by their matching free functions.
- Scalar getters return `0.0` on failure; callers must inspect the error state when zero is ambiguous.

## Verified Baseline

The latest local verification was performed on 2026-07-19 with MinGW GCC 15.1.0, CMake, and Ninja:

- Strict Debug shared build: passed with warnings treated as errors.
- Strict Release static build: passed with warnings treated as errors.
- Contracts-enabled Debug static build: compiled successfully.
- CTest: 3 of 3 suites passed in both tested runtime configurations.
- `test_basic`: 91 assertions passed.
- `test_optim`: 25 assertions passed.
- `test_ml`: 39 assertions passed.
- Total: 155 assertions passed, 0 failed.
- `git diff --check`: no whitespace errors; only expected Windows line-ending notices.

The numerical tests include analytic reference systems, normalized residuals, mandatory pivoting, 1x1 systems, uniform scales from `1e-150` to `1e150`, near-singular inputs, NaN/Inf rejection, optimization divergence, and ML overflow paths.

## Maturity Assessment

The repository is a solid educational pre-release. Its strongest aspects are its small modules, readable scalar algorithms, explicit ownership, allocation hardening, documented error policy, deterministic negative tests, and honest numerical limits.

It is not production-ready. The following evidence is still missing:

- A completed remote CI run across GCC, Clang, and MSVC.
- Executed sanitizer and coverage reports.
- Warning-free generated Doxygen output.
- A complete installed CMake package usable through `find_package`.
- Verification from a clean external consumer project.
- RISC-V cross-build and QEMU test results.
- Scalar performance baselines.
- Condition-number-aware validation against an established numerical reference.

## Pre-1.0 Design Decisions Still Open

- Whether scalar getters and prediction should move to status-returning output-pointer APIs.
- Whether vector and matrix structures should become opaque.
- Whether the next linear-algebra milestone should introduce reusable LU factorization.
- How optional optimized backends should be selected without changing the public API.

See `docs/REVIEWER_ROADMAP.md` for the ordered development plan and `docs/DEVELOPMENT_GUIDE.md` for the maintainer workflow.
