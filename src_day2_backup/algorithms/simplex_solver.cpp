#include "sovopt/algorithms/simplex_solver.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace sovopt::algorithms {

namespace {

struct StandardForm {
    std::vector<std::vector<double>> a;
    std::vector<double> b;
    std::vector<double> c;

    std::size_t original_variables = 0;

    double objective_sign = 1.0;
};

}

SimplexSolver::SimplexSolver(
    math::NumericalConfig config)
    : config_(config)
{
}

LPResult SimplexSolver::solve(const lp::LPModel& model)
{
    const auto start = std::chrono::steady_clock::now();

    LPResult result;

    const std::size_t n = model.variable_count();
    const std::size_t m = model.constraint_count();

    if (n == 0) {
        result.status = LPStatus::InvalidModel;
        result.message = "LP contains no variables.";
        return result;
    }

    /*
     * Prototype restriction:
     *
     * The first LP engine works directly with:
     *
     *   x >= 0
     *   A x <= b
     *
     * This is deliberate. Later we add a full transformation
     * layer for >=, equality and general variable bounds.
     */

    for (std::size_t j = 0; j < n; ++j) {

        if (std::abs(model.lower_bounds()[j]) >
            config_.feasibility_tolerance) {

            result.status = LPStatus::InvalidModel;
            result.message =
                "Current simplex phase requires lower bounds of zero.";

            return result;
        }

        if (!std::isinf(model.upper_bounds()[j])) {

            result.status = LPStatus::InvalidModel;
            result.message =
                "Finite upper bounds are handled in a later presolve phase.";

            return result;
        }
    }

    std::vector<std::vector<double>> a(
        m,
        std::vector<double>(n, 0.0));

    std::vector<double> b(m, 0.0);

    for (std::size_t i = 0; i < m; ++i) {

        const auto& constraint = model.constraints()[i];

        if (constraint.sense !=
            lp::ConstraintSense::LessEqual) {

            result.status = LPStatus::InvalidModel;
            result.message =
                "Current simplex phase requires <= constraints.";

            return result;
        }

        b[i] = constraint.rhs;

        for (const auto& [column, value] :
             constraint.coefficients) {

            a[i][column] = value;
        }

        if (b[i] < -config_.feasibility_tolerance) {

            result.status = LPStatus::InvalidModel;
            result.message =
                "Negative RHS requires Phase-I preprocessing.";

            return result;
        }
    }

    std::vector<double> c = model.objective();

    double objective_sign = 1.0;

    if (model.objective_sense() ==
        lp::ObjectiveSense::Minimize) {

        for (double& value : c) {
            value = -value;
        }

        objective_sign = -1.0;
    }

    result = solve_standard_form(
        a,
        b,
        c,
        n,
        objective_sign);

    const auto end = std::chrono::steady_clock::now();

    result.runtime_ms =
        std::chrono::duration<double, std::milli>(
            end - start).count();

    return result;
}

LPResult SimplexSolver::solve_standard_form(
    const std::vector<std::vector<double>>& input_a,
    const std::vector<double>& input_b,
    const std::vector<double>& input_c,
    std::size_t original_variables,
    double objective_sign)
{
    LPResult result;

    const std::size_t m = input_a.size();
    const std::size_t n = input_c.size();

    if (m == 0) {

        bool improving = false;

        for (double value : input_c) {
            if (value > config_.optimality_tolerance) {
                improving = true;
                break;
            }
        }

        if (improving) {
            result.status = LPStatus::Unbounded;
            result.message =
                "Objective can increase without bound.";
            return result;
        }

        result.status = LPStatus::Optimal;
        result.solution.assign(n, 0.0);
        result.objective_value = 0.0;

        return result;
    }

    /*
     * Tableau representation for the first working LP engine.
     *
     * The surrounding mathematical infrastructure is sparse,
     * while this initial simplex implementation keeps the
     * basis arithmetic explicit and easy to validate.
     */

    const std::size_t total_variables = n + m;

    std::vector<std::vector<double>> tableau(
        m + 1,
        std::vector<double>(total_variables + 1, 0.0));

    for (std::size_t i = 0; i < m; ++i) {

        for (std::size_t j = 0; j < n; ++j) {
            tableau[i][j] = input_a[i][j];
        }

        tableau[i][n + i] = 1.0;
        tableau[i][total_variables] = input_b[i];
    }

    /*
     * Maximization tableau.
     */
    for (std::size_t j = 0; j < n; ++j) {
        tableau[m][j] = -input_c[j];
    }

    std::vector<std::size_t> basis(m);

    for (std::size_t i = 0; i < m; ++i) {
        basis[i] = n + i;
    }

    for (std::size_t iteration = 0;
         iteration < config_.max_iterations;
         ++iteration) {

        result.iterations = iteration;

        /*
         * Bland-style deterministic entering variable:
         * select the first negative reduced cost.
         *
         * This helps avoid cycling on many degenerate models.
         */
        std::size_t entering =
            total_variables;

        for (std::size_t j = 0;
             j < total_variables;
             ++j) {

            if (tableau[m][j] <
                -config_.optimality_tolerance) {

                entering = j;
                break;
            }
        }

        if (entering == total_variables) {

            result.status = LPStatus::Optimal;

            result.solution.assign(
                original_variables,
                0.0);

            for (std::size_t row = 0;
                 row < m;
                 ++row) {

                const std::size_t variable =
                    basis[row];

                if (variable <
                    original_variables) {

                    result.solution[variable] =
                        tableau[row][total_variables];
                }
            }

            double objective = 0.0;

            for (std::size_t j = 0;
                 j < original_variables;
                 ++j) {

                objective +=
                    input_c[j] *
                    result.solution[j];
            }

            result.objective_value =
                objective * objective_sign;

            double maximum_violation = 0.0;

            for (std::size_t i = 0;
                 i < m;
                 ++i) {

                double lhs = 0.0;

                for (std::size_t j = 0;
                     j < original_variables;
                     ++j) {

                    lhs +=
                        input_a[i][j] *
                        result.solution[j];
                }

                maximum_violation =
                    std::max(
                        maximum_violation,
                        std::max(
                            0.0,
                            lhs - input_b[i]));
            }

            result.feasibility_error =
                maximum_violation;

            result.message =
                "Optimal solution found.";

            return result;
        }

        /*
         * Ratio test.
         */
        std::size_t leaving =
            m;

        double best_ratio =
            std::numeric_limits<double>::infinity();

        for (std::size_t i = 0;
             i < m;
             ++i) {

            const double coefficient =
                tableau[i][entering];

            if (coefficient >
                config_.pivot_tolerance) {

                const double ratio =
                    tableau[i][total_variables] /
                    coefficient;

                if (ratio <
                    best_ratio -
                    config_.feasibility_tolerance) {

                    best_ratio = ratio;
                    leaving = i;
                }
            }
        }

        if (leaving == m) {

            result.status = LPStatus::Unbounded;

            result.message =
                "LP is unbounded in the selected direction.";

            return result;
        }

        /*
         * Pivot.
         */
        const double pivot =
            tableau[leaving][entering];

        if (std::abs(pivot) <=
            config_.pivot_tolerance) {

            result.status =
                LPStatus::NumericalFailure;

            result.message =
                "Numerically unstable pivot encountered.";

            return result;
        }

        for (std::size_t j = 0;
             j <= total_variables;
             ++j) {

            tableau[leaving][j] /= pivot;
        }

        for (std::size_t i = 0;
             i <= m;
             ++i) {

            if (i == leaving) {
                continue;
            }

            const double factor =
                tableau[i][entering];

            if (std::abs(factor) <=
                config_.zero_tolerance) {

                continue;
            }

            for (std::size_t j = 0;
                 j <= total_variables;
                 ++j) {

                tableau[i][j] -=
                    factor *
                    tableau[leaving][j];
            }
        }

        basis[leaving] = entering;
    }

    result.status =
        LPStatus::IterationLimit;

    result.message =
        "Maximum simplex iterations reached.";

    return result;
}

const char* to_string(LPStatus status) noexcept
{
    switch (status) {

        case LPStatus::Optimal:
            return "OPTIMAL";

        case LPStatus::Infeasible:
            return "INFEASIBLE";

        case LPStatus::Unbounded:
            return "UNBOUNDED";

        case LPStatus::IterationLimit:
            return "ITERATION_LIMIT";

        case LPStatus::NumericalFailure:
            return "NUMERICAL_FAILURE";

        case LPStatus::InvalidModel:
            return "INVALID_MODEL";
    }

    return "UNKNOWN";
}

} // namespace sovopt::algorithms
