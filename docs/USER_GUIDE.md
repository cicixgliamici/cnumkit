# cnumkit User Guide

Welcome to the **cnumkit** (C Num Kit) documentation. This guide will walk you through the architectural design of the library and provide practical examples of how to use its modules.

## Architecture & Design Philosophy

`cnumkit` is designed to be a lightweight, zero-dependency C library for numerical analysis and machine learning. Its architecture is built around professional-grade paradigms specifically designed for robust scientific computing.

### 1. Opaque and Contiguous Data Structures
Core structures like `cnk_vector` and `cnk_matrix` are simple, flat, and use contiguous memory (row-major for matrices). This guarantees high cache locality and makes the structures easily translatable into hardware-accelerated SIMD instructions (like RISC-V RVV or x86 AVX) in the future.

### 2. The Two-Tiered Safety Architecture
A fundamental problem in C libraries is how to handle invalid inputs (e.g., multiplying matrices of incompatible dimensions). `cnumkit` solves this using a two-tiered approach:

#### Tier 1: Formal Verification & Design By Contract (Fail-Fast)
To prevent bugs during development, the library employs **Design By Contract (DbC)**.
- **Theory**: Every function has a strict mathematical "contract" (Preconditions it expects, Postconditions it guarantees). We define these formally using **ACSL (ANSI/ISO C Specification Language)** in the Doxygen comments (e.g., `/*@ requires a->cols == b->rows; */`).
- **Implementation**: At runtime, these contracts are enforced via the `CNK_REQUIRES` and `CNK_ENSURES` macros. If a developer breaks a mathematical rule, the program crashes immediately (Fail-Fast) pointing to the exact broken contract.
- **Zero-Overhead**: Because these checks are implemented as macros, they compile away completely in `Release` mode (`NDEBUG`), guaranteeing absolute maximum performance for scientific computation.

#### Tier 2: Thread-Local Error Handling (Fail-Soft)
For runtime errors in production (e.g., running out of memory, or loading user-provided invalid datasets), crashing is unacceptable.
- **Theory**: The library uses a "Fail-Soft" approach. Functions return `NULL` or `-1` on failure, but they do not fail silently.
- **Implementation**: Before returning, the function records exactly what went wrong using a `THREAD_LOCAL` variable. This makes the library perfectly thread-safe. You can query `cnk_get_last_error()` and `cnk_get_last_error_message()` at any time to understand why a mathematical operation failed without polluting function signatures with double pointers.

### 3. Cross-Platform Compatibility
Using CMake and automatic export headers (`cnumkit_export.h`), the library compiles flawlessly as a shared library (`.dll`, `.so`) or static library (`.a`, `.lib`) across Windows, Linux, and macOS. The export macros handle the complex symbol visibility rules automatically.

## Modules Overview

### 1. Vectors and Matrices (`vector.h`, `matrix.h`)
These are the foundational building blocks. They provide dynamic allocation and basic mathematical operations (dot product, norms, matrix multiplication). Memory is allocated dynamically; you *must* remember to call `cnk_vector_free()` or `cnk_matrix_free()` when done.

### 2. Linear Algebra (`linalg.h`)
Contains solvers for systems of linear equations. Currently implemented:
- **Gaussian Elimination with Partial Pivoting**: Extremely stable for solving $Ax = b$.

### 3. Optimization (`optim.h`)
Provides tools for finding function minima and calculating derivatives.
- **Numerical Derivative**: Central difference method.
- **Gradient Descent (1D)**: A basic iterative optimizer.

### 4. Machine Learning (`ml.h`)
Implements fundamental ML concepts.
- **Linear Regression**: A simple 1D linear model trained using gradient descent to minimize Mean Squared Error (MSE).

## Practical Example: Training a Linear Model

Here is how you can use the library to train a simple machine learning model:

```c
#include <stdio.h>
#include "cnumkit.h"

int main() {
    // 1. Create dataset (y = 3x + 2)
    cnk_vector *x = cnk_vector_create(3);
    cnk_vector *y = cnk_vector_create(3);
    
    cnk_vector_set(x, 0, 1.0); cnk_vector_set(y, 0, 5.0);
    cnk_vector_set(x, 1, 2.0); cnk_vector_set(y, 1, 8.0);
    cnk_vector_set(x, 2, 3.0); cnk_vector_set(y, 2, 11.0);

    // 2. Initialize model and fit
    cnk_linear_regression_model model;
    int status = cnk_ml_linear_regression_fit(x, y, 0.01, 1000, &model);
    
    // Demonstrate Tier 2: Fail-Soft Error Handling
    if (status != 0) {
        printf("Error: %s\n", cnk_get_last_error_message());
        return 1;
    }

    // 3. Output results
    printf("Learned equation: y = %.2f * x + %.2f\n", model.weight, model.bias);

    // 4. Cleanup
    cnk_vector_free(x);
    cnk_vector_free(y);
    return 0;
}
```

## Integrating into your project

Using CMake, you can easily pull this library into your own:

```cmake
find_package(cnumkit REQUIRED)
target_link_libraries(your_executable PRIVATE cnumkit::cnumkit)
```
