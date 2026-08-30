#include "sovopt/lp/simplex_solver.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace sovopt::lp {

namespace {

constexpr double BIG_M = 1e12;

struct StandardVariable {
    int original_index = -1;
    double multiplier = 1.0;
};

} // namespace

SimplexSolver::SimplexSolver(
    sovopt::math::NumericalSettings settings
)
    : settings_(settings)
{
}

LPResult SimplexSolver::solve(
    const LPModel& model)
{
    const auto start = std::chrono::steady_clock::now();

    LPResult result = solve_standard(model);

    const auto end = std::chrono::steady_clock::now();

    result.solve_time_seconds =
        std::chrono::duration<double>(end - start).count();

    return result;
}

LPResult SimplexSolver::solve_standard(
    const LPModel& model)
{
    LPResult result;

    std::string error;

    if (!model.validate(error)) {
        result.status = LPStatus::InvalidModel;
        result.message = error;
        return result;
    }

    const auto& variables = model.variables();
    const auto& constraints = model.constraints();

    const std::size_t original_variables =
        variables.size();

    /*
     * Convert variable lower bounds:
     *
     *     x = lower + y
     *
     * where y >= 0.
     *
     * Upper bounds become:
     *
     *     y <= upper - lower
     */

    std::vector<double> shifted_rhs;

    for (const auto& constraint : constraints) {
        double rhs = constraint.rhs;

        for (std::size_t j = 0;
             j < original_variables;
             ++j) {
            rhs -=
                model.matrix().get(
                    shifted_rhs.size(),
                    j) *
                variables[j].lower_bound;
        }

        shifted_rhs.push_back(rhs);
    }

    /*
     * Build a canonical tableau.
     *
     * Columns:
     *
     * original shifted variables
     * slack/surplus variables
     * artificial variables
     */

    struct RowData {
        std::vector<double> coefficients;
        ConstraintSense sense;
        double rhs;
    };

    std::vector<RowData> rows;

    for (std::size_t i = 0;
         i < constraints.size();
         ++i) {

        RowData row;

        row.coefficients.resize(
            original_variables);

        for (std::size_t j = 0;
             j < original_variables;
             ++j) {

            row.coefficients[j] =
                model.matrix().get(i, j);
        }

        row.sense = constraints[i].sense;
        row.rhs = shifted_rhs[i];

        rows.push_back(std::move(row));
    }

    /*
     * Add upper-bound rows.
     */

    for (std::size_t j = 0;
         j < original_variables;
         ++j) {

        if (variables[j].upper_bound <
            math::infinity() / 2.0) {

            RowData row;

            row.coefficients.assign(
                original_variables,
                0.0);

            row.coefficients[j] = 1.0;

            row.sense = ConstraintSense::LessEqual;

            row.rhs =
                variables[j].upper_bound -
                variables[j].lower_bound;

            rows.push_back(std::move(row));
        }
    }

    /*
     * Ensure negative RHS rows are represented
     * with a non-negative RHS.
     */

    for (auto& row : rows) {

        if (row.rhs < -settings_.feasibility_tolerance) {

            row.rhs = -row.rhs;

            for (double& value : row.coefficients) {
                value = -value;
            }

            if (row.sense ==
                ConstraintSense::LessEqual) {

                row.sense =
                    ConstraintSense::GreaterEqual;

            } else if (row.sense ==
                       ConstraintSense::GreaterEqual) {

                row.sense =
                    ConstraintSense::LessEqual;
            }
        }
    }

    std::size_t slack_count = 0;
    std::size_t artificial_count = 0;

    for (const auto& row : rows) {

        if (row.sense ==
            ConstraintSense::LessEqual) {
            ++slack_count;

        } else if (row.sense ==
                   ConstraintSense::GreaterEqual) {

            ++slack_count;
            ++artificial_count;

        } else {
            ++artificial_count;
        }
    }

    const std::size_t total_columns =
        original_variables +
        slack_count +
        artificial_count;

    Tableau tableau;

    tableau.rows = rows.size();

    tableau.columns = total_columns;

    tableau.rhs_column = total_columns;

    tableau.a.assign(
        tableau.rows + 1,
        std::vector<double>(
            total_columns + 1,
            0.0));

    tableau.basis.assign(
        tableau.rows,
        std::numeric_limits<std::size_t>::max());

    tableau.artificial.assign(
        total_columns,
        false);

    tableau.original_variable.assign(
        total_columns,
        -1);

    for (std::size_t j = 0;
         j < original_variables;
         ++j) {

        tableau.original_variable[j] =
            static_cast<int>(j);
    }

    std::size_t slack_column =
        original_variables;

    std::size_t artificial_column =
        original_variables + slack_count;

    for (std::size_t i = 0;
         i < rows.size();
         ++i) {

        const auto& row = rows[i];

        for (std::size_t j = 0;
             j < original_variables;
             ++j) {

            tableau.a[i][j] =
                row.coefficients[j];
        }

        if (row.sense ==
            ConstraintSense::LessEqual) {

            tableau.a[i][slack_column] = 1.0;

            tableau.basis[i] = slack_column;

            ++slack_column;

        } else if (row.sense ==
                   ConstraintSense::GreaterEqual) {

            tableau.a[i][slack_column] = -1.0;

            ++slack_column;

            tableau.a[i][artificial_column] = 1.0;

            tableau.basis[i] = artificial_column;

            tableau.artificial[artificial_column] =
                true;

            ++artificial_column;

        } else {

            tableau.a[i][artificial_column] = 1.0;

            tableau.basis[i] = artificial_column;

            tableau.artificial[artificial_column] =
                true;

            ++artificial_column;
        }

        tableau.a[i][tableau.rhs_column] =
            row.rhs;
    }

    /*
     * Phase I:
     *
     * Minimize sum of artificial variables.
     *
     * Internally simplex maximizes, therefore
     * artificial objective coefficients are -1.
     */

    std::vector<double> phase_one_objective(
        total_columns,
        0.0);

    for (std::size_t j = 0;
         j < total_columns;
         ++j) {

        if (tableau.artificial[j]) {
            phase_one_objective[j] = -1.0;
        }
    }

    /*
     * Put the objective into canonical form by
     * eliminating coefficients of basic variables.
     */

    for (std::size_t i = 0;
         i < tableau.rows;
         ++i) {

        const std::size_t basic =
            tableau.basis[i];

        if (basic < total_columns &&
            tableau.artificial[basic]) {

            for (std::size_t j = 0;
                 j < total_columns;
                 ++j) {

                phase_one_objective[j] +=
                    tableau.a[i][j];
            }
        }
    }

    std::size_t iterations = 0;
    bool unbounded = false;

    if (!run_simplex(
            tableau,
            phase_one_objective,
            iterations,
            unbounded)) {

        result.status = LPStatus::IterationLimit;
        result.iterations = iterations;
        result.message =
            "Phase I reached the iteration limit.";

        return result;
    }

    if (unbounded) {
        result.status = LPStatus::NumericalError;
        result.iterations = iterations;
        result.message =
            "Unexpected unbounded Phase I problem.";

        return result;
    }

    /*
     * Phase I objective is the negative artificial sum.
     *
     * Therefore zero means feasible.
     */

    double artificial_sum = 0.0;

    for (std::size_t i = 0;
         i < tableau.rows;
         ++i) {

        const auto basic =
            tableau.basis[i];

        if (basic < total_columns &&
            tableau.artificial[basic]) {

            artificial_sum +=
                tableau.a[i][tableau.rhs_column];
        }
    }

    if (artificial_sum >
        settings_.feasibility_tolerance) {

        result.status = LPStatus::Infeasible;
        result.iterations = iterations;
        result.message =
            "No feasible solution exists.";

        return result;
    }

    /*
     * Remove artificial variables from the
     * active problem.
     *
     * Instead of physically rebuilding the matrix,
     * mark artificial columns with zero objective
     * and prevent them from entering.
     */

    std::vector<double> phase_two_objective(
        total_columns,
        0.0);

    for (std::size_t j = 0;
         j < original_variables;
         ++j) {

        double coefficient =
            model.objective()[j];

        if (model.objective_sense() ==
            ObjectiveSense::Minimize) {

            coefficient = -coefficient;
        }

        phase_two_objective[j] = coefficient;
    }

    /*
     * Canonicalize phase II objective.
     */

    for (std::size_t i = 0;
         i < tableau.rows;
         ++i) {

        const auto basic =
            tableau.basis[i];

        if (basic >= total_columns) {
            continue;
        }

        const double coefficient =
            phase_two_objective[basic];

        if (std::abs(coefficient) <=
            settings_.zero_tolerance) {
            continue;
        }

        for (std::size_t j = 0;
             j < total_columns;
             ++j) {

            phase_two_objective[j] -=
                coefficient *
                tableau.a[i][j];
        }
    }

    /*
     * Prevent artificial columns from entering.
     */

    for (std::size_t j = 0;
         j < total_columns;
         ++j) {

        if (tableau.artificial[j]) {
            phase_two_objective[j] = 0.0;
        }
    }

    if (!run_simplex(
            tableau,
            phase_two_objective,
            iterations,
            unbounded)) {

        result.status = LPStatus::IterationLimit;
        result.iterations = iterations;
        result.message =
            "Phase II reached the iteration limit.";

        return result;
    }

    if (unbounded) {
        result.status = LPStatus::Unbounded;
        result.iterations = iterations;
        result.message =
            "LP objective is unbounded.";

        return result;
    }

    /*
     * Extract original variables.
     */

    result.solution.assign(
        original_variables,
        0.0);

    for (std::size_t i = 0;
         i < tableau.rows;
         ++i) {

        const std::size_t basic =
            tableau.basis[i];

        if (basic >= total_columns) {
            continue;
        }

        const int original =
            tableau.original_variable[basic];

        if (original >= 0) {

            result.solution[
                static_cast<std::size_t>(original)] =
                tableau.a[i][tableau.rhs_column];
        }
    }

    /*
     * Restore original lower bounds.
     */

    for (std::size_t j = 0;
         j < original_variables;
         ++j) {

        result.solution[j] +=
            variables[j].lower_bound;

        if (std::abs(result.solution[j]) <=
            settings_.zero_tolerance) {

            result.solution[j] = 0.0;
        }
    }

    double residual = 0.0;

    if (!validate_solution(
            model,
            result.solution,
            residual)) {

        result.status = LPStatus::NumericalError;
        result.iterations = iterations;
        result.primal_residual = residual;
        result.message =
            "Numerical validation failed.";

        return result;
    }

    result.objective_value =
        calculate_objective(
            model,
            result.solution);

    result.primal_residual = residual;

    result.iterations = iterations;

    result.status = LPStatus::Optimal;

    result.message =
        "Optimal solution found by two-phase simplex.";

    return result;
}

bool SimplexSolver::pivot(
    Tableau& tableau,
    std::size_t row,
    std::size_t column)
{
    if (row >= tableau.rows ||
        column >= tableau.columns) {
        return false;
    }

    const double pivot_value =
        tableau.a[row][column];

    if (std::abs(pivot_value) <
        settings_.zero_tolerance) {
        return false;
    }

    const double inverse =
        1.0 / pivot_value;

    for (std::size_t j = 0;
         j <= tableau.columns;
         ++j) {

        tableau.a[row][j] *= inverse;
    }

    for (std::size_t i = 0;
         i <= tableau.rows;
         ++i) {

        if (i == row) {
            continue;
        }

        const double factor =
            tableau.a[i][column];

        if (std::abs(factor) <=
            settings_.zero_tolerance) {
            continue;
        }

        for (std::size_t j = 0;
             j <= tableau.columns;
             ++j) {

            tableau.a[i][j] -=
                factor *
                tableau.a[row][j];
        }
    }

    tableau.basis[row] = column;

    return true;
}

int SimplexSolver::choose_entering_variable(
    const Tableau& tableau,
    const std::vector<double>& objective) const
{
    int best = -1;

    double best_value =
        settings_.optimality_tolerance;

    for (std::size_t j = 0;
         j < tableau.columns;
         ++j) {

        if (tableau.artificial[j]) {
            continue;
        }

        if (objective[j] >
            best_value) {

            best_value = objective[j];
            best = static_cast<int>(j);
        }
    }

    return best;
}

int SimplexSolver::choose_leaving_row(
    const Tableau& tableau,
    std::size_t entering) const
{
    int best_row = -1;

    double best_ratio =
        std::numeric_limits<double>::infinity();

    for (std::size_t i = 0;
         i < tableau.rows;
         ++i) {

        const double coefficient =
            tableau.a[i][entering];

        if (coefficient <=
            settings_.zero_tolerance) {
            continue;
        }

        const double rhs =
            tableau.a[i][tableau.rhs_column];

        const double ratio =
            rhs / coefficient;

        if (ratio < best_ratio -
            settings_.feasibility_tolerance) {

            best_ratio = ratio;

            best_row =
                static_cast<int>(i);

        } else if (
            std::abs(ratio - best_ratio) <=
            settings_.feasibility_tolerance) {

            /*
             * Bland-style tie breaking.
             */

            if (best_row < 0 ||
                tableau.basis[i] <
                tableau.basis[
                    static_cast<std::size_t>(
                        best_row)]) {

                best_row =
                    static_cast<int>(i);
            }
        }
    }

    return best_row;
}

bool SimplexSolver::run_simplex(
    Tableau& tableau,
    std::vector<double>& objective,
    std::size_t& iterations,
    bool& unbounded)
{
    unbounded = false;

    while (iterations <
           settings_.max_iterations) {

        const int entering =
            choose_entering_variable(
                tableau,
                objective);

        if (entering < 0) {
            return true;
        }

        const int leaving =
            choose_leaving_row(
                tableau,
                static_cast<std::size_t>(
                    entering));

        if (leaving < 0) {
            unbounded = true;
            return true;
        }

        if (!pivot(
                tableau,
                static_cast<std::size_t>(leaving),
                static_cast<std::size_t>(entering))) {

            return false;
        }

        /*
         * Rebuild objective row after pivot.
         */

        const double pivot_objective =
            objective[
                static_cast<std::size_t>(entering)];

        if (std::abs(pivot_objective) >
            settings_.zero_tolerance) {

            const std::size_t row =
                static_cast<std::size_t>(leaving);

            for (std::size_t j = 0;
                 j < tableau.columns;
                 ++j) {

                objective[j] -=
                    pivot_objective *
                    tableau.a[row][j];
            }
        }

        /*
         * The objective vector above is maintained
         * separately. Reconstruct it from the
         * current basis to avoid accumulated
         * reduced-cost drift.
         */

        std::vector<double> reconstructed =
            objective;

        for (std::size_t i = 0;
             i < tableau.rows;
             ++i) {

            const std::size_t basic =
                tableau.basis[i];

            if (basic >= tableau.columns) {
                continue;
            }

            const double coefficient =
                reconstructed[basic];

            if (std::abs(coefficient) <=
                settings_.zero_tolerance) {
                continue;
            }

            for (std::size_t j = 0;
                 j < tableau.columns;
                 ++j) {

                reconstructed[j] -=
                    coefficient *
                    tableau.a[i][j];
            }
        }

        objective.swap(reconstructed);

        ++iterations;
    }

    return false;
}


double SimplexSolver::calculate_objective(
    const LPModel& model,
    const std::vector<double>& solution) const
{
    double value = 0.0;

    for (std::size_t j = 0;
         j < solution.size();
         ++j) {

        value +=
            model.objective()[j] *
            solution[j];
    }

    return value;
}

bool SimplexSolver::validate_solution(
    const LPModel& model,
    const std::vector<double>& solution,
    double& residual) const
{
    residual = 0.0;

    if (solution.size() !=
        model.variables().size()) {
        return false;
    }

    for (std::size_t j = 0;
         j < solution.size();
         ++j) {

        const auto& variable =
            model.variables()[j];

        if (solution[j] <
            variable.lower_bound -
            settings_.feasibility_tolerance) {

            residual = std::max(
                residual,
                variable.lower_bound -
                solution[j]);

            return false;
        }

        if (solution[j] >
            variable.upper_bound +
            settings_.feasibility_tolerance) {

            residual = std::max(
                residual,
                solution[j] -
                variable.upper_bound);

            return false;
        }
    }

    for (std::size_t i = 0;
         i < model.constraints().size();
         ++i) {

        double lhs = 0.0;

        for (std::size_t j = 0;
             j < solution.size();
             ++j) {

            lhs +=
                model.matrix().get(i, j) *
                solution[j];
        }

        const auto& constraint =
            model.constraints()[i];

        double violation = 0.0;

        if (constraint.sense ==
            ConstraintSense::LessEqual) {

            violation =
                std::max(
                    0.0,
                    lhs - constraint.rhs);

        } else if (
            constraint.sense ==
            ConstraintSense::GreaterEqual) {

            violation =
                std::max(
                    0.0,
                    constraint.rhs - lhs);

        } else {

            violation =
                std::abs(
                    lhs - constraint.rhs);
        }

        residual =
            std::max(
                residual,
                violation);

        if (violation >
            settings_.feasibility_tolerance) {

            return false;
        }
    }

    return true;
}

} // namespace sovopt::lp




