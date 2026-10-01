# Reviewer-Readiness Roadmap

This roadmap orders the remaining work by reviewer value. The goal is not to maximize the number of algorithms; it is to make every implemented capability understandable, reproducible, and supported by evidence.

## Current Priority

The 2026-09-29 borrowed-kernel increment adds scalar copy, robust norm, matrix
products, owned-object adapters, and direct C++ header verification. Strict
Debug/shared and Release/static configurations pass six CTest cases, and local
Linux ASan/UBSan execution passes all five C suites. Installed-package consumer
verification and version policy are the next foundation tasks. See
`PROJECT_STATUS.md` and `BORROWED_KERNELS.md` for current evidence and limits.

The 2026-09-30 scenario increment adds a sixth C suite, two runnable use cases,
and analytic checks for optimization and ML. All six C suites pass the direct
Linux ASan/UBSan build. On 2026-10-01, clean Debug/shared and Release/static
CMake builds each passed seven CTest cases, including C++ header verification.
Both installed packages built and ran the separate `tests/consumer` project.
The next packaging check is Windows consumption and remote CI evidence.

The `v0.1.0` candidate now includes MIT licensing, checked scalar access and
prediction, a changelog, and a release checklist. Local GCC CMake, sanitizer,
external-consumer, and warning-clean Doxygen checks pass. Remote CI and Windows
package consumption still require evidence before tagging.

## Previous Reviewer Baseline

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

Status-returning alternatives for scalar getters and prediction are now
available while the original value-returning calls remain for compatibility.
Public owned-object structures remain visible for the `0.1.x` educational API.

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
- Doxygen generates locally with warnings treated as errors; the CI check is
  configured and awaits its first remote run.
- Keep public claims tied to executed evidence.

Completion criterion: a new contributor can navigate the project from the README, and Doxygen generates without critical warnings.

## Phase 5: Packaging And External Consumption

Status: **advanced partial**.

- Completed on Linux: generate and install `cnumkit-config.cmake` and a version
  file, verify static and shared installations, and run a separate consumer using
  `find_package(cnumkit 0.1 CONFIG REQUIRED)` and `cnumkit::cnumkit`.
- Confirm generated export headers and shared-library symbol visibility on Windows.
- The changelog and `docs/RELEASE_CHECKLIST.md` define minimum tag criteria;
  remote CI and Windows consumption remain open.

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
