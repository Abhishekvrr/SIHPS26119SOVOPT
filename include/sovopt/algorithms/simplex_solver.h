#pragma once

#include "sovopt/lp/lp_model.h"
#include "sovopt/math/numerical.h"

#include <string>
#include <vector>

namespace sovopt::algorithms {

enum class LPStatus {
    Optimal,
    Infeasible,
    Unbounded,
    IterationLimit,
    NumericalFailure,
    InvalidModel
};

struct LPResult {
    LPStatus status = LPStatus::InvalidModel;

    std::vector<double> solution;

    double objective_value = 0.0;
    double feasibility_error = 0.0;

    std::size_t iterations = 0;
    double runtime_ms = 0.0;

    std::string message;
};

class SimplexSolver {
public:
    explicit SimplexSolver(
        math::NumericalConfig config = {});

    LPResult solve(const lp::LPModel& model);

private:
    math::NumericalConfig config_;

    LPResult solve_standard_form(
        const std::vector<std::vector<double>>& a,
        const std::vector<double>& b,
        const std::vector<double>& c,
        std::size_t original_variables,
        double objective_sign);
};

const char* to_string(LPStatus status) noexcept;

} // namespace sovopt::algorithms
