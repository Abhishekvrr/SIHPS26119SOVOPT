#include "sovopt/lp/lp_model.h"

#include <cmath>
#include <stdexcept>

namespace sovopt::lp {

LPModel::LPModel(ObjectiveSense sense)
    : objective_sense_(sense),
      matrix_(0, 0)
{
}

std::size_t LPModel::add_variable(
    const std::string& name,
    double lower_bound,
    double upper_bound)
{
    if (finalized_) {
        throw std::logic_error(
            "Cannot modify a finalized LP model");
    }

    if (lower_bound > upper_bound) {
        throw std::invalid_argument(
            "Variable lower bound exceeds upper bound");
    }

    variables_.push_back({
        name,
        lower_bound,
        upper_bound
    });

    objective_.push_back(0.0);

    return variables_.size() - 1;
}

std::size_t LPModel::add_constraint(
    const std::string& name,
    ConstraintSense sense,
    double rhs)
{
    if (finalized_) {
        throw std::logic_error(
            "Cannot modify a finalized LP model");
    }

    constraints_.push_back({
        name,
        sense,
        rhs
    });

    return constraints_.size() - 1;
}

void LPModel::set_objective(
    std::size_t variable,
    double coefficient)
{
    if (variable >= objective_.size()) {
        throw std::out_of_range(
            "Objective variable index out of range");
    }

    objective_[variable] = coefficient;
}

void LPModel::set_coefficient(
    std::size_t constraint,
    std::size_t variable,
    double coefficient)
{
    if (constraint >= constraints_.size()) {
        throw std::out_of_range(
            "Constraint index out of range");
    }

    if (variable >= variables_.size()) {
        throw std::out_of_range(
            "Variable index out of range");
    }

    if (finalized_) {
        throw std::logic_error(
            "Cannot modify finalized LP matrix");
    }

    matrix_.add(
        constraint,
        variable,
        coefficient);
}

void LPModel::set_rhs(
    std::size_t constraint,
    double rhs)
{
    if (constraint >= constraints_.size()) {
        throw std::out_of_range(
            "Constraint index out of range");
    }

    constraints_[constraint].rhs = rhs;
}

void LPModel::set_constraint_sense(
    std::size_t constraint,
    ConstraintSense sense)
{
    if (constraint >= constraints_.size()) {
        throw std::out_of_range(
            "Constraint index out of range");
    }

    constraints_[constraint].sense = sense;
}

void LPModel::finalize()
{
    matrix_.finalize();
    finalized_ = true;
}

bool LPModel::validate(std::string& error) const
{
    if (variables_.empty()) {
        error = "LP contains no variables";
        return false;
    }

    if (objective_.size() != variables_.size()) {
        error = "Objective dimension mismatch";
        return false;
    }

    if (matrix_.rows() != constraints_.size()) {
        error = "Constraint matrix row mismatch";
        return false;
    }

    if (matrix_.columns() != variables_.size()) {
        error = "Constraint matrix column mismatch";
        return false;
    }

    for (const auto& variable : variables_) {
        if (!std::isfinite(variable.lower_bound) ||
            !std::isfinite(variable.upper_bound)) {
            error = "Variable bounds must be finite";
            return false;
        }

        if (variable.lower_bound > variable.upper_bound) {
            error = "Invalid variable bounds";
            return false;
        }
    }

    for (const auto& constraint : constraints_) {
        if (!std::isfinite(constraint.rhs)) {
            error = "Constraint RHS must be finite";
            return false;
        }
    }

    return true;
}

} // namespace sovopt::lp