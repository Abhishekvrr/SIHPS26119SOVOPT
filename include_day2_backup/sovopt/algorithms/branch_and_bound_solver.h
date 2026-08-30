#pragma once

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

struct BranchAndBoundOptions
{
    std::size_t node_limit = 100000;
    double time_limit_seconds = 60.0;

    double integrality_tolerance = 1e-7;
    double feasibility_tolerance = 1e-7;

    bool enable_logging = true;
};

struct BranchAndBoundResult
{
    SolveStatus status = SolveStatus::NotSolved;

    double objective_value =
        std::numeric_limits<double>::infinity();

    std::vector<double> solution;

    std::size_t nodes_processed = 0;
    std::size_t nodes_created = 0;
    std::size_t integer_nodes = 0;

    double elapsed_seconds = 0.0;

    std::string message;
};

class BranchAndBoundSolver
{
public:

    BranchAndBoundSolver() = default;

    explicit BranchAndBoundSolver(
        BranchAndBoundOptions options);

    void set_options(
        const BranchAndBoundOptions& options);

    const BranchAndBoundOptions& options() const noexcept;

    BranchAndBoundResult solve();

    void reset();

private:

    struct Node
    {
        std::vector<double> lower_bounds;
        std::vector<double> upper_bounds;

        std::size_t depth = 0;

        double lower_bound =
            -std::numeric_limits<double>::infinity();

        std::uint64_t id = 0;
    };

    BranchAndBoundOptions options_{};

    BranchAndBoundResult result_{};

    std::uint64_t next_node_id_ = 0;

    std::vector<Node> open_nodes_;

    bool is_integral(double value) const noexcept;

    int select_branch_variable(
        const std::vector<double>& solution) const;

    void create_child_nodes(
        const Node& parent,
        int branch_variable,
        double branch_value);

    bool should_stop() const noexcept;
};

} // namespace sovopt::algorithms