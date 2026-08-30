#include "objective.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace sovopt::core
{

Objective::Objective(
    OptimizationSense sense,
    std::vector<LinearTerm> terms
)
    : sense_(sense),
      terms_(std::move(terms))
{
    for (const auto& term : terms_)
    {
        if (!std::isfinite(term.coefficient))
        {
            throw std::invalid_argument(
                "Objective contains a non-finite coefficient."
            );
        }
    }
}

OptimizationSense Objective::sense() const noexcept
{
    return sense_;
}

const std::vector<LinearTerm>& Objective::terms() const noexcept
{
    return terms_;
}

} // namespace sovopt::core