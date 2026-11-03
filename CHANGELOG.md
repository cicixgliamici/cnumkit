# Changelog

## Unreleased — candidate for v0.1.0

First public release candidate of the scalar C11 library.

### Added

- Owned dense vectors and row-major matrices, Gaussian linear solve, scalar
  numerical optimization, and educational one-dimensional linear regression.
- Borrowed strided `double` views, allocation-free scalar kernels, and adapters
  between owned objects and views.
- Status-returning scalar access and prediction functions that leave outputs
  unchanged on failure. Existing value-returning functions remain available.
- CMake package metadata for `find_package(cnumkit 0.1 CONFIG REQUIRED)`, with
  static and shared library installation and a separate consumer example.
- Numerical, error-path, C++ header, sanitizer, and simulated-use tests.
- Offline usage documentation in installed packages and a binary packaging
  guide covering archive selection, checksums, CMake consumption, and DLL setup.
- CI verification of consumers against extracted, relocated Linux and Windows
  archives, explicit C++ header checks, and release asset `SHA256SUMS` generation.

### Fixed

- C++ smoke coverage now compiles every public header in isolation and links
  direct calls to the solver, optimization, vector access, and error APIs.
- Central differences avoid spurious zero or math errors from overflowing
  finite sample subtraction or `2*h`. Invalid sample points and steps that
  round away are rejected without modifying caller outputs.

### Known limits

- The API and ABI are pre-1.0. Compatibility is promised only within the
  `0.1.x` line; any intentional API break requires a new minor version.
- Vectors and matrices expose their storage fields. Callers must preserve the
  documented size, layout, ownership, and finite-value invariants.
- The Gaussian solver uses a pivot threshold rather than a condition estimate.
  Optimization and regression routines have fixed algorithms and are intended
  for small educational examples.
- Windows installed-package consumption, remote CI results, RISC-V execution,
  and performance baselines remain to be verified before tagging this candidate.
