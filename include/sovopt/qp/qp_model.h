#pragma once

#include "sovopt/core/constraint.h"
#include "sovopt/core/types.h"
#include "sovopt/core/variable.h"

#include <cstddef>
#include <string>
#include <vector>

namespace sovopt::qp
{

struct QuadraticTerm
{
    std::size_t row_var;
    std::size_t col_var;
    double coefficient;
};

class QPModel
{
public:
    explicit QPModel(core::OptimizationSense sense = core::OptimizationSense::Minimize);

    std::size_t add_variable(
        const std::string& name,
        double lower_bound = 0.0,
        double upper_bound = core::Infinity
    );

    std::size_t add_constraint(
        const std::string& name,
        core::ConstraintSense sense,
        double rhs
    );

    void set_linear_coefficient(std::size_t constraint_idx, std::size_t var_idx, double coeff);
    void set_linear_objective(std::size_t var_idx, double coeff);
    void set_quadratic_objective(std::size_t row_var, std::size_t col_var, double coeff);

    core::OptimizationSense sense() const noexcept { return sense_; }
    const std::vector<core::Variable>& variables() const noexcept { return variables_; }
    const std::vector<core::Constraint>& constraints() const noexcept { return constraints_; }
    const std::vector<double>& linear_objective() const noexcept { return linear_obj_; }
    const std::vector<std::vector<double>>& quadratic_matrix() const noexcept { return Q_; }

    double evaluate_objective(const std::vector<double>& x) const;

private:
    core::OptimizationSense sense_{core::OptimizationSense::Minimize};
    std::vector<core::Variable> variables_;
    std::vector<core::Constraint> constraints_;
    std::vector<double> linear_obj_;
    std::vector<std::vector<double>> Q_; // Hessian matrix (n x n)
};

} // namespace sovopt::qp
