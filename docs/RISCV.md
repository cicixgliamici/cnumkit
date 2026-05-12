# RISC-V Build Notes

RISC-V support starts with cross-compiling and running the existing scalar implementation. RVV acceleration should be added only after scalar behavior is stable and covered by parity tests.

## Expected Tools

- `riscv64-linux-gnu-gcc`
- `qemu-riscv64`
- CMake 3.16 or newer

## Configure, Build, Test

```bash
cmake -S . -B build-riscv -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/riscv64-linux-gnu.cmake
cmake --build build-riscv
ctest --test-dir build-riscv --output-on-failure
```

## First Milestone

The first RISC-V milestone is a passing scalar build and smoke-test run. RVV work belongs behind an explicit option or compile-time feature check, with scalar C kept as the reference implementation.
