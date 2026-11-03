# Numerical Validation Report

**Baseline:** `780094927b7757bdb9885a2afa2324df231d9994`, verified locally on
2026-11-03 with Ubuntu/WSL, GCC/G++ 13.3.0, and CMake 3.28.3. This report
summarizes the executed thermal-case-study increment; adding this document does
not change the numerical implementation or tests.

## Executed Evidence

| Configuration | Result |
| --- | --- |
| Debug/shared, C11, `-g`, strict warnings as errors | 9/9 CTest cases passed |
| Release/static, C11, `-O3 -DNDEBUG`, strict warnings as errors | 9/9 CTest cases passed |
| Direct GCC build, `-O1 -g`, ASan/leak detection and UBSan | Seven C suites: 3,281 assertions passed, zero failed; no sanitizer diagnostics |

The nine CTest cases comprise seven C suites, the C++ header smoke test, and
the thermal executable. The C assertion counts are 108 basic, 55 optimization,
62 ML, 85 vector kernels, 2,813 matrix kernels/adapters, 42 composed scenarios,
and 116 thermal checks. These totals include input validation and ownership
checks; they are not counts of independent numerical problems or a coverage
percentage. Sanitizer success concerns executed memory/undefined-behavior paths,
not numerical accuracy. Contracts-enabled builds compiled separately; their
intentional negative cases were not executed.

## Numerical References And Acceptance Criteria

Comparisons normally accept
`abs(actual-expected) <= atol + rtol*max(abs(actual),abs(expected))`.
The test framework defaults to `atol=rtol=1e-9`. Optimization overrides both to
`1e-3`; ML overrides both to `0.15` for its educational iterative fit. Explicit
assertions use tighter bounds where stated below. Passing the ML fit tolerance
does not imply high-precision parameter estimation.

| Area and executable evidence | Reference and criterion |
| --- | --- |
| Gaussian solve: `test_basic` | Analytic systems, 1x1 and forced pivoting; normalized residual below `1e-12`, including uniform scales `1e-150` and `1e150`; singular and near-singular systems rejected |
| Vector kernels: `test_kernels` | Strided `[3,-4]` norms at scales `1e-300`, `1`, `1e300`, compared with `5*scale` and platform `hypot`; normalized relative differences below `1e-14` |
| Matrix/layout composition: `test_matrix_kernels`, `test_math_scenarios` | Hand-calculated products and transformed sensor values; 1,521 small shape/stride layouts compared with independently enumerated offsets; invalid layouts and late failures preserve outputs |
| Derivative and optimization: `test_optim` | Polynomial derivatives and known quadratic minima; regression for the positive slope `1/DBL_MAX` with `h=DBL_MAX`, overflow-safe identity slopes, subnormal differences, and rejected rounded-away steps |
| Regression: `test_ml` | Synthetic line `y=2*x+1`, known prediction/MSE values, invalid/non-finite data and overflow; suite-specific fit tolerance as disclosed above |
| Thermal model: `test_thermal` | Series resistances and hand-eliminated heated solutions; temperature/flow comparisons use `1e-9` absolute/relative tolerances, residual at most `1e-14`, heat imbalance at most `1e-12*max(1 W, largest expected heat-rate magnitude)` |

The linear-system residual uses the Frobenius matrix norm and vector L2 norms:

```text
eta = ||A*x-b||2 / (||A||F * ||x||2 + ||b||2)
```

The zero-load, zero-temperature thermal state is assigned `eta=0`. A small
residual establishes agreement with the assembled equations; it is not a
forward-error bound without conditioning information.

## Principal Scientific Case: Observed Diagnostics

For the three default cases in the Release/static `thermal_network` run:

| Case | Relative residual | Maximum node imbalance (W) | Signed global imbalance (W) |
| --- | --- | --- | --- |
| Uniform reference | `0.000e+00` | `0.000e+00` | `0.000e+00` |
| Unequal conductances, internal heating | `0.000e+00` | `3.553e-15` | `-3.553e-15` |
| Left boundary raised by 10 K | `5.877e-17` | `7.105e-15` | `7.105e-15` |

These are rounded observations from one toolchain, not required bit patterns
for other platforms. The heated solution is `[258/11, 70/11]` degrees Celsius;
its outgoing heat exceeds incoming heat by the prescribed `6 W`. Tests also
check reversed flow, equilibrium, boundary sensitivity, power rescaling, and
small-pivot rejection. See [the case study](THERMAL_CASE_STUDY.md) for the
physical derivation, exact reference values, and balance definitions.

## Reproduce

From the repository root, use the baseline commit above for a historical
comparison, or the current checkout to check subsequent changes:

```bash
cmake -S . -B build/validation-debug -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_SHARED_LIBS=ON -DCNUMKIT_WARNINGS_AS_ERRORS=ON \
  -DCNUMKIT_BUILD_CPP_SMOKE_TEST=ON
cmake --build build/validation-debug --parallel
ctest --test-dir build/validation-debug --output-on-failure
cmake -S . -B build/validation-release -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF -DCNUMKIT_WARNINGS_AS_ERRORS=ON \
  -DCNUMKIT_BUILD_CPP_SMOKE_TEST=ON
cmake --build build/validation-release --parallel
ctest --test-dir build/validation-release --output-on-failure
./build/validation-release/thermal_network
bash tests/run_sanitizers.sh
```

These commands target the recorded Linux environment. Record compiler versions,
configuration, source commit, and output when repeating them elsewhere.

## Limits And Pending Evidence

References are analytic or discrete invariants, with `hypot` as a limited
platform-library comparison. There is no BLAS/LAPACK comparison, arbitrary
precision oracle, systematic condition-number study, or continuum/experimental
validation. Fixed optimization steps and platform-dependent `long double`
accumulation remain limitations. Uniform scaling tests do not establish
robustness for all badly scaled inputs. Remote Clang/MSVC CI, RISC-V execution,
coverage measurement, and performance baselines remain unverified or pending.
See [numerical notes](NUMERICAL_NOTES.md) for algorithm assumptions and
[project status](PROJECT_STATUS.md) for the broader evidence record.
