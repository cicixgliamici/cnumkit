# cnumkit v0.1.0

<!-- Publication template: the tag workflow uses this after verification passes. -->

The first public release of `cnumkit` is a small scalar C11 numerical library
for learning, review, and embedding in scientific-engine prototypes.

It includes owned vectors and matrices, borrowed strided views, allocation-free
vector and matrix kernels, Gaussian linear solving, one-dimensional optimization,
and educational linear regression. Checked scalar access and prediction functions
report errors without an ambiguous `0.0` result.

The principal scientific walkthrough is a two-node thermal network with
unequal conductances, internal heating, and boundary sensitivity. It connects
physical units and conservation equations to analytic temperatures and heat
rates, a normalized algebraic residual, and local/global energy checks. See the
[thermal case study](https://github.com/cicixgliamici/cnumkit/blob/v0.1.0/docs/THERMAL_CASE_STUDY.md)
for reproducible commands and the model's limits.

The release assets contain installed CMake packages for Linux and Windows,
in shared and static variants. Source archives are provided by GitHub. See the
[user guide](https://github.com/cicixgliamici/cnumkit/blob/v0.1.0/docs/USER_GUIDE.md)
for ownership and error behavior, and the
[changelog](https://github.com/cicixgliamici/cnumkit/blob/v0.1.0/CHANGELOG.md)
for the full scope and known limits.

The shared archives are Debug builds; the static archives are Release builds.
Linux binaries use the workflow's Ubuntu/GCC toolchain and Windows binaries use
MSVC. Match the platform, architecture, and runtime, or build from source.
`SHA256SUMS` accompanies the four binary archives. See the
[packaging guide](https://github.com/cicixgliamici/cnumkit/blob/v0.1.0/docs/PACKAGING.md)
for verification, extraction, CMake consumption, and Windows DLL setup. Offline
usage and numerical documentation are included under `share/doc/cnumkit/`.

The API is pre-1.0 and version compatibility is limited to `0.1.x`. The scalar
implementation is a reference and is not a replacement for BLAS or LAPACK.
