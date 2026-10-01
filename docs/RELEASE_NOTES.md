# cnumkit v0.1.0

The first public release of `cnumkit` is a small scalar C11 numerical library
for learning, review, and embedding in scientific-engine prototypes.

It includes owned vectors and matrices, borrowed strided views, allocation-free
vector and matrix kernels, Gaussian linear solving, one-dimensional optimization,
and educational linear regression. Checked scalar access and prediction functions
report errors without an ambiguous `0.0` result.

The release assets contain installed CMake packages for Linux and Windows,
in shared and static variants. Source archives are provided by GitHub. See the
[user guide](https://github.com/cicixgliamici/cnumkit/blob/v0.1.0/docs/USER_GUIDE.md)
for ownership and error behavior, and the
[changelog](https://github.com/cicixgliamici/cnumkit/blob/v0.1.0/CHANGELOG.md)
for the full scope and known limits.

The API is pre-1.0 and version compatibility is limited to `0.1.x`. The scalar
implementation is a reference and is not a replacement for BLAS or LAPACK.
