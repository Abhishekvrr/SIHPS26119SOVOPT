#include "sovopt/lp/lp_solver.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace sovopt::lp
{

namespace
{

constexpr double EPSILON = 1e-12;

struct Tableau
{
    std::size_t rows{};
    std::size_t columns{};

    std::vector<std::vector<double>> a;

    std::vector<int> basis;
};

bool approximately_zero(
    double value,
    double tolerance
)
{
    return std::abs(value) <= tolerance;
}

} // namespace

// ============================================================
// LPModel
// ============================================================

void LPModel::resize_variables(
    std::size_t count
)
{
    variable_count = count;
    objective.assign(count, 0.0);
}

void LPModel::set_objective(
    const std::vector<double>& coefficients
)
{
    if (variable_count == 0)
    {
        resize_variables(coefficients.size());
    }

    if (coefficients.size() != variable_count)
    {
        throw std::invalid_argument(
            "Objective coefficient count does not match variable count."
        );
    }

    objective = coefficients;
}

void LPModel::add_constraint(
    const std::vector<double>& coefficients,
    ConstraintSense sense,
    double rhs
)
{
    if (variable_count == 0)
    {
        resize_variables(coefficients.size());
    }

    if (coefficients.size() != variable_count)
    {
        throw std::invalid_argument(
            "Constraint coefficient count does not match variable count."
        );
    }

    constraints.push_back(
        LPConstraint{
            coefficients,
            sense,
            rhs
        }
    );
}

// ============================================================
// SimplexSolver
// ============================================================

SimplexSolver::SimplexSolver(
    Options options
)
    : options_(options)
{
}

// ------------------------------------------------------------
// Public solve
// ------------------------------------------------------------

LPResult SimplexSolver::solve(
    const LPModel& model
) const
{
    try
    {
        if (model.variable_count == 0)
        {
            return {
                LPStatus::NumericalError,
                0.0,
                {},
                0,
                "LP contains no variables."
            };
        }

        if (model.objective.size() != model.variable_count)
        {
            return {
                LPStatus::NumericalError,
                0.0,
                {},
                0,
                "Objective dimension mismatch."
            };
        }

        for (const auto& constraint : model.constraints)
        {
            if (constraint.coefficients.size()
                != model.variable_count)
            {
                return {
                    LPStatus::NumericalError,
                    0.0,
                    {},
                    0,
                    "Constraint dimension mismatch."
                };
            }
        }

        return solve_maximization(model);
    }
    catch (const std::exception& exception)
    {
        return {
            LPStatus::NumericalError,
            0.0,
            {},
            0,
            exception.what()
        };
    }
}

// ============================================================
// Simplex implementation
//
// Canonical problem:
//
//      maximize     c^T x
//
//      subject to   A x <= b
//
//                   x >= 0
//
// For the initial prototype, constraints with >= or =
// are rejected explicitly instead of silently producing
// an incorrect mathematical model.
// ============================================================

LPResult SimplexSolver::solve_maximization(
    const LPModel& model
) const
{
    const std::size_t n = model.variable_count;
    const std::size_t m = model.constraints.size();

    if (m == 0)
    {
        bool positive_direction = false;

        for (double coefficient : model.objective)
        {
            if (coefficient > options_.tolerance)
            {
                positive_direction = true;
                break;
            }
        }

        if (positive_direction)
        {
            return {
                LPStatus::Unbounded,
                0.0,
                std::vector<double>(n, 0.0),
                0,
                "Objective can increase without bound."
            };
        }

        return {
            LPStatus::Optimal,
            0.0,
            std::vector<double>(n, 0.0),
            0,
            "Optimal solution found."
        };
    }

    for (const auto& constraint : model.constraints)
    {
        if (constraint.sense != ConstraintSense::LessEqual)
        {
            return {
                LPStatus::NumericalError,
                0.0,
                {},
                0,
                "Initial simplex core currently requires <= constraints."
            };
        }
    }

    // --------------------------------------------------------
    // Build tableau
    //
    // [ A | I | b ]
    // [ c | 0 | 0 ]
    //
    // Slack variables create the initial basis.
    // --------------------------------------------------------

    const std::size_t slack_count = m;
    const std::size_t total_variables = n + slack_count;

    Tableau tableau;

    tableau.rows = m + 1;
    tableau.columns = total_variables + 1;

    tableau.a.assign(
        tableau.rows,
        std::vector<double>(
            tableau.columns,
            0.0
        )
    );

    tableau.basis.resize(m);

    // --------------------------------------------------------
    // Constraint rows
    // --------------------------------------------------------

    for (std::size_t i = 0; i < m; ++i)
    {
        const auto& constraint = model.constraints[i];

        for (std::size_t j = 0; j < n; ++j)
        {
            tableau.a[i][j] =
                constraint.coefficients[j];
        }

        // Slack variable
        tableau.a[i][n + i] = 1.0;

        tableau.a[i][total_variables] =
            constraint.rhs;

        tableau.basis[i] =
            static_cast<int>(n + i);
    }

    // --------------------------------------------------------
    // Objective row
    //
    // Tableau uses:
    //
    // z - c1*x1 - c2*x2 ... = 0
    //
    // --------------------------------------------------------

    for (std::size_t j = 0; j < n; ++j)
    {
        double coefficient =
            model.objective[j];

        if (model.objective_sense
            == ObjectiveSense::Minimize)
        {
            coefficient = -coefficient;
        }

        tableau.a[m][j] = -coefficient;
    }

    // --------------------------------------------------------
    // Negative RHS cannot be handled by this basic
    // primal-simplex initialization.
    //
    // Phase I will be added in the next mathematical-core
    // iteration.
    // --------------------------------------------------------

    for (std::size_t i = 0; i < m; ++i)
    {
        if (tableau.a[i][total_variables]
            < -options_.tolerance)
        {
            return {
                LPStatus::Infeasible,
                0.0,
                {},
                0,
                "Negative RHS requires Phase-I feasibility initialization."
            };
        }
    }

    // --------------------------------------------------------
    // Simplex iterations
    // --------------------------------------------------------

    std::size_t iterations = 0;

    while (iterations < options_.maximum_iterations)
    {
        ++iterations;

        // ----------------------------------------------------
        // Choose entering variable.
        //
        // Bland-style smallest index selection is used to
        // reduce cycling risk.
        // ----------------------------------------------------

        std::size_t entering =
            total_variables;

        for (std::size_t j = 0;
             j < total_variables;
             ++j)
        {
            if (tableau.a[m][j]
                < -options_.tolerance)
            {
                entering = j;
                break;
            }
        }

        // ----------------------------------------------------
        // No negative reduced cost => optimal.
        // ----------------------------------------------------

        if (entering == total_variables)
        {
            std::vector<double> solution(n, 0.0);

            for (std::size_t i = 0; i < m; ++i)
            {
                const int basic =
                    tableau.basis[i];

                if (basic >= 0
                    && static_cast<std::size_t>(basic) < n)
                {
                    solution[
                        static_cast<std::size_t>(basic)
                    ] =
                        tableau.a[i][total_variables];
                }
            }

            double objective =
                tableau.a[m][total_variables];

            if (model.objective_sense
                == ObjectiveSense::Minimize)
            {
                objective = -objective;
            }

            return {
                LPStatus::Optimal,
                objective,
                std::move(solution),
                iterations,
                "Optimal solution found by simplex."
            };
        }

        // ----------------------------------------------------
        // Ratio test
        // ----------------------------------------------------

        std::size_t leaving =
            m;

        double best_ratio =
            std::numeric_limits<double>::infinity();

        for (std::size_t i = 0; i < m; ++i)
        {
            const double coefficient =
                tableau.a[i][entering];

            if (coefficient > options_.tolerance)
            {
                const double rhs =
                    tableau.a[i][total_variables];

                const double ratio =
                    rhs / coefficient;

                if (ratio < best_ratio - options_.tolerance)
                {
                    best_ratio = ratio;
                    leaving = i;
                }
            }
        }

        // ----------------------------------------------------
        // No leaving variable => unbounded.
        // ----------------------------------------------------

        if (leaving == m)
        {
            return {
                LPStatus::Unbounded,
                0.0,
                {},
                iterations,
                "LP is unbounded."
            };
        }

        // ----------------------------------------------------
        // Pivot
        // ----------------------------------------------------

        const double pivot =
            tableau.a[leaving][entering];

        if (approximately_zero(
                pivot,
                EPSILON))
        {
            return {
                LPStatus::NumericalError,
                0.0,
                {},
                iterations,
                "Numerically unstable pivot encountered."
            };
        }

        // Normalize pivot row
        for (std::size_t j = 0;
             j < tableau.columns;
             ++j)
        {
            tableau.a[leaving][j] /= pivot;
        }

        // Eliminate entering variable
        // from every other row.
        for (std::size_t i = 0;
             i < tableau.rows;
             ++i)
        {
            if (i == leaving)
            {
                continue;
            }

            const double factor =
                tableau.a[i][entering];

            if (approximately_zero(
                    factor,
                    EPSILON))
            {
                continue;
            }

            for (std::size_t j = 0;
                 j < tableau.columns;
                 ++j)
            {
                tableau.a[i][j] -=
                    factor *
                    tableau.a[leaving][j];
            }
        }

        tableau.basis[leaving] =
            static_cast<int>(entering);
    }

    return {
        LPStatus::IterationLimit,
        0.0,
        {},
        iterations,
        "Simplex iteration limit reached."
    };
}

// ============================================================
// String conversion
// ============================================================

const char* to_string(
    ObjectiveSense sense
) noexcept
{
    switch (sense)
    {
        case ObjectiveSense::Maximize:
            return "MAXIMIZE";

        case ObjectiveSense::Minimize:
            return "MINIMIZE";
    }

    return "UNKNOWN";
}

const char* to_string(
    ConstraintSense sense
) noexcept
{
    switch (sense)
    {
        case ConstraintSense::LessEqual:
            return "<=";

        case ConstraintSense::GreaterEqual:
            return ">=";

        case ConstraintSense::Equal:
            return "=";
    }

    return "?";
}

} // namespace sovopt::lp

