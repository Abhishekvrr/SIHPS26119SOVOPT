#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace sovopt::lp {

enum class LPStatus {
    Optimal,
    Infeasible,
    Unbounded,
    IterationLimit,
    NumericalError,
    InvalidModel
};

struct LPResult {
    LPStatus status = LPStatus::NumericalError;

    double objective_value = 0.0;

    std::vector<double> solution;

    std::size_t iterations = 0;

    double primal_residual = 0.0;
    double dual_residual = 0.0;

    double solve_time_seconds = 0.0;

    std::string message;
};

const char* to_string(LPStatus status) noexcept;

} // namespace sovopt::lp