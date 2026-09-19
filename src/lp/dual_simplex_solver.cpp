#include "sovopt/lp/dual_simplex_solver.h"
#include "sovopt/math/simd_vector.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>

namespace sovopt::lp
{

DualSimplexSolver::DualSimplexSolver(sovopt::math::NumericalSettings settings)
    : settings_(settings)
{
}

LPResult DualSimplexSolver::solve(const LPModel& model)
{
    const auto start = std::chrono::steady_clock::now();
    LPResult result;

    std::string error;
    if (!model.validate(error))
    {
        result.status = LPStatus::InvalidModel;
        result.message = error;
        return result;
    }

    const auto& vars = model.variables();
    const auto& cons = model.constraints();
    const auto& mat = model.matrix();
    const std::size_t n_vars = vars.size();
    const std::size_t n_cons = cons.size();

    // Standard Dual Simplex canonical tableau setup
    DualTableau tab;
    tab.rows = n_cons;
    tab.cols = n_vars + n_cons;
    tab.rhs_col = tab.cols;
    tab.a.assign(tab.rows + 1, std::vector<double>(tab.cols + 1, 0.0));
    tab.basis.resize(tab.rows);
    tab.original_variable.assign(tab.cols, -1);

    for (std::size_t j = 0; j < n_vars; ++j)
    {
        tab.original_variable[j] = static_cast<int>(j);
    }

    // Populate constraint rows
    for (std::size_t i = 0; i < n_cons; ++i)
    {
        for (std::size_t j = 0; j < n_vars; ++j)
        {
            tab.a[i][j] = mat.get(i, j);
        }

        // Slack/Surplus
        if (cons[i].sense == ConstraintSense::LessEqual)
        {
            tab.a[i][n_vars + i] = 1.0;
        }
        else if (cons[i].sense == ConstraintSense::GreaterEqual)
        {
            // multiply by -1 to have positive slack
            for (std::size_t j = 0; j < n_vars; ++j)
            {
                tab.a[i][j] = -tab.a[i][j];
            }
            tab.a[i][n_vars + i] = 1.0;
        }
        else // Equal
        {
            tab.a[i][n_vars + i] = 1.0;
        }

        tab.basis[i] = n_vars + i;

        double rhs_val = cons[i].rhs;
        if (cons[i].sense == ConstraintSense::GreaterEqual)
        {
            rhs_val = -rhs_val;
        }
        tab.a[i][tab.rhs_col] = rhs_val;
    }

    // Objective row (min c^T x)
    const bool is_max = (model.objective_sense() == ObjectiveSense::Maximize);
    for (std::size_t j = 0; j < n_vars; ++j)
    {
        double c = model.objective()[j];
        tab.a[tab.rows][j] = is_max ? -c : c;
    }
    tab.a[tab.rows][tab.rhs_col] = 0.0;

    std::size_t iterations = 0;
    bool infeasible = false;
    bool solved = run_dual_simplex(tab, iterations, infeasible);

    const auto end = std::chrono::steady_clock::now();
    result.solve_time_seconds = std::chrono::duration<double>(end - start).count();
    result.iterations = iterations;

    if (infeasible)
    {
        result.status = LPStatus::Infeasible;
        result.message = "Dual Simplex detected primal infeasibility (dual unbounded).";
        return result;
    }

    if (!solved)
    {
        result.status = LPStatus::IterationLimit;
        result.message = "Dual Simplex reached iteration limit.";
        return result;
    }

    // Extract solution
    result.status = LPStatus::Optimal;
    result.solution.assign(n_vars, 0.0);

    for (std::size_t i = 0; i < tab.rows; ++i)
    {
        std::size_t b_var = tab.basis[i];
        if (b_var < tab.cols && tab.original_variable[b_var] >= 0)
        {
            std::size_t orig_idx = static_cast<std::size_t>(tab.original_variable[b_var]);
            if (orig_idx < n_vars)
            {
                result.solution[orig_idx] = tab.a[i][tab.rhs_col];
            }
        }
    }

    // Compute objective value
    double total_obj = 0.0;
    for (std::size_t j = 0; j < n_vars; ++j)
    {
        total_obj += model.objective()[j] * result.solution[j];
    }
    result.objective_value = total_obj;
    result.message = "Optimal solution found by Dual Simplex.";
    return result;
}

bool DualSimplexSolver::run_dual_simplex(DualTableau& tab, std::size_t& iterations, bool& infeasible)
{
    while (iterations < settings_.max_iterations)
    {
        // 1. Choose leaving row (most negative primal RHS)
        int leaving_row = choose_leaving_row(tab);
        if (leaving_row < 0)
        {
            // All RHS >= 0 -> Primal feasible & Dual feasible -> OPTIMAL
            return true;
        }

        // 2. Choose entering column using Dual Ratio Test
        int entering_col = choose_entering_column_dual(tab, static_cast<std::size_t>(leaving_row));
        if (entering_col < 0)
        {
            // No negative entry in pivot row -> Primal Infeasible
            infeasible = true;
            return false;
        }

        // 3. Pivot
        if (!pivot(tab, static_cast<std::size_t>(leaving_row), static_cast<std::size_t>(entering_col)))
        {
            return false;
        }

        iterations++;
    }
    return false;
}

int DualSimplexSolver::choose_leaving_row(const DualTableau& tab) const
{
    int best_row = -1;
    double min_rhs = -settings_.feasibility_tolerance;

    for (std::size_t i = 0; i < tab.rows; ++i)
    {
        double rhs = tab.a[i][tab.rhs_col];
        if (rhs < min_rhs)
        {
            min_rhs = rhs;
            best_row = static_cast<int>(i);
        }
    }
    return best_row;
}

int DualSimplexSolver::choose_entering_column_dual(const DualTableau& tab, std::size_t leaving_row) const
{
    int best_col = -1;
    double min_ratio = std::numeric_limits<double>::infinity();

    for (std::size_t j = 0; j < tab.cols; ++j)
    {
        double a_rj = tab.a[leaving_row][j];
        if (a_rj < -settings_.pivot_tolerance)
        {
            double reduced_cost = tab.a[tab.rows][j];
            if (reduced_cost < -settings_.optimality_tolerance)
            {
                reduced_cost = 0.0;
            }
            double ratio = std::abs(reduced_cost / a_rj);
            if (ratio < min_ratio)
            {
                min_ratio = ratio;
                best_col = static_cast<int>(j);
            }
        }
    }
    return best_col;
}

bool DualSimplexSolver::pivot(DualTableau& tab, std::size_t row, std::size_t col)
{
    double pivot_val = tab.a[row][col];
    if (std::abs(pivot_val) < settings_.pivot_tolerance) return false;

    double inv_pivot = 1.0 / pivot_val;

    // Scale pivot row using SIMD
    sovopt::math::SIMDVector::scale(tab.a[row].data(), inv_pivot, tab.cols + 1);
    tab.a[row][col] = 1.0;

    // Eliminate other rows using SIMD row operations
    for (std::size_t i = 0; i <= tab.rows; ++i)
    {
        if (i != row)
        {
            double factor = tab.a[i][col];
            if (std::abs(factor) > settings_.zero_tolerance)
            {
                sovopt::math::SIMDVector::pivot_row_elimination(
                    tab.a[i].data(),
                    tab.a[row].data(),
                    factor,
                    tab.cols + 1
                );
                tab.a[i][col] = 0.0;
            }
        }
    }

    tab.basis[row] = col;
    return true;
}

} // namespace sovopt::lp
