# v0.1.0 Release Checklist

This checklist records evidence needed before creating a public tag. A source
checkout is not a release merely because its CMake project version is `0.1.0`.

## Scope and policy

- [x] MIT license selected and included in the source and install trees.
- [x] Current API, ownership rules, failure behavior, and numerical limits documented.
- [x] Public vector and matrix structs retained for `0.1.x`; changing their
  layout requires a new minor version before 1.0.
- [x] Status-returning alternatives added for scalar getters and prediction;
  value-returning functions retained for existing callers.
- [x] Changelog prepared as an unreleased candidate.
- [x] Binary archive selection, checksum verification, CMake consumption, and
  Windows DLL setup documented in `PACKAGING.md`.
- [x] Offline usage documentation included in installed packages.
- [x] Principal thermal case study documents units, analytic references,
  boundary sensitivity, numerical residuals, energy balances, and model limits.

## Verification

- [x] Clean Linux GCC Debug/shared and Release/static CMake builds and CTest.
- [x] Installed Linux static and shared packages used by a separate consumer.
- [x] Linux static and shared archives extracted at a new prefix and consumed
  with the original installation prefix unavailable.
- [x] Linux ASan and UBSan run for all seven C suites, including the thermal model.
- [x] Public-error semantics and isolated contract deaths verified locally with
  contracts ON/OFF, both `NDEBUG` states, Debug/Release, and GCC sanitizers.
- [ ] Complete remote GCC, Clang, and MSVC CI run on the candidate commit.
- [ ] Installed Windows static and shared packages consumed by a separate project.
- [x] Warning-free Doxygen generation with warnings treated as errors on Linux.
- [ ] Final build, test, install, and consumer check from the exact tagged commit.

## Tagging

- [ ] Review the final increment, commit the candidate, and confirm a clean worktree.
- [ ] Record the commit hash, compiler versions, and CI run links in the release notes.
- [ ] Change the changelog heading from "Unreleased" to the release date.
- [ ] Create and push annotated `v0.1.0` only after the candidate CI passes.
- [ ] Confirm the tag workflow passes and publishes four installable archives
  and `SHA256SUMS`, plus GitHub's source archives as a GitHub Release.

The CI workflow now tests consumers against extracted, relocated archives on
Linux and Windows and explicitly enables the C++ header smoke test. These are
configured checks; mark remote evidence complete only after observing the run.
See `PACKAGING.md` for the archive layout and publication procedure.
