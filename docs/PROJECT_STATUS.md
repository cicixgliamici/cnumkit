# Project Status

This document is the current, evidence-based snapshot of `cnumkit`. Historical work and previous test counts remain recorded in `local/LOCAL_CHANGELOG.md`.

## Project Goal

`cnumkit` is an educational C11 scientific-computing library. Its purpose is to demonstrate readable API design, explicit memory ownership, recoverable error handling, numerical reasoning, testing, and portable builds. It is not intended to replace production libraries such as BLAS or LAPACK.

The development strategy is deliberately scalar-first: establish a small, trustworthy reference implementation before adding more algorithms, packaging, benchmarks, or architecture-specific acceleration.

## Implemented Modules

- Borrowed mutable/read-only strided `f64` views and checked owned-object adapters.
- Allocation-free scalar fill, copy, scale, AXPY, dot, scaled L2 norm, matrix-vector,
  and matrix-matrix kernels with direct status returns.
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
- Successful public operations clear the thread-local error state, except pure
  `_make` view constructors, which only assemble metadata and preserve that state.
- Error inspection does not modify the current error state.
- Caller-provided outputs and models remain unchanged on failure.
- Setters and numerical algorithms reject non-finite inputs or results.
- Created vectors and matrices are owned by the caller and released by their matching free functions.
- Scalar getters return `0.0` on failure; callers must inspect the error state when zero is ambiguous.

## Verified Baseline

On 2026-10-01, the `v0.1.0` candidate added status-returning scalar getters and
prediction, an MIT license, release notes, and Windows consumer and Doxygen CI
steps. Clean GCC 13.3 Debug/shared and Release/static CMake builds each passed
7 of 7 CTest cases, including the C++ header smoke test. All six C suites passed
ASan and UBSan with 3,142 assertions total. Both installed packages built and
ran the external consumer, which also calls the new prediction API. Doxygen
1.9.8 generated documentation with warnings treated as errors. Remote CI and
Windows package consumption are still pending.

On 2026-10-01, clean Ubuntu/WSL GCC 13.3 CMake builds passed with warnings as
errors in Debug/shared and Release/static configurations. Each passed 7 of 7
CTest cases, including the optional C++ header smoke test and the new scenario
suite. Both configurations installed successfully. A separate consumer project
used only `find_package(cnumkit 0.1 CONFIG REQUIRED)`, the installed headers,
and `cnumkit::cnumkit`; both executables printed `[7.0, 1.0]` and a norm of
`7.071068`. The new CI consumer steps are configured but have not been run
remotely. Windows package consumption remains unverified.

On 2026-09-30, the six C suites passed a direct strict GCC 13.3 build on
Ubuntu/WSL with ASan and UBSan: 3,114 assertions passed, 0 failed. This includes
the new mathematical scenario suite and additional optimization and ML cases.
The new `sensor_calibration` and `thermal_network` examples also compiled with
strict warnings and produced their documented outputs.

The previous CMake verification was performed on 2026-09-29 with MinGW GCC/G++
15.1.0, CMake, and Ninja, plus Ubuntu/WSL GCC 13.3.0:

- Strict Debug shared build: passed with warnings treated as errors.
- Strict Release static build: passed with warnings treated as errors.
- Contracts-enabled Debug static build: compiled successfully.
- CTest: 6 of 6 cases passed in Debug/shared and Release/static, including the
  optional direct-header C++ linkage smoke test.
- Both kernel suites also passed with contracts enabled; legacy negative tests
  remain excluded from contracts execution.
- `test_basic`: 91 assertions passed.
- `test_optim`: 25 assertions passed.
- `test_ml`: 39 assertions passed.
- `test_kernels`: 85 assertions passed.
- `test_matrix_kernels`: 2,813 assertions passed, including an independent check
  of 1,521 matrix shape/stride combinations.
- Total: 3,053 C assertions passed, 0 failed, plus the C++ smoke case.
- Linux ASan/leak detection and UBSan: all five C suites passed using
  `bash tests/run_sanitizers.sh` (direct GCC build; no Linux CMake installation).
- `git diff --check`: no whitespace errors; only expected Windows line-ending notices.

The numerical tests include analytic reference systems, normalized residuals, mandatory pivoting, 1x1 systems, uniform scales from `1e-150` to `1e150`, near-singular inputs, NaN/Inf rejection, optimization divergence, and ML overflow paths.

## Maturity Assessment

The repository is a solid educational pre-release. Its strongest aspects are its small modules, readable scalar algorithms, explicit ownership, allocation hardening, documented error policy, deterministic negative tests, and honest numerical limits.

It is not production-ready. The following evidence is still missing:

- A completed remote CI run across GCC, Clang, and MSVC.
- Coverage reports and sanitizer coverage on additional compilers/platforms.
- Installed-package verification on Windows, including shared-library symbol visibility.
- RISC-V cross-build and QEMU test results.
- Scalar performance baselines.
- Condition-number-aware validation against an established numerical reference.

## Pre-1.0 Design Decisions Still Open

- Whether vector and matrix structures should become opaque in a later minor version.
- The exact reusable LU factorization interface and workspace/allocator policy.
- How optional optimized backends should be selected without changing the public API.

See `docs/REVIEWER_ROADMAP.md` for the ordered development plan and `docs/DEVELOPMENT_GUIDE.md` for the maintainer workflow.
