#pragma once

#include "sovopt/lp/lp_model.h"
#include "sovopt/lp/lp_result.h"
#include "sovopt/core/variable.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace sovopt::algorithms
{

enum class SolveStatus
{
    NotSolved,
    Optimal,
    Feasible,
    Infeasible,
    TimeLimit,
    NodeLimit,
    NumericalError
};

enum class BranchingRule
{
    MostFractional,
    FirstFractional,
    PseudoCost
};

enum class NodeSelection
{
    BestBound,
    DepthFirst,
    BreadthFirst
};

struct BranchAndBoundOptions
{
    std::size_t node_limit = 100000;
    double time_limit_seconds = 60.0;

    double integrality_tolerance = 1e-6;
    double feasibility_tolerance = 1e-7;
    double optimality_tolerance = 1e-7;

    bool enable_gomory_cuts = true;
    std::size_t max_cut_rounds = 5;

    BranchingRule branching_rule = BranchingRule::MostFractional;
    NodeSelection node_selection = NodeSelection::BestBound;

    bool enable_logging = true;
};

struct BranchAndBoundResult
{
    SolveStatus status = SolveStatus::NotSolved;

    double objective_value = std::numeric_limits<double>::infinity();
    double lower_bound = -std::numeric_limits<double>::infinity();
    double upper_bound = std::numeric_limits<double>::infinity();
    double duality_gap = 0.0;

    std::vector<double> solution;

    std::size_t nodes_processed = 0;
    std::size_t nodes_created = 0;
    std::size_t integer_nodes = 0;
    std::size_t cuts_generated = 0;

    double elapsed_seconds = 0.0;
    std::string message;
};

class BranchAndBoundSolver
{
public:
    BranchAndBoundSolver() = default;

    explicit BranchAndBoundSolver(BranchAndBoundOptions options);

    void set_options(const BranchAndBoundOptions& options);
    const BranchAndBoundOptions& options() const noexcept;

    // Solve MILP model with given variable integer/binary types
    BranchAndBoundResult solve(
        const lp::LPModel& base_model,
        const std::vector<core::VariableType>& var_types
    );

    void reset();

private:
    struct Node
    {
        std::uint64_t id = 0;
        std::size_t depth = 0;
        std::vector<double> lower_bounds;
        std::vector<double> upper_bounds;
        double bound = 0.0; // relaxed LP objective value
    };

    BranchAndBoundOptions options_{};
    BranchAndBoundResult result_{};
    std::uint64_t next_node_id_ = 0;

    bool is_integral(double value) const noexcept;

    int select_branch_variable(
        const std::vector<double>& solution,
        const std::vector<core::VariableType>& var_types
    ) const;

    bool generate_gomory_cut(
        const lp::LPModel& current_model,
        const std::vector<double>& solution,
        const std::vector<core::VariableType>& var_types,
        std::vector<double>& cut_coeffs,
        double& cut_rhs
    );
};

} // namespace sovopt::algorithms