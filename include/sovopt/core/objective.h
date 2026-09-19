#pragma once

#include "constraint.h"
#include "types.h"

#include <string>
#include <vector>

namespace sovopt::core
{

struct QuadraticObjectiveTerm
{
    VariableIndex row_variable;
    VariableIndex col_variable;
    double coefficient;
};

class Objective
{
public:
    Objective(
        OptimizationSense sense,
        std::vector<LinearTerm> terms,
        std::vector<QuadraticObjectiveTerm> quadratic_terms = {}
    );

    OptimizationSense sense() const noexcept;
    const std::vector<LinearTerm>& terms() const noexcept;
    const std::vector<QuadraticObjectiveTerm>& quadratic_terms() const noexcept;
    bool is_quadratic() const noexcept;

private:
    OptimizationSense sense_;
    std::vector<LinearTerm> terms_;
    std::vector<QuadraticObjectiveTerm> quadratic_terms_;
};

} // namespace sovopt::core