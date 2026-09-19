#pragma once

#include <cmath>
#include <cstddef>
#include <limits>

namespace sovopt::math {

struct NumericalSettings {
    double feasibility_tolerance = 1e-9;
    double optimality_tolerance = 1e-9;
    double pivot_tolerance = 1e-10;
    double zero_tolerance = 1e-12;
    double bound_tolerance = 1e-9;
    double integrality_tolerance = 1e-7;

    std::size_t max_iterations = 100000;

    [[nodiscard]] bool is_zero(double value) const noexcept {
        return std::abs(value) <= zero_tolerance;
    }

    [[nodiscard]] bool is_feasible(double violation) const noexcept {
        return violation <= feasibility_tolerance;
    }

    [[nodiscard]] bool is_optimal(double reduced_cost) const noexcept {
        return reduced_cost <= optimality_tolerance;
    }

    [[nodiscard]] bool is_valid(double value) const noexcept {
        return std::isfinite(value);
    }
};

using NumericalConfig = NumericalSettings;

inline double infinity() noexcept {
    return std::numeric_limits<double>::infinity();
}

inline bool is_zero(double value, double tolerance = 1e-12) noexcept {
    return std::abs(value) <= tolerance;
}

inline bool is_positive(double value, double tolerance = 1e-12) noexcept {
    return value > tolerance;
}

inline bool is_negative(double value, double tolerance = 1e-12) noexcept {
    return value < -tolerance;
}

inline bool approximately_equal(double a, double b, double tolerance = 1e-9) noexcept {
    return std::abs(a - b) <= tolerance;
}

} // namespace sovopt::math