#pragma once

#include "sovopt/core/constraint.h"
#include "sovopt/core/objective.h"
#include "sovopt/core/variable.h"

#include <optional>
#include <string>
#include <vector>

namespace sovopt::core
{

class Model
{
public:
    explicit Model(std::string name);

    VariableIndex add_variable(
        const std::string& name,
        VariableType type = VariableType::Continuous,
        double lower_bound = 0.0,
        double upper_bound = Infinity
    );

    ConstraintIndex add_constraint(
        const std::string& name,
        std::vector<LinearTerm> terms,
        ConstraintSense sense,
        double rhs
    );

    void set_objective(
        OptimizationSense sense,
        std::vector<LinearTerm> terms
    );

    void set_objective(
        OptimizationSense sense,
        std::vector<LinearTerm> terms,
        std::vector<QuadraticObjectiveTerm> quadratic_terms
    );

    const std::string& name() const noexcept;

    const std::vector<Variable>& variables() const noexcept;
    const std::vector<Constraint>& constraints() const noexcept;

    const std::optional<Objective>& objective() const noexcept;

    bool is_milp() const noexcept;
    bool is_qp() const noexcept;

private:
    std::string name_;

    std::vector<Variable> variables_;
    std::vector<Constraint> constraints_;

    std::optional<Objective> objective_;
};

} // namespace sovopt::core