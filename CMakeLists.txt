cmake_minimum_required(VERSION 3.16)

project(cnumkit C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(cnumkit
    src/vector.c
    src/matrix.c
    src/linalg.c
    src/optim.c
    src/ml.c
)

target_include_directories(cnumkit PUBLIC include)

target_compile_options(cnumkit PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(test_basic tests/test_basic.c)
target_link_libraries(test_basic PRIVATE cnumkit m)

add_executable(test_optim tests/test_optim.c)
target_link_libraries(test_optim PRIVATE cnumkit m)

add_executable(test_ml tests/test_ml.c)
target_link_libraries(test_ml PRIVATE cnumkit m)

add_executable(solve_linear_system examples/solve_linear_system.c)
target_link_libraries(solve_linear_system PRIVATE cnumkit m)

add_executable(linear_regression_demo examples/linear_regression_demo.c)
target_link_libraries(linear_regression_demo PRIVATE cnumkit m)
