cmake_minimum_required(VERSION 3.25)

project(SOVOPT
    VERSION 0.2.0
    DESCRIPTION "Sovereign Mathematical Optimization Solver (MILP, QP, Presolve, Dual Simplex, SIMD/GPU)"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(
        /W4
        /permissive-
    )
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

set(SOVOPT_CORE_SOURCES
    src/core/variable.cpp
    src/core/constraint.cpp
    src/core/objective.cpp
    src/core/model.cpp
    src/core/evaluator.cpp
    src/core/solution.cpp

    src/math/vector.cpp
    src/math/sparse_matrix.cpp
    src/math/linear_solver.cpp
    src/math/simd_vector.cpp
    src/math/gpu_accelerator.cpp

    src/lp/lp_model.cpp
    src/lp/lp_result.cpp
    src/lp/lp_solver.cpp
    src/lp/simplex_solver.cpp
    src/lp/dual_simplex_solver.cpp
    src/lp/presolve.cpp

    src/algorithms/branch_and_bound_solver.cpp

    src/qp/qp_model.cpp
    src/qp/qp_solver.cpp

    src/optimization/optimization_result.cpp
    src/optimization/optimization_engine.cpp

    src/io/json_io.cpp
    src/api/solver_api.cpp
)

add_executable(sovopt
    src/main.cpp
    ${SOVOPT_CORE_SOURCES}
)

target_include_directories(
    sovopt
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(
    sovopt
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
)

add_executable(sovopt_tests
    tests/test_solver.cpp
    ${SOVOPT_CORE_SOURCES}
)

target_include_directories(
    sovopt_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(
    sovopt_tests
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
)