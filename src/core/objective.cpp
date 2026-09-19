#include "sovopt/core/objective.h"

namespace sovopt::core
{

Objective::Objective(
    OptimizationSense sense,
    std::vector<LinearTerm> terms,
    std::vector<QuadraticObjectiveTerm> quadratic_terms
)
    : sense_(sense),
      terms_(std::move(terms)),
      quadratic_terms_(std::move(quadratic_terms))
{
}

OptimizationSense Objective::sense() const noexcept
{
    return sense_;
}

const std::vector<LinearTerm>& Objective::terms() const noexcept
{
    return terms_;
}

const std::vector<QuadraticObjectiveTerm>& Objective::quadratic_terms() const noexcept
{
    return quadratic_terms_;
}

bool Objective::is_quadratic() const noexcept
{
    return !quadratic_terms_.empty();
}

} // namespace sovopt::core