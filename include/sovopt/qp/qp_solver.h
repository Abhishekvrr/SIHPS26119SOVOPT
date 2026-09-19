#pragma once

#include "sovopt/qp/qp_model.h"
#include "sovopt/optimization/optimization_options.h"
#include "sovopt/optimization/optimization_result.h"

#include <cstddef>
#include <vector>

namespace sovopt::qp
{

struct QPResult
{
    optimization::OptimizationStatus status = optimization::OptimizationStatus::Error;
    double objective_value = 0.0;
    std::vector<double> solution;
    std::vector<double> lagrange_multipliers;
    std::size_t iterations = 0;
    double kkt_primal_residual = 0.0;
    double kkt_dual_residual = 0.0;
    double solve_time_ms = 0.0;
    std::string message;
};

class QPSolver
{
public:
    explicit QPSolver(optimization::OptimizationOptions options = {});

    QPResult solve(const QPModel& model);

private:
    optimization::OptimizationOptions options_;

    static bool solve_kkt_system(
        const std::vector<std::vector<double>>& M,
        const std::vector<double>& rhs,
        std::vector<double>& sol
    );
};

} // namespace sovopt::qp
