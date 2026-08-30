#pragma once

#include <cstddef>

namespace sovopt::math {

struct NumericalConfig {
    double feasibility_tolerance = 1e-9;
    double optimality_tolerance = 1e-9;
    double pivot_tolerance = 1e-10;
    double zero_tolerance = 1e-12;

    std::size_t max_iterations = 100000;
};

inline bool is_zero(double value,
                    double tolerance = 1e-12) noexcept
{
    return value >= -tolerance && value <= tolerance;
}

inline bool is_positive(double value,
                        double tolerance = 1e-12) noexcept
{
    return value > tolerance;
}

inline bool is_negative(double value,
                        double tolerance = 1e-12) noexcept
{
    return value < -tolerance;
}

} // namespace sovopt::math
