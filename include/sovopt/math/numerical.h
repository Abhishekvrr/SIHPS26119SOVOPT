#pragma once

#include <cmath>
#include <cstddef>
#include <limits>

namespace sovopt::math {

struct NumericalSettings {
    double feasibility_tolerance = 1e-8;
    double optimality_tolerance = 1e-8;
    double pivot_tolerance = 1e-10;
    double zero_tolerance = 1e-12;
    double bound_tolerance = 1e-8;

    std::size_t max_iterations = 100000;

    bool is_zero(double value) const noexcept {
        return std::abs(value) <= zero_tolerance;
    }

    bool is_feasible(double violation) const noexcept {
        return violation <= feasibility_tolerance;
    }

    bool is_optimal(double reduced_cost) const noexcept {
        return reduced_cost <= optimality_tolerance;
    }

    bool is_valid(double value) const noexcept {
        return std::isfinite(value);
    }
};

inline constexpr double infinity =
    std::numeric_limits<double>::infinity();

} // namespace sovopt::math