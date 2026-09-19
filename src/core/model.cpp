#include "sovopt/core/model.h"

namespace sovopt::core
{

Model::Model(std::string name)
    : name_(std::move(name))
{
}

VariableIndex Model::add_variable(
    const std::string& name,
    VariableType type,
    double lower_bound,
    double upper_bound
)
{
    const VariableIndex index = variables_.size();
    variables_.emplace_back(index, name, type, lower_bound, upper_bound);
    return index;
}

ConstraintIndex Model::add_constraint(
    const std::string& name,
    std::vector<LinearTerm> terms,
    ConstraintSense sense,
    double rhs
)
{
    const ConstraintIndex index = constraints_.size();
    constraints_.emplace_back(index, name, std::move(terms), sense, rhs);
    return index;
}

void Model::set_objective(
    OptimizationSense sense,
    std::vector<LinearTerm> terms
)
{
    objective_.emplace(sense, std::move(terms));
}

void Model::set_objective(
    OptimizationSense sense,
    std::vector<LinearTerm> terms,
    std::vector<QuadraticObjectiveTerm> quadratic_terms
)
{
    objective_.emplace(sense, std::move(terms), std::move(quadratic_terms));
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

bool Model::is_milp() const noexcept
{
    for (const auto& var : variables_)
    {
        if (var.type() == VariableType::Integer || var.type() == VariableType::Binary)
        {
            return true;
        }
    }
    return false;
}

bool Model::is_qp() const noexcept
{
    return objective_.has_value() && objective_->is_quadratic();
}

} // namespace sovopt::core