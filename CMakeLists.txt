cmake_minimum_required(VERSION 3.25)

project(SOVOPT
    VERSION 0.1.0
    DESCRIPTION "Sovereign Mathematical Optimization Solver"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(sovopt
    src/main.cpp

    src/core/variable.cpp
    src/core/constraint.cpp
    src/core/objective.cpp
    src/core/model.cpp

    src/math/vector.cpp
    src/math/sparse_matrix.cpp
    src/math/linear_solver.cpp

    src/lp/lp_model.cpp
    src/lp/lp_result.cpp
    src/lp/lp_solver.cpp
    src/lp/simplex_solver.cpp
)

target_include_directories(sovopt
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(sovopt PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
