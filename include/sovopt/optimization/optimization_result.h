#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace sovopt::optimization
{

enum class OptimizationStatus
{
    Optimal,
    Feasible,
    Infeasible,
    Unbounded,
    IterationLimit,
    TimeLimit,
    NodeLimit,
    NumericalError,
    InvalidModel,
    Error
};

const char* to_string(OptimizationStatus status) noexcept;

struct OptimizationResult
{
    OptimizationStatus status{OptimizationStatus::Error};

    double objective_value{0.0};
    std::vector<double> variable_values;
    std::vector<std::string> variable_names;

    std::size_t iterations{0};
    std::size_t nodes_explored{0};
    std::size_t cuts_added{0};
    std::size_t presolve_reductions_count{0};
    double duality_gap{0.0};

    double solve_time_ms{0.0};
    double presolve_time_ms{0.0};
    double validation_time_ms{0.0};
    double total_time_ms{0.0};

    double max_primal_violation{0.0};
    double max_dual_violation{0.0};
    double kkt_primal_residual{0.0};
    double kkt_dual_residual{0.0};
    double simd_speedup_factor{1.0};
    bool is_valid{false};

    std::string solver_used{"Unknown"};
    std::string problem_type{"Unknown"};
    std::string acceleration_used{"CPU (SIMD Vectorized)"};
    std::string message;

    [[nodiscard]] bool is_optimal() const noexcept
    {
        return status == OptimizationStatus::Optimal;
    }

    [[nodiscard]] bool is_feasible() const noexcept
    {
        return status == OptimizationStatus::Optimal || status == OptimizationStatus::Feasible;
    }

    [[nodiscard]] double get_variable_value(std::size_t index) const;
    [[nodiscard]] double get_variable_value(const std::string& name) const;
    [[nodiscard]] std::unordered_map<std::string, double> get_solution_map() const;

    [[nodiscard]] std::string to_string_summary() const;
};

} // namespace sovopt::optimization
