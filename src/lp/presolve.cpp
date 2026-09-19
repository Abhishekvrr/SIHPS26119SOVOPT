#include "sovopt/lp/presolve.h"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace sovopt::lp
{

PresolveEngine::PresolveEngine(double tolerance)
    : tolerance_(tolerance)
{
}

bool PresolveEngine::presolve(
    const LPModel& original_model,
    LPModel& presolved_model,
    PresolveStats& stats
)
{
    const auto t_start = std::chrono::high_resolution_clock::now();

    const auto& vars = original_model.variables();
    const auto& cons = original_model.constraints();
    const auto& mat = original_model.matrix();

    stats.original_cols = vars.size();
    stats.original_rows = cons.size();

    col_mapping_.assign(vars.size(), -1);
    fixed_values_.assign(vars.size(), 0.0);

    // Copy variable bounds to working copy
    std::vector<double> lbs(vars.size());
    std::vector<double> ubs(vars.size());
    for (std::size_t j = 0; j < vars.size(); ++j)
    {
        lbs[j] = vars[j].lower_bound;
        ubs[j] = vars[j].upper_bound;
    }

    std::vector<bool> active_constraints(cons.size(), true);

    // Pass 1: Singleton Row Bound Tightening & Empty Row Check
    for (std::size_t i = 0; i < cons.size(); ++i)
    {
        std::size_t nz_count = 0;
        std::size_t last_j = 0;
        double last_coeff = 0.0;

        for (std::size_t j = 0; j < vars.size(); ++j)
        {
            double val = mat.get(i, j);
            if (std::abs(val) > tolerance_)
            {
                nz_count++;
                last_j = j;
                last_coeff = val;
            }
        }

        if (nz_count == 0)
        {
            // Empty row: check RHS feasibility
            if (cons[i].sense == ConstraintSense::LessEqual && cons[i].rhs < -tolerance_)
            {
                stats.is_infeasible = true;
                stats.message = "Presolve detected infeasible empty constraint row: " + cons[i].name;
                return false;
            }
            if (cons[i].sense == ConstraintSense::GreaterEqual && cons[i].rhs > tolerance_)
            {
                stats.is_infeasible = true;
                stats.message = "Presolve detected infeasible empty constraint row: " + cons[i].name;
                return false;
            }
            if (cons[i].sense == ConstraintSense::Equal && std::abs(cons[i].rhs) > tolerance_)
            {
                stats.is_infeasible = true;
                stats.message = "Presolve detected infeasible empty constraint row: " + cons[i].name;
                return false;
            }
            // Redundant empty row -> drop
            active_constraints[i] = false;
            stats.removed_rows++;
        }
        else if (nz_count == 1)
        {
            // Singleton row: coeff * x_j <=, >=, == rhs
            double b = cons[i].rhs;
            double a = last_coeff;
            double bound_val = b / a;

            if (cons[i].sense == ConstraintSense::Equal)
            {
                lbs[last_j] = std::max(lbs[last_j], bound_val);
                ubs[last_j] = std::min(ubs[last_j], bound_val);
            }
            else if ((cons[i].sense == ConstraintSense::LessEqual && a > 0) ||
                     (cons[i].sense == ConstraintSense::GreaterEqual && a < 0))
            {
                ubs[last_j] = std::min(ubs[last_j], bound_val);
            }
            else
            {
                lbs[last_j] = std::max(lbs[last_j], bound_val);
            }

            if (lbs[last_j] > ubs[last_j] + tolerance_)
            {
                stats.is_infeasible = true;
                stats.message = "Presolve detected contradictory bounds for variable " + vars[last_j].name;
                return false;
            }

            stats.tightened_bounds++;
            active_constraints[i] = false; // converted to direct variable bound
            stats.removed_rows++;
        }
    }

    // Pass 2: Detect Fixed Variables (lower == upper)
    int new_col_idx = 0;
    std::vector<std::size_t> active_var_indices;
    for (std::size_t j = 0; j < vars.size(); ++j)
    {
        if (std::abs(lbs[j] - ubs[j]) < tolerance_)
        {
            col_mapping_[j] = -1;
            fixed_values_[j] = lbs[j];
            stats.fixed_variables++;
        }
        else
        {
            col_mapping_[j] = new_col_idx++;
            active_var_indices.push_back(j);
        }
    }

    // If all variables are fixed or none reduced, construct presolved model
    presolved_model = LPModel(original_model.objective_sense());

    for (std::size_t orig_j : active_var_indices)
    {
        presolved_model.add_variable(
            vars[orig_j].name,
            lbs[orig_j],
            ubs[orig_j]
        );
    }

    // Set objectives for active variables
    for (std::size_t orig_j : active_var_indices)
    {
        int new_j = col_mapping_[orig_j];
        if (new_j >= 0)
        {
            presolved_model.set_objective(new_j, original_model.objective()[orig_j]);
        }
    }

    // Add reduced active constraints
    for (std::size_t i = 0; i < cons.size(); ++i)
    {
        if (!active_constraints[i]) continue;

        double shifted_rhs = cons[i].rhs;
        for (std::size_t j = 0; j < vars.size(); ++j)
        {
            if (col_mapping_[j] == -1) // fixed variable
            {
                shifted_rhs -= mat.get(i, j) * fixed_values_[j];
            }
        }

        std::size_t new_c_idx = presolved_model.add_constraint(
            cons[i].name,
            cons[i].sense,
            shifted_rhs
        );

        for (std::size_t orig_j : active_var_indices)
        {
            int new_j = col_mapping_[orig_j];
            double coeff = mat.get(i, orig_j);
            if (std::abs(coeff) > tolerance_)
            {
                presolved_model.set_coefficient(new_c_idx, new_j, coeff);
            }
        }
    }

    presolved_model.finalize();

    stats.presolved_cols = presolved_model.variables().size();
    stats.presolved_rows = presolved_model.constraints().size();

    const auto t_end = std::chrono::high_resolution_clock::now();
    stats.presolve_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    stats.message = "Presolve completed: removed " + std::to_string(stats.removed_rows) + " rows, fixed " +
                    std::to_string(stats.fixed_variables) + " vars, tightened " +
                    std::to_string(stats.tightened_bounds) + " bounds.";
    return true;
}

std::vector<double> PresolveEngine::postsolve(
    const LPModel& original_model,
    const LPModel& presolved_model,
    const std::vector<double>& presolved_solution
)
{
    std::vector<double> full_solution(original_model.variables().size(), 0.0);

    for (std::size_t j = 0; j < original_model.variables().size(); ++j)
    {
        int p_idx = (j < col_mapping_.size()) ? col_mapping_[j] : -1;
        if (p_idx >= 0 && static_cast<std::size_t>(p_idx) < presolved_solution.size())
        {
            full_solution[j] = presolved_solution[p_idx];
        }
        else if (j < fixed_values_.size())
        {
            full_solution[j] = fixed_values_[j];
        }
        else
        {
            full_solution[j] = original_model.variables()[j].lower_bound;
        }
    }

    (void)presolved_model;
    return full_solution;
}

} // namespace sovopt::lp
