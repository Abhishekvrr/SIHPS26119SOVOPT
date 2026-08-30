#pragma once

#include <cmath>
#include <cstddef>
#include <limits>

namespace sovopt::math
{

struct NumericalSettings
{
    double feasibility_tolerance = 1e-9;
    double optimality_tolerance = 1e-9;
    double zero_tolerance = 1e-12;
    double pivot_tolerance = 1e-12;

    std::size_t max_iterations = 10000;
};

inline bool is_zero(
    double value,
    double tolerance = 1e-12)
{
    return std::abs(value) <= tolerance;
}

inline bool is_positive(
    double value,
    double tolerance = 1e-12)
{
    return value > tolerance;
}

inline bool is_negative(
    double value,
    double tolerance = 1e-12)
{
    return value < -tolerance;
}

inline bool approximately_equal(
    double a,
    double b,
    double tolerance = 1e-9)
{
    return std::abs(a - b) <= tolerance;
}

inline double infinity()
{
    return std::numeric_limits<double>::infinity();
}

} // namespace sovopt::math
