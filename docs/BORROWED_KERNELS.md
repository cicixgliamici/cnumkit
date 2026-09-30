# Borrowed Views And Scalar Kernels

The `f64` kernel API operates on caller-owned `double` storage without copying
buffers or allocating scratch memory. It is the first step toward a numerical
runtime for C++, Rust, and other scientific engines. The API is still pre-1.0;
it is not yet a frozen ABI, and no RVV backend or language wrapper is implemented.

## Layout And Lifetime

Include `cnumkit/kernels.h` directly or use `cnumkit.h`. Vector views contain a
pointer, length, and element stride. Matrix views contain a pointer, shape, and
independent row and column strides. Strides count `double` elements, not bytes.
Read-only descriptors use `const double *`; mutable descriptors use `double *`.

Examples of a matrix with three rows and four columns:

| Storage | Row stride | Column stride |
| --- | ---: | ---: |
| Contiguous row-major | 4 | 1 |
| Contiguous column-major | 1 | 3 |
| Row-major with two padding elements per row | 6 | 1 |
| Rows of the contiguous row-major matrix reversed | -4 | 1 |

For the reversed case, `data` must point at the first element of the last physical
row. To transpose a view, exchange its shape dimensions and its two strides.
Every addressed element must be within a live, aligned allocation. Metadata
validation cannot establish allocation size, provenance, or lifetime.

Constructors ending in `_make` only assemble descriptors; they do not validate
or modify the thread-local error state. Kernels validate metadata before reading
or writing. Nonempty axes longer than one element require nonzero strides, and
matrix elements must have distinct addresses; broadcasting is not supported.
The sum of absolute axis spans, in bytes, must fit `PTRDIFF_MAX`. This conservative
bound prevents integer overflow in subsequent address calculations.

Empty vectors and matrices may contain NULL pointers. A singleton axis may use
any stride because no movement along that axis occurs. An empty matrix has at
least one zero dimension. Shape checks still apply to empty products: multiplying
an `m x 0` matrix by a `0 x n` matrix writes an `m x n` zero matrix.

## Operations And Error Behavior

- `cnk_f64_fill`: overwrite a vector with one finite value.
- `cnk_f64_copy`: copy finite elements to an equally sized view.
- `cnk_f64_scale`: scale a vector in place.
- `cnk_f64_axpy`: compute `y = alpha * x + y`.
- `cnk_f64_dot`: reduce two vectors to their dot product.
- `cnk_f64_norm2`: compute a scaled sum-of-squares Euclidean norm.
- `cnk_f64_matvec`: overwrite a vector with `A * x`.
- `cnk_f64_matmul`: overwrite a matrix with `A * B`.

Every kernel returns `cnk_error_code` directly and updates thread-local
diagnostics. Failures leave caller outputs unchanged. Copy, scale, and AXPY
preflight their inputs/results before writing. Matrix products compute every
output twice: one validation pass and one writing pass. This avoids hidden
allocation but costs additional computation. It is an explicit scalar reference
tradeoff to revisit with benchmark evidence and workspace APIs.

Fill, copy, and matrix products do not read old destination values. Input values
actually read must be finite; fill/scale/AXPY scalar parameters must also be
finite, even on empty views. Empty outputs do not trigger numerical reads.

The caller must prevent concurrent mutation and honor these overlap rules:

- Copy and AXPY support exactly identical input/output views, or disjoint logical
  elements. Other overlap is outside the contract and is not detected.
- Matrix-product outputs must not overlap any input elements. Inputs may overlap
  each other, for example when computing `A * A` into a separate buffer.
- Dot and norm finish reading before writing their scalar result, which may
  therefore alias an input element.

The norm scales values before squaring, so inputs such as `[3e300, 4e300]` produce
a representable norm instead of overflowing intermediate squares. Tiny values
are subject to the platform's subnormal handling. An unrepresentable final norm
returns `CNK_ERROR_MATH`.

Dot and matrix products accumulate using `long double`. Its range and precision
are platform-dependent, so this is neither exact summation nor a promise that
all mathematically finite cancellation-heavy products succeed. There is no
bitwise reproducibility guarantee across compilers/backends. Fast-math modes
that discard finite-value checks are outside the supported build policy.

## Existing Owned Objects

`cnumkit/adapters.h` provides mutable and const adapters for `cnk_vector` and
`cnk_matrix`. They validate metadata and write a borrowed descriptor through an
output pointer. No allocation or ownership transfer occurs; freeing or replacing
the owner's storage invalidates every borrowed view. Legacy owners still require
positive dimensions. Their element values are checked only when read by kernels.

Legacy `cnk_vector_dot` and `cnk_vector_norm2` delegate to the shared scalar core,
preserving their `0`/`-1` return convention and thread-local diagnostics. They now
reject non-finite reduction inputs/results, and the legacy norm benefits from
scaled accumulation. Legacy matrix multiplication retains its owned-result API.

## Example And Verification

`examples/borrowed_matvec.c` demonstrates a padded matrix and engine-owned input
and output buffers. The expected result is `[14.0, 32.0]`.

The kernel suites exercise reversed/padded/transposed layouts, late numerical
failures, empty products, metadata overflow, and norms from `1e-300` to `1e300`.
Norm cases also compare against the platform `hypot` implementation. Optional
`CNUMKIT_BUILD_CPP_SMOKE_TEST=ON` compiles and links individual headers directly
from C++, without requiring a C++ compiler for ordinary C builds.

On Linux with GCC and sanitizer runtimes installed, `bash tests/run_sanitizers.sh`
builds and runs the five C suites with ASan (including leak detection) and UBSan.
This path requires no CMake; artifacts stay in `build/linux-sanitized`.
