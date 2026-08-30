#include "sovopt/lp/lp_model.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace sovopt::lp {

LPModel::LPModel(std::size_t variables)
    : objective_(variables, 0.0),
      lower_bounds_(variables, 0.0),
      upper_bounds_(
          variables,
          std::numeric_limits<double>::infinity())
{
}

std::size_t LPModel::add_variable(double objective_coefficient)
{
    const std::size_t index = objective_.size();

    objective_.push_back(objective_coefficient);
    lower_bounds_.push_back(0.0);
    upper_bounds_.push_back(
        std::numeric_limits<double>::infinity());

    return index;
}

void LPModel::set_objective_sense(ObjectiveSense sense)
{
    objective_sense_ = sense;
}

void LPModel::set_objective_coefficient(
    std::size_t variable,
    double coefficient)
{
    objective_.at(variable) = coefficient;
}

void LPModel::set_lower_bound(
    std::size_t variable,
    double lower_bound)
{
    lower_bounds_.at(variable) = lower_bound;
}

void LPModel::set_upper_bound(
    std::size_t variable,
    double upper_bound)
{
    upper_bounds_.at(variable) = upper_bound;
}

void LPModel::add_constraint(
    std::vector<std::pair<std::size_t, double>> coefficients,
    ConstraintSense sense,
    double rhs)
{
    for (const auto& [variable, coefficient] : coefficients) {
        if (variable >= variable_count()) {
            throw std::out_of_range(
                "Constraint references invalid variable");
        }

        (void)coefficient;
    }

    constraints_.push_back(
        {std::move(coefficients), sense, rhs});
}

std::size_t LPModel::variable_count() const noexcept
{
    return objective_.size();
}

std::size_t LPModel::constraint_count() const noexcept
{
    return constraints_.size();
}

ObjectiveSense LPModel::objective_sense() const noexcept
{
    return objective_sense_;
}

const std::vector<double>& LPModel::objective() const noexcept
{
    return objective_;
}

const std::vector<double>& LPModel::lower_bounds() const noexcept
{
    return lower_bounds_;
}

const std::vector<double>& LPModel::upper_bounds() const noexcept
{
    return upper_bounds_;
}

const std::vector<LPConstraint>&
LPModel::constraints() const noexcept
{
    return constraints_;
}

math::SparseMatrix LPModel::build_sparse_matrix() const
{
    math::SparseMatrix matrix(
        constraint_count(),
        variable_count());

    std::size_t expected_nonzeros = 0;

    for (const auto& constraint : constraints_) {
        expected_nonzeros += constraint.coefficients.size();
    }

    matrix.reserve(expected_nonzeros);

    for (std::size_t row = 0;
         row < constraints_.size();
         ++row) {

        for (const auto& [column, value] :
             constraints_[row].coefficients) {

            matrix.add_entry(row, column, value);
        }
    }

    matrix.compress();

    return matrix;
}

} // namespace sovopt::lp
