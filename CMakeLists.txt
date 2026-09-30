cmake_minimum_required(VERSION 3.16)

project(cnumkit VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(GNUInstallDirs)

# Keep standard CMake linkage selection available to both reviewers and consumers.
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(CNUMKIT_ENABLE_CONTRACTS "Enable runtime contract checks" OFF)
option(CNUMKIT_ENABLE_SANITIZERS "Enable address/undefined behavior sanitizers when supported" OFF)
option(CNUMKIT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Keep reviewer builds strict while allowing library users to opt out of -Werror.
function(cnumkit_apply_project_options target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
        if(CNUMKIT_WARNINGS_AS_ERRORS)
            target_compile_options(${target_name} PRIVATE /WX)
        endif()
    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(CNUMKIT_WARNINGS_AS_ERRORS)
            target_compile_options(${target_name} PRIVATE -Werror)
        endif()
    endif()

    # Every final executable must link the sanitizer runtime, not only the library.
    if(CNUMKIT_ENABLE_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

include(CTest)

add_library(cnumkit
    src/vector.c
    src/matrix.c
    src/linalg.c
    src/optim.c
    src/ml.c
    src/error.c
    src/view.c
    src/kernels_scalar.c
    src/kernels_matrix.c
    src/view_validation.c
    src/adapters.c
)

# Match the installed namespace when the target is consumed in-tree.
add_library(cnumkit::cnumkit ALIAS cnumkit)

set_target_properties(cnumkit PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME cnumkit
)

target_include_directories(cnumkit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Generate platform-specific symbol visibility attributes for shared builds.
include(GenerateExportHeader)
generate_export_header(cnumkit
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/cnumkit_export.h
)
target_include_directories(cnumkit BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

cnumkit_apply_project_options(cnumkit)

if(CNUMKIT_ENABLE_CONTRACTS)
    target_compile_definitions(cnumkit PUBLIC CNUMKIT_ENABLE_CONTRACTS)
endif()

# Link the C math library where it exists as a separate library.
if(NOT MSVC)
    target_link_libraries(cnumkit PUBLIC m)
endif()

# -----------------
# Tests and Examples
# -----------------
option(CNUMKIT_BUILD_TESTS "Build tests" ON)
option(CNUMKIT_BUILD_CPP_SMOKE_TEST "Verify individual public headers from C++" OFF)
if(CNUMKIT_BUILD_TESTS AND BUILD_TESTING)
    add_executable(test_basic tests/test_basic.c)
    target_link_libraries(test_basic PRIVATE cnumkit)
    cnumkit_apply_project_options(test_basic)
    add_test(NAME test_basic COMMAND test_basic)

    add_executable(test_optim tests/test_optim.c)
    target_link_libraries(test_optim PRIVATE cnumkit)
    cnumkit_apply_project_options(test_optim)
    add_test(NAME test_optim COMMAND test_optim)

    add_executable(test_ml tests/test_ml.c)
    target_link_libraries(test_ml PRIVATE cnumkit)
    cnumkit_apply_project_options(test_ml)
    add_test(NAME test_ml COMMAND test_ml)

    add_executable(test_kernels tests/test_kernels.c)
    target_link_libraries(test_kernels PRIVATE cnumkit)
    cnumkit_apply_project_options(test_kernels)
    add_test(NAME test_kernels COMMAND test_kernels)

    add_executable(test_matrix_kernels tests/test_matrix_kernels.c)
    target_link_libraries(test_matrix_kernels PRIVATE cnumkit)
    cnumkit_apply_project_options(test_matrix_kernels)
    add_test(NAME test_matrix_kernels COMMAND test_matrix_kernels)

    # Scenario tests check mathematical results across multiple public modules.
    add_executable(test_math_scenarios tests/test_math_scenarios.c)
    target_link_libraries(test_math_scenarios PRIVATE cnumkit)
    cnumkit_apply_project_options(test_math_scenarios)
    add_test(NAME test_math_scenarios COMMAND test_math_scenarios)

    # A C-only consumer should not need a C++ compiler to configure the library.
    if(CNUMKIT_BUILD_CPP_SMOKE_TEST)
        enable_language(CXX)
        add_executable(test_cpp_headers tests/test_cpp_headers.cpp)
        target_link_libraries(test_cpp_headers PRIVATE cnumkit)
        target_compile_features(test_cpp_headers PRIVATE cxx_std_11)
        cnumkit_apply_project_options(test_cpp_headers)
        add_test(NAME test_cpp_headers COMMAND test_cpp_headers)
    endif()
endif()

option(CNUMKIT_BUILD_EXAMPLES "Build examples" ON)
if(CNUMKIT_BUILD_EXAMPLES)
    add_executable(borrowed_matvec examples/borrowed_matvec.c)
    target_link_libraries(borrowed_matvec PRIVATE cnumkit)
    cnumkit_apply_project_options(borrowed_matvec)

    add_executable(solve_linear_system examples/solve_linear_system.c)
    target_link_libraries(solve_linear_system PRIVATE cnumkit)
    cnumkit_apply_project_options(solve_linear_system)

    add_executable(linear_regression_demo examples/linear_regression_demo.c)
    target_link_libraries(linear_regression_demo PRIVATE cnumkit)
    cnumkit_apply_project_options(linear_regression_demo)

    add_executable(sensor_calibration examples/sensor_calibration.c)
    target_link_libraries(sensor_calibration PRIVATE cnumkit)
    cnumkit_apply_project_options(sensor_calibration)

    add_executable(thermal_network examples/thermal_network.c)
    target_link_libraries(thermal_network PRIVATE cnumkit)
    cnumkit_apply_project_options(thermal_network)
endif()

# -----------------
# Installation
# -----------------
install(TARGETS cnumkit
    EXPORT cnumkit-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/cnumkit_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT cnumkit-targets
    FILE cnumkit-targets.cmake
    NAMESPACE cnumkit::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cnumkit
)
