# Development Guide

This guide is the fastest way to regain context after time away from `cnumkit`. It explains what to read, which invariants must remain true, and how to change the library without weakening its educational value.

## Recommended Reading Order

1. `README.md`: purpose, supported modules, and basic build commands.
2. `docs/PROJECT_STATUS.md`: verified facts and current limitations.
3. `docs/USER_GUIDE.md`: behavior visible to library users.
4. `docs/LIBRARY_ARCHITECTURE.md`: internal data flow and module boundaries.
5. `docs/NUMERICAL_NOTES.md`: floating-point decisions and algorithm limits.
6. `docs/REVIEWER_ROADMAP.md`: ordered future work.
7. `local/LOCAL_CHANGELOG.md`: chronological local reasoning and verification history.

Read the public header for a module before its source file. The header defines the contract; the source should make the reasons behind non-obvious checks visible.

## Repository Map

- `include/cnumkit/`: public types, functions, ownership, and error contracts.
- `src/`: scalar reference implementations, one source file per module.
- `tests/`: lightweight deterministic suites and the local test framework.
- `examples/`: complete user-facing programs with explicit cleanup and error handling.
- `docs/`: public explanations, status, numerical notes, and roadmap.
- `local/`: ignored planning history that must be appended to, not rewritten as public history.
- `cmake/toolchains/`: cross-compilation configuration.
- `.github/workflows/`: clean-checkout automation.

## Core Invariants

Every change must preserve these rules unless an intentional API revision updates the implementation, tests, and documentation together:

- A vector has a positive `size` and a valid contiguous `data` buffer.
- A matrix has positive `rows` and `cols`, a valid contiguous buffer, and row-major indexing.
- Allocation-size multiplication is checked before allocation.
- Created objects are caller-owned; free functions accept `NULL`.
- Successful public operations set `CNK_SUCCESS`.
- Error inspection does not change the current error.
- Caller-provided outputs and models remain unchanged on failure.
- Public numerical inputs and computed results must be finite.
- Scalar getters may return `0.0` on error; the error state disambiguates that value.
- Contract checks supplement runtime validation; they never replace it.
- Scalar behavior remains the correctness reference for future optimized backends.

## How To Change A Module

1. Write down the ownership, valid-input, success, and failure behavior in the public header.
2. Add one analytic success case and the relevant failure cases to the matching test suite.
3. Implement the smallest readable scalar algorithm.
4. Explain decisions in comments where the reason is not evident from the code. Do not narrate obvious assignments or loops.
5. Set a specific error before every failure return and clear the error on success.
6. Assign caller outputs only after all validation and computation succeeds.
7. Update user, architecture, or numerical documentation when behavior or assumptions change.
8. Append the decision and verification result to `local/LOCAL_CHANGELOG.md`.
9. Run strict Debug/shared, Release/static, and contracts-enabled builds.

## Comment Style

Comments and documentation are written in English. A useful comment answers “why this choice?” or “which invariant is protected?” Examples include explaining an overflow check, a numerical threshold, or why contracts are tested separately.

Avoid comments that merely translate code into prose, promises such as “works flawlessly,” speculative TODOs without roadmap context, and informal punctuation in public headers. Prefer short functions and descriptive identifiers so the code carries most of the explanation.

## Verification Commands

Typical strict build:

```bash
cmake -S . -B build-strict \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCNUMKIT_WARNINGS_AS_ERRORS=ON
cmake --build build-strict --parallel
ctest --test-dir build-strict --output-on-failure
```

Release static build:

```bash
cmake -S . -B build-static \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DCNUMKIT_WARNINGS_AS_ERRORS=ON
cmake --build build-static --parallel
ctest --test-dir build-static --output-on-failure
```

Contracts build:

```bash
cmake -S . -B build-contracts \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCNUMKIT_ENABLE_CONTRACTS=ON \
  -DCNUMKIT_WARNINGS_AS_ERRORS=ON
cmake --build build-contracts --parallel
```

Current negative tests run with contracts disabled because intentionally invalid calls would terminate the process. Add dedicated death tests before enabling CTest for contract-violation cases.

## Definition Of Done

A change is ready for review when:

- Public behavior is documented.
- The implementation is warning-clean.
- Success, boundary, and failure paths are tested.
- Numerical tolerances and limits are explicit where relevant.
- Debug/shared and Release/static CTest runs pass.
- The contracts-enabled configuration compiles.
- Public status and local history are current.
- No generated build artifacts are added to version control.
