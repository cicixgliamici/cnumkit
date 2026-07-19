# Reviewer-Readiness Roadmap

This roadmap orders the remaining work by reviewer value. The goal is not to maximize the number of algorithms; it is to make every implemented capability understandable, reproducible, and supported by evidence.

## Current Priority

The 2026-07-19 MinGW baseline passes all 155 assertions with strict compiler warnings. The current API policy and first numerical-hardening pass are complete. The next priorities are documentation generation, remote CI evidence, and packaging from a clean consumer project.

## Phase 1: Reproducible Build And Test

Status: **advanced partial**.

Completed locally:

- Strict Debug shared and Release static CMake builds.
- CTest registration and successful execution of all three suites.
- A contracts-enabled build.
- A GitHub Actions matrix covering GCC, Clang, MSVC, static/shared linkage, contracts, and sanitizers.

Remaining:

- Run and confirm the GitHub Actions matrix remotely.
- Add coverage reporting as supporting evidence, not as a substitute for meaningful numerical cases.
- Add dedicated death tests before running negative tests with fail-fast contracts enabled.

Completion criterion: all supported CI configurations pass from a clean checkout and the status document records the result.

## Phase 2: Public API And Error Policy

Status: **complete for the current API**.

Completed:

- Ownership, valid inputs, return values, and failure behavior are documented.
- Null pointers, zero sizes, dimension mismatches, NaN/Inf, and output pointers have consistent runtime handling.
- Successful operations, error inspection, and unchanged-on-failure outputs follow one documented policy.
- Doxygen comments, implementation behavior, and regression tests are aligned.

Deferred pre-1.0 decision: evaluate status-returning alternatives for scalar getters and prediction, whose `0.0` failure value is inherently ambiguous.

## Phase 3: Numerical And Negative Testing

Status: **complete for the current scalar core**.

Completed:

- Combined absolute and relative test tolerances.
- A scale-relative Gaussian pivot threshold.
- Normalized residual checks and analytic reference solutions.
- Tests for pivoting, 1x1 systems, near-singular systems, extreme uniform scales, non-finite data, divergence, and overflow.
- Documentation of stability assumptions and intentional limits.

Deferred work: condition-number-aware cases and comparison with an established reference implementation should accompany reusable LU factorization.

## Phase 4: Documentation And Doxygen

Status: **in progress**.

- Keep the README short and oriented toward scope, build, tests, and first use.
- Maintain separate user, architecture, numerical, development, status, roadmap, and RISC-V documents.
- Generate Doxygen and resolve warnings for all public API declarations.
- Add an automated documentation check after local generation is clean.
- Keep public claims tied to executed evidence.

Completion criterion: a new contributor can navigate the project from the README, and Doxygen generates without critical warnings.

## Phase 5: Packaging And External Consumption

Status: **partial**.

- Generate and install `cnumkit-config.cmake` and a version file.
- Verify static and shared installation layouts.
- Build a separate consumer using only `find_package(cnumkit REQUIRED)` and `cnumkit::cnumkit`.
- Confirm generated export headers and Windows symbol visibility.
- Define the public changelog and minimum criteria for a `v0.1.0` tag.

Completion criterion: a clean external project can find, include, link, and run against an installed `cnumkit` package.

## Phase 6: Scalar API Expansion

Status: **pending**.

- Add foundational vector operations: copy, fill, add, subtract, scale, distance, and normalization.
- Add foundational matrix operations: copy, add, subtract, transpose, matrix-vector multiplication, trace, and norms.
- Introduce forward/back substitution and LU factorization with pivoting.
- Add determinant and multi-right-hand-side solving only when they can reuse LU cleanly.
- Add statistics and analytic linear regression only after the numerical core remains coherent.

Completion criterion: each addition has documented ownership, failure behavior, analytic reference tests, and explicit numerical limits.

## Phase 7: Scalar Performance Baseline

Status: **pending**.

- Benchmark dot product, norm, matrix-vector multiplication, matrix multiplication, Gaussian solve, and future LU solve.
- Define repeatable small, medium, and large inputs.
- Record compiler, flags, hardware, warm-up, repetitions, and reported statistic.
- Profile before changing data layout or algorithm structure.

Completion criterion: optimization decisions can cite repeatable scalar measurements.

## Phase 8: RISC-V Scalar Readiness

Status: **partial**.

- Verify `riscv64-linux-gnu-gcc` and `qemu-riscv64`.
- Cross-build with `cmake/toolchains/riscv64-linux-gnu.cmake`.
- Execute the scalar tests under QEMU.
- Record compiler, architecture, ABI, emulator, and runtime details.

Completion criterion: the scalar RISC-V build and smoke tests pass and are reproducible from `docs/RISCV.md`.

## Phase 9: Optional RVV Backend

Status: **pending**.

- Start with one measured kernel, preferably dot product.
- Select RVV through explicit feature detection or a CMake option.
- Keep scalar C as the correctness reference and fallback.
- Add scalar/RVV parity tests using documented tolerances.
- Benchmark before enabling any optimized path by default.

Completion criterion: the RVV path is optional, measurably useful, and behaviorally equivalent to the scalar reference within declared tolerances.
