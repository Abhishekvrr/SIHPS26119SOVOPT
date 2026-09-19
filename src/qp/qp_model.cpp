#include "sovopt/qp/qp_model.h"
#include "sovopt/math/simd_vector.h"

#include <cmath>

namespace sovopt::qp
{

QPModel::QPModel(core::OptimizationSense sense)
    : sense_(sense)
{
}

std::size_t QPModel::add_variable(
    const std::string& name,
    double lower_bound,
    double upper_bound
)
{
    const std::size_t idx = variables_.size();
    variables_.emplace_back(idx, name, core::VariableType::Continuous, lower_bound, upper_bound);
    linear_obj_.push_back(0.0);

    // Expand Hessian Q matrix
    for (auto& row : Q_)
    {
        row.push_back(0.0);
    }
    Q_.emplace_back(variables_.size(), 0.0);

    return idx;
}

std::size_t QPModel::add_constraint(
    const std::string& name,
    core::ConstraintSense sense,
    double rhs
)
{
    const std::size_t idx = constraints_.size();
    constraints_.emplace_back(idx, name, std::vector<core::LinearTerm>{}, sense, rhs);
    return idx;
}

void QPModel::set_linear_coefficient(std::size_t constraint_idx, std::size_t var_idx, double coeff)
{
    if (constraint_idx < constraints_.size() && var_idx < variables_.size())
    {
        // Add or update term
        auto terms = constraints_[constraint_idx].terms();
        bool updated = false;
        for (auto& t : terms)
        {
            if (t.variable == var_idx)
            {
                t.coefficient = coeff;
                updated = true;
                break;
            }
        }
        if (!updated)
        {
            terms.push_back(core::LinearTerm{.variable = var_idx, .coefficient = coeff});
        }
        constraints_[constraint_idx] = core::Constraint(
            constraint_idx,
            constraints_[constraint_idx].name(),
            terms,
            constraints_[constraint_idx].sense(),
            constraints_[constraint_idx].rhs()
        );
    }
}

void QPModel::set_linear_objective(std::size_t var_idx, double coeff)
{
    if (var_idx < linear_obj_.size())
    {
        linear_obj_[var_idx] = coeff;
    }
}

void QPModel::set_quadratic_objective(std::size_t row_var, std::size_t col_var, double coeff)
{
    if (row_var < Q_.size() && col_var < Q_[row_var].size())
    {
        Q_[row_var][col_var] = coeff;
        if (row_var != col_var)
        {
            Q_[col_var][row_var] = coeff; // Maintain symmetry
        }
    }
}

double QPModel::evaluate_objective(const std::vector<double>& x) const
{
    const std::size_t n = variables_.size();
    double obj = 0.0;

    // Linear term: c^T x
    obj += sovopt::math::SIMDVector::dot_product(linear_obj_.data(), x.data(), n);

    // Quadratic term: 0.5 * x^T Q x
    std::vector<double> Qx(n, 0.0);
    sovopt::math::SIMDVector::mat_vec_mult(Q_, x.data(), Qx.data(), n, n);
    obj += 0.5 * sovopt::math::SIMDVector::dot_product(x.data(), Qx.data(), n);

    if (sense_ == core::OptimizationSense::Maximize)
    {
        return -obj;
    }
    return obj;
}

} // namespace sovopt::qp
