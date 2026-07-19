# RISC-V Scalar Build Notes

This document describes the intended first RISC-V milestone: cross-build and execute the existing scalar reference implementation. The workflow has not yet been verified in the current environment; commands below are a reproducible target, not a claim of completed support.

RVV acceleration must wait until scalar RISC-V tests pass and scalar performance has been measured.

## Required Tools

- `riscv64-linux-gnu-gcc`
- `qemu-riscv64`
- CMake 3.16 or newer
- A RISC-V Linux sysroot, or static runtime libraries for the cross-compiler

Confirm the installed tools before configuring:

```bash
riscv64-linux-gnu-gcc --version
qemu-riscv64 --version
cmake --version
```

## Recommended Static Smoke Build

A static test executable avoids relying on target shared libraries outside a configured sysroot:

```bash
cmake -S . -B build-riscv \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/riscv64-linux-gnu.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_EXE_LINKER_FLAGS=-static \
  -DCNUMKIT_WARNINGS_AS_ERRORS=ON
cmake --build build-riscv --parallel
ctest --test-dir build-riscv --output-on-failure
```

The toolchain file configures `qemu-riscv64` as CMake's cross-compiling emulator, allowing CTest to prefix test commands with QEMU. If static runtime libraries are unavailable, configure QEMU with an appropriate target sysroot and document the exact path used.

## Evidence To Record

- Cross-compiler version.
- QEMU version.
- Target architecture and ABI flags.
- Static or dynamic linkage choice.
- Sysroot details when dynamic linkage is used.
- CMake configure command.
- CTest results and assertion counts.

## Completion Criterion

Scalar RISC-V support is considered verified only when a clean cross-build succeeds and all scalar test suites execute under QEMU. RVV work must remain optional, preserve the public API, and compare against scalar results within documented tolerances.
