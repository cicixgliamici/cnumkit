# Contract Semantics And Death Tests

Public argument validation is recoverable in every build. Invalid sizes,
pointers, indices, or dimensions report the documented error code and return
value, with caller outputs unchanged where promised. Enabling contracts does
not turn these public errors into process termination. Caller-owned storage
must still satisfy the documented lifetime and layout rules; validation cannot
make arbitrary dangling pointers safe.

## Internal Checks

`CNK_REQUIRES` and `CNK_ENSURES` check implementation invariants. Preconditions
belong in private helpers reached after public validation; postconditions may
check a successfully constructed object. An internal violation indicates a
programming defect and is not translated into thread-local API error state.
These macros do not execute or prove the full ACSL specifications in headers.

| Setting in a translation unit | Condition evaluation | False condition |
| --- | --- | --- |
| Undefined or `CNUMKIT_ENABLE_CONTRACTS=0` | Never | No action |
| `CNUMKIT_ENABLE_CONTRACTS=1` | Once per check | Diagnostic to stderr, then `abort()` |

`NDEBUG` does not change this behavior. Explicitly enabled checks remain active
in Release as well as Debug. Conditions must be free of side effects: disabling
contracts must not change an algorithm's state or skip required work. The
diagnostic identifies the check kind, condition text, file, and source line.

The CMake option defaults to OFF. When ON, `cnumkit::cnumkit` carries the public
definition `CNUMKIT_ENABLE_CONTRACTS=1`, so consumers compiling checks from the
header inherit the selection. Checks are header-only private inline code;
there is no new exported ABI. Direct compiler builds can use
`-DCNUMKIT_ENABLE_CONTRACTS=1` or `=0` explicitly.

## Run The Tests

```bash
cmake -S . -B build/contracts -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF -DCNUMKIT_ENABLE_CONTRACTS=ON \
  -DCNUMKIT_WARNINGS_AS_ERRORS=ON -DCNUMKIT_BUILD_CPP_SMOKE_TEST=ON
cmake --build build/contracts --config Release --parallel
ctest --test-dir build/contracts -C Release --output-on-failure
ctest --test-dir build/contracts -C Release --output-on-failure -L contracts
```

The full suite includes ordinary success/error tests: public negative cases
must pass with contracts ON and OFF. Four standalone executables compile
`tests/test_contracts.c` for both contract modes and both `NDEBUG` states,
independently of the library's selection. The tests deliberately force each
`NDEBUG` state, even when the outer build type would normally choose the other.
Enabled checks evaluate exactly once; disabled checks do not evaluate their
conditions and accept false expressions without terminating.

Enabled tests run failed preconditions and postconditions in separate child
processes. On POSIX, the parent requires termination by `SIGABRT`, rejecting
ordinary error returns and unrelated signals. Core-file generation is disabled
in the child. On Windows, a fresh copy of the executable suppresses interactive
error dialogs and MSVC CRT report-fault behavior; the parent requires the CRT
abort exit code `3`. A child returning a different code cannot pass as an
expected death. Windows children and CTest cases have bounded timeouts.

An ordinary child exit is also checked as a negative control. CTest observes
the surviving parent, so an expected abort is a passing test only when the
parent verifies its specific outcome. Merely setting `WILL_FAIL` on an arbitrary
crashing program would not establish this behavior.

CI configures contract-enabled static builds on Linux and Windows in both
Debug and Release and runs the complete suite. Publication depends on those
jobs alongside the existing test matrix. See `PROJECT_STATUS.md` for observed
local results; workflow configuration alone does not establish remote success.
