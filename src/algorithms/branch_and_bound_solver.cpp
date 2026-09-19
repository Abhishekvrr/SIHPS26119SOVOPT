#include "sovopt/algorithms/branch_and_bound_solver.h"
#include "sovopt/lp/simplex_solver.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <queue>

namespace sovopt::algorithms
{

BranchAndBoundSolver::BranchAndBoundSolver(BranchAndBoundOptions options)
    : options_(std::move(options))
{
}

void BranchAndBoundSolver::set_options(const BranchAndBoundOptions& options)
{
    options_ = options;
}

const BranchAndBoundOptions& BranchAndBoundSolver::options() const noexcept
{
    return options_;
}

void BranchAndBoundSolver::reset()
{
    result_ = BranchAndBoundResult{};
    next_node_id_ = 0;
}

bool BranchAndBoundSolver::is_integral(double value) const noexcept
{
    const double rounded = std::round(value);
    return std::abs(value - rounded) <= options_.integrality_tolerance;
}

int BranchAndBoundSolver::select_branch_variable(
    const std::vector<double>& solution,
    const std::vector<core::VariableType>& var_types
) const
{
    int best_var = -1;
    double max_fractionality = 0.0;

    for (std::size_t j = 0; j < solution.size(); ++j)
    {
        if (j >= var_types.size()) break;
        if (var_types[j] == core::VariableType::Integer ||
            var_types[j] == core::VariableType::Binary)
        {
            const double val = solution[j];
            if (!is_integral(val))
            {
                const double fractionality = std::abs(val - std::floor(val) - 0.5);
                const double score = 0.5 - fractionality;
                if (score > max_fractionality)
                {
                    max_fractionality = score;
                    best_var = static_cast<int>(j);
                }
            }
        }
    }
    return best_var;
}

bool BranchAndBoundSolver::generate_gomory_cut(
    const lp::LPModel& current_model,
    const std::vector<double>& solution,
    const std::vector<core::VariableType>& var_types,
    std::vector<double>& cut_coeffs,
    double& cut_rhs
)
{
    const auto& vars = current_model.variables();
    const auto& cons = current_model.constraints();
    const auto& mat = current_model.matrix();
    const std::size_t n_vars = vars.size();

    // Check binding constraints for fractional RHS terms
    for (std::size_t i = 0; i < cons.size(); ++i)
    {
        double rhs = cons[i].rhs;
        double f0 = rhs - std::floor(rhs);

        if (f0 > 1e-4 && f0 < 0.9996)
        {
            cut_coeffs.assign(n_vars, 0.0);
            bool has_non_zero = false;

            for (std::size_t j = 0; j < n_vars; ++j)
            {
                if (j < var_types.size() &&
                    (var_types[j] == core::VariableType::Integer || var_types[j] == core::VariableType::Binary))
                {
                    double a_ij = mat.get(i, j);
                    double fj = a_ij - std::floor(a_ij);
                    if (fj > 1e-6)
                    {
                        cut_coeffs[j] = fj;
                        has_non_zero = true;
                    }
                }
            }

            if (has_non_zero)
            {
                cut_rhs = f0;
                return true;
            }
        }
    }

    (void)solution;
    return false;
}

BranchAndBoundResult BranchAndBoundSolver::solve(
    const lp::LPModel& base_model,
    const std::vector<core::VariableType>& var_types
)
{
    const auto start_time = std::chrono::steady_clock::now();
    reset();

    const bool is_maximize = (base_model.objective_sense() == lp::ObjectiveSense::Maximize);
    const std::size_t n_vars = base_model.variables().size();

    double best_obj = is_maximize ? -std::numeric_limits<double>::infinity()
                                  : std::numeric_limits<double>::infinity();
    std::vector<double> best_solution(n_vars, 0.0);
    bool found_feasible = false;

    auto node_cmp = [is_maximize](const Node& a, const Node& b) {
        return is_maximize ? (a.bound < b.bound) : (a.bound > b.bound);
    };
    std::priority_queue<Node, std::vector<Node>, decltype(node_cmp)> node_queue(node_cmp);

    Node root_node;
    root_node.id = next_node_id_++;
    root_node.depth = 0;
    root_node.lower_bounds.resize(n_vars);
    root_node.upper_bounds.resize(n_vars);

    for (std::size_t j = 0; j < n_vars; ++j)
    {
        root_node.lower_bounds[j] = base_model.variables()[j].lower_bound;
        root_node.upper_bounds[j] = base_model.variables()[j].upper_bound;

        if (j < var_types.size() && var_types[j] == core::VariableType::Binary)
        {
            root_node.lower_bounds[j] = std::max(0.0, root_node.lower_bounds[j]);
            root_node.upper_bounds[j] = std::min(1.0, root_node.upper_bounds[j]);
        }
    }
    root_node.bound = is_maximize ? std::numeric_limits<double>::infinity()
                                  : -std::numeric_limits<double>::infinity();

    node_queue.push(root_node);
    result_.nodes_created = 1;

    sovopt::math::NumericalSettings num_settings;
    num_settings.feasibility_tolerance = options_.feasibility_tolerance;
    num_settings.optimality_tolerance = options_.optimality_tolerance;
    lp::SimplexSolver lp_solver(num_settings);

    while (!node_queue.empty() && result_.nodes_processed < options_.node_limit)
    {
        const auto now = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double>(now - start_time).count();
        if (elapsed > options_.time_limit_seconds)
        {
            result_.status = found_feasible ? SolveStatus::Feasible : SolveStatus::TimeLimit;
            result_.message = "Branch-and-Bound reached execution time limit.";
            break;
        }

        Node current_node = node_queue.top();
        node_queue.pop();
        result_.nodes_processed++;

        // Pruning by bound
        if (found_feasible)
        {
            if (is_maximize && current_node.bound <= best_obj + options_.optimality_tolerance)
            {
                continue;
            }
            if (!is_maximize && current_node.bound >= best_obj - options_.optimality_tolerance)
            {
                continue;
            }
        }

        // Build LP relaxation for this node
        lp::LPModel node_model = base_model;
        for (std::size_t j = 0; j < n_vars; ++j)
        {
            node_model.set_variable_bounds(
                j,
                current_node.lower_bounds[j],
                current_node.upper_bounds[j]
            );
        }

        // Cutting plane round (Branch-and-Cut)
        if (options_.enable_gomory_cuts && current_node.depth == 0)
        {
            for (std::size_t cut_round = 0; cut_round < options_.max_cut_rounds; ++cut_round)
            {
                lp::LPResult cut_lp = lp_solver.solve(node_model);
                if (cut_lp.status != lp::LPStatus::Optimal) break;

                std::vector<double> cut_coeffs;
                double cut_rhs = 0.0;
                if (generate_gomory_cut(node_model, cut_lp.solution, var_types, cut_coeffs, cut_rhs))
                {
                    std::size_t c_idx = node_model.add_constraint(
                        "GomoryCut_" + std::to_string(result_.cuts_generated + 1),
                        lp::ConstraintSense::GreaterEqual,
                        cut_rhs
                    );
                    for (std::size_t k = 0; k < n_vars; ++k)
                    {
                        if (std::abs(cut_coeffs[k]) > 1e-9)
                        {
                            node_model.set_coefficient(c_idx, k, cut_coeffs[k]);
                        }
                    }
                    node_model.finalize();
                    result_.cuts_generated++;
                }
                else
                {
                    break;
                }
            }
        }

        // Solve node LP relaxation
        lp::LPResult lp_res = lp_solver.solve(node_model);

        if (lp_res.status != lp::LPStatus::Optimal)
        {
            // Node is infeasible or unbounded -> prune
            continue;
        }

        const double node_obj = lp_res.objective_value;

        // Pruning by bound check
        if (found_feasible)
        {
            if (is_maximize && node_obj <= best_obj + options_.optimality_tolerance)
            {
                continue;
            }
            if (!is_maximize && node_obj >= best_obj - options_.optimality_tolerance)
            {
                continue;
            }
        }

        // Check if solution satisfies all integer constraints
        int branch_var = select_branch_variable(lp_res.solution, var_types);

        if (branch_var < 0)
        {
            // Integer feasible incumbent found!
            result_.integer_nodes++;
            if (!found_feasible ||
                (is_maximize && node_obj > best_obj) ||
                (!is_maximize && node_obj < best_obj))
            {
                best_obj = node_obj;
                best_solution = lp_res.solution;
                found_feasible = true;
            }
            continue;
        }

        // Branch on selected fractional integer variable
        const double frac_val = lp_res.solution[branch_var];
        const double floor_val = std::floor(frac_val);
        const double ceil_val = std::ceil(frac_val);

        // Left Child (x_j <= floor(x_j))
        if (floor_val >= current_node.lower_bounds[branch_var])
        {
            Node left_child = current_node;
            left_child.id = next_node_id_++;
            left_child.depth = current_node.depth + 1;
            left_child.upper_bounds[branch_var] = floor_val;
            left_child.bound = node_obj;
            node_queue.push(left_child);
            result_.nodes_created++;
        }

        // Right Child (x_j >= ceil(x_j))
        if (ceil_val <= current_node.upper_bounds[branch_var])
        {
            Node right_child = current_node;
            right_child.id = next_node_id_++;
            right_child.depth = current_node.depth + 1;
            right_child.lower_bounds[branch_var] = ceil_val;
            right_child.bound = node_obj;
            node_queue.push(right_child);
            result_.nodes_created++;
        }
    }

    const auto finish_time = std::chrono::steady_clock::now();
    result_.elapsed_seconds = std::chrono::duration<double>(finish_time - start_time).count();

    if (found_feasible)
    {
        result_.status = SolveStatus::Optimal;
        result_.objective_value = best_obj;
        result_.solution = best_solution;
        result_.duality_gap = 0.0;
        result_.message = "Global MILP optimal integer solution verified (" +
                          std::to_string(result_.nodes_processed) + " nodes explored, " +
                          std::to_string(result_.cuts_generated) + " Gomory cuts generated).";
    }
    else if (result_.status == SolveStatus::NotSolved)
    {
        result_.status = SolveStatus::Infeasible;
        result_.message = "MILP problem is integer infeasible across all branch nodes.";
    }

    return result_;
}

} // namespace sovopt::algorithms
