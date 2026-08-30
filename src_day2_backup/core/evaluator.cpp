#include "sovopt/core/evaluator.h"

#include <cmath>
#include <stdexcept>

namespace sovopt::core
{

namespace
{

constexpr double FeasibilityTolerance = 1e-9;

double evaluate_linear_expression(
    const std::vector<LinearTerm>& terms,
    const Solution& solution
)
{
    double result = 0.0;

    for (const auto& term : terms)
    {
        result +=
            term.coefficient *
            solution.value(term.variable);
    }

    return result;
}

double calculate_violation(
    double activity,
    ConstraintSense sense,
    double rhs
)
{
    switch (sense)
    {
        case ConstraintSense::LessEqual:
            return std::max(0.0, activity - rhs);

        case ConstraintSense::Equal:
            return std::abs(activity - rhs);

        case ConstraintSense::GreaterEqual:
            return std::max(0.0, rhs - activity);
    }

    throw std::logic_error(
        "Unknown constraint sense."
    );
}

bool is_satisfied(
    double violation
)
{
    return violation <= FeasibilityTolerance;
}

} // namespace

EvaluationResult Evaluator::evaluate(
    const Model& model,
    const Solution& solution
) const
{
    if (solution.size() != model.variables().size())
    {
        throw std::invalid_argument(
            "Solution dimension does not match model variable count."
        );
    }

    EvaluationResult result{
        .objective_value = 0.0,
        .feasible = true,
        .total_violation = 0.0,
        .constraints = {}
    };

    /*
     * Evaluate objective.
     */
    if (model.objective().has_value())
    {
        const auto& objective = model.objective().value();

        result.objective_value =
            evaluate_linear_expression(
                objective.terms(),
                solution
            );
    }

    /*
     * Evaluate constraints.
     */
    result.constraints.reserve(
        model.constraints().size()
    );

    for (const auto& constraint : model.constraints())
    {
        const double activity =
            evaluate_linear_expression(
                constraint.terms(),
                solution
            );

        const double violation =
            calculate_violation(
                activity,
                constraint.sense(),
                constraint.rhs()
            );

        const bool satisfied =
            is_satisfied(violation);

        result.constraints.push_back(
            ConstraintEvaluation{
                .constraint = constraint.index(),
                .activity = activity,
                .rhs = constraint.rhs(),
                .violation = violation,
                .satisfied = satisfied
            }
        );

        result.total_violation += violation;

        if (!satisfied)
        {
            result.feasible = false;
        }
    }

    /*
     * Check variable bounds.
     */
    for (const auto& variable : model.variables())
    {
        const double value =
            solution.value(variable.index());

        if (!std::isfinite(value))
        {
            throw std::invalid_argument(
                "Solution contains a non-finite variable value."
            );
        }

        if (value < variable.lower_bound() -
                       FeasibilityTolerance)
        {
            result.feasible = false;
            result.total_violation +=
                variable.lower_bound() - value;
        }

        if (value > variable.upper_bound() +
                       FeasibilityTolerance)
        {
            result.feasible = false;
            result.total_violation +=
                value - variable.upper_bound();
        }

        if (variable.type() == VariableType::Binary)
        {
            if (std::abs(value) > FeasibilityTolerance &&
                std::abs(value - 1.0) >
                    FeasibilityTolerance)
            {
                result.feasible = false;

                const double distance =
                    std::min(
                        std::abs(value),
                        std::abs(value - 1.0)
                    );

                result.total_violation += distance;
            }
        }

        if (variable.type() == VariableType::Integer)
        {
            const double distance =
                std::abs(value - std::round(value));

            if (distance > FeasibilityTolerance)
            {
                result.feasible = false;
                result.total_violation += distance;
            }
        }
    }

    return result;
}

} // namespace sovopt::core