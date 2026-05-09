# cnumkit

`cnumkit` (C Num Kit) is a lightweight C library for numerical analysis, linear algebra, optimization, and basic machine learning algorithms. It is designed to be simple, clean, and easy to integrate into other C/C++ projects.

## Features

- **Vectors and Matrices**: Dynamic allocation, basic algebraic operations, norms, dot products.
- **Linear Algebra**: System solvers (Gaussian elimination).
- **Optimization**: Gradient descent and basic function minimizers.
- **Machine Learning**: Linear regression and other fundamental ML utilities.

## Build Instructions

The project uses CMake as its build system. To build the library and tests/examples:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage Example

Include the main header file to use the library:

```c
#include <stdio.h>
#include "cnumkit.h"

int main() {
    cnk_vector *v1 = cnk_vector_create(3);
    cnk_vector *v2 = cnk_vector_create(3);
    
    cnk_vector_set(v1, 0, 1.0);
    cnk_vector_set(v2, 0, 2.0);
    
    double dot_prod;
    cnk_vector_dot(v1, v2, &dot_prod);
    printf("Dot product: %f\n", dot_prod);
    
    cnk_vector_free(v1);
    cnk_vector_free(v2);
    return 0;
}
```

## Project Structure

- `include/`: Public headers. You can include `cnumkit.h` for all functionalities.
- `src/`: Implementation of the library.
- `examples/`: Code examples demonstrating how to use the modules.
- `tests/`: Unit tests to ensure reliability.