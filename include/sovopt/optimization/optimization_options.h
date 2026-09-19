#pragma once

#include "sovopt/math/numerical.h"

#include <cstddef>
#include <string>

namespace sovopt::optimization
{

enum class SolverType
{
    Auto,
    Simplex,
    DualSimplex,
    BranchAndBound,
    BranchAndCut,
    ActiveSetQP,
    InteriorPoint
};

enum class HardwareAcceleration
{
    CPU_Scalar,
    CPU_SIMD,
    GPU_Accelerated
};

enum class PresolveLevel
{
    None,
    Basic,
    Aggressive
};

enum class Verbosity
{
    Silent,
    Summary,
    Detailed
};

struct OptimizationOptions
{
    SolverType solver_type{SolverType::Auto};
    HardwareAcceleration acceleration{HardwareAcceleration::CPU_SIMD};
    PresolveLevel presolve_level{PresolveLevel::Basic};
    Verbosity verbosity{Verbosity::Summary};

    // Numerical tolerances
    double feasibility_tolerance{1e-9};
    double optimality_tolerance{1e-9};
    double integrality_tolerance{1e-6};
    double zero_tolerance{1e-12};
    double pivot_tolerance{1e-10};

    // Computation limits
    std::size_t max_iterations{100000};
    double time_limit_seconds{60.0};
    std::size_t max_nodes{100000};

    // Feature switches
    bool enable_presolve{true};
    bool enable_validation{true};
    bool enable_explainability{true};
    bool enable_gomory_cuts{true};
    std::size_t max_cut_rounds{5};

    [[nodiscard]] math::NumericalSettings to_numerical_settings() const noexcept
    {
        math::NumericalSettings settings;
        settings.feasibility_tolerance = feasibility_tolerance;
        settings.optimality_tolerance = optimality_tolerance;
        settings.integrality_tolerance = integrality_tolerance;
        settings.zero_tolerance = zero_tolerance;
        settings.pivot_tolerance = pivot_tolerance;
        settings.max_iterations = max_iterations;
        return settings;
    }
};

inline const char* to_string(SolverType type) noexcept
{
    switch (type)
    {
        case SolverType::Auto:
            return "AUTO";
        case SolverType::Simplex:
            return "SIMPLEX";
        case SolverType::DualSimplex:
            return "DUAL_SIMPLEX";
        case SolverType::BranchAndBound:
            return "BRANCH_AND_BOUND";
        case SolverType::BranchAndCut:
            return "BRANCH_AND_CUT";
        case SolverType::ActiveSetQP:
            return "ACTIVE_SET_QP";
        case SolverType::InteriorPoint:
            return "INTERIOR_POINT";
    }
    return "UNKNOWN";
}

inline const char* to_string(HardwareAcceleration accel) noexcept
{
    switch (accel)
    {
        case HardwareAcceleration::CPU_Scalar:
            return "CPU (Scalar Core)";
        case HardwareAcceleration::CPU_SIMD:
            return "CPU (SIMD AVX2/SSE Vectorized)";
        case HardwareAcceleration::GPU_Accelerated:
            return "GPU (Parallel Matrix Workgroup Core)";
    }
    return "CPU (Default)";
}

} // namespace sovopt::optimization
