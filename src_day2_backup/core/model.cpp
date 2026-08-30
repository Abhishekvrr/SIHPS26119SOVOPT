#include "model.h"

#include <stdexcept>
#include <utility>

namespace sovopt::core
{

Model::Model(std::string name)
    : name_(std::move(name))
{
    if (name_.empty())
    {
        throw std::invalid_argument(
            "Model name cannot be empty."
        );
    }
}

VariableIndex Model::add_variable(
    const std::string& name,
    VariableType type,
    double lower_bound,
    double upper_bound
)
{
    const VariableIndex index = variables_.size();

    variables_.emplace_back(
        index,
        name,
        type,
        lower_bound,
        upper_bound
    );

    return index;
}

ConstraintIndex Model::add_constraint(
    const std::string& name,
    std::vector<LinearTerm> terms,
    ConstraintSense sense,
    double rhs
)
{
    for (const auto& term : terms)
    {
        if (term.variable >= variables_.size())
        {
            throw std::out_of_range(
                "Constraint references an unknown variable."
            );
        }
    }

    const ConstraintIndex index = constraints_.size();

    constraints_.emplace_back(
        index,
        name,
        std::move(terms),
        sense,
        rhs
    );

    return index;
}

void Model::set_objective(
    OptimizationSense sense,
    std::vector<LinearTerm> terms
)
{
    for (const auto& term : terms)
    {
        if (term.variable >= variables_.size())
        {
            throw std::out_of_range(
                "Objective references an unknown variable."
            );
        }
    }

    objective_.emplace(
        sense,
        std::move(terms)
    );
}

const std::string& Model::name() const noexcept
{
    return name_;
}

const std::vector<Variable>& Model::variables() const noexcept
{
    return variables_;
}

const std::vector<Constraint>& Model::constraints() const noexcept
{
    return constraints_;
}

const std::optional<Objective>& Model::objective() const noexcept
{
    return objective_;
}

} // namespace sovopt::core