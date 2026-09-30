#!/usr/bin/env bash
set -euo pipefail

# This direct Linux build also exercises the source-tree export-header fallback.
# It is useful on minimal systems with a compiler but no CMake installation.
cd "$(dirname "${BASH_SOURCE[0]}")/.."
test_build_dir="build/linux-sanitized"
mkdir -p "$test_build_dir"
test_compiler="${CC:-gcc}"
test_flags=(-std=c11 -g -O1 -Wall -Wextra -Wpedantic -Werror
    -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude)
test_objects=()

"$test_compiler" --version
for source_file in src/*.c; do
    object_file="$test_build_dir/$(basename "${source_file%.c}").o"
    "$test_compiler" "${test_flags[@]}" -c "$source_file" -o "$object_file"
    test_objects+=("$object_file")
done

for test_name in test_basic test_optim test_ml test_kernels test_matrix_kernels test_math_scenarios; do
    "$test_compiler" "${test_flags[@]}" "tests/$test_name.c" "${test_objects[@]}" \
        -lm -o "$test_build_dir/$test_name"
    # UBSan must fail the run, not merely print a diagnostic and return success.
    ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
        "$test_build_dir/$test_name"
done
