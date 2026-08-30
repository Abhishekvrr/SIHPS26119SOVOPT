#include "constraint.h"

#include <cmath>
#include <stdexcept>

namespace sovopt::core
{

Constraint::Constraint(
    ConstraintIndex index,
    std::string name,
    std::vector<LinearTerm> terms,
    ConstraintSense sense,
    double rhs
)
    : index_(index),
      name_(std::move(name)),
      terms_(std::move(terms)),
      sense_(sense),
      rhs_(rhs)
{
    for (const auto& term : terms_)
    {
        if (!std::isfinite(term.coefficient))
        {
            throw std::invalid_argument(
                "Constraint contains a non-finite coefficient."
            );
        }
    }

    if (!std::isfinite(rhs_))
    {
        throw std::invalid_argument(
            "Constraint right-hand side must be finite."
        );
    }
}

ConstraintIndex Constraint::index() const noexcept
{
    return index_;
}

const std::string& Constraint::name() const noexcept
{
    return name_;
}

const std::vector<LinearTerm>& Constraint::terms() const noexcept
{
    return terms_;
}

ConstraintSense Constraint::sense() const noexcept
{
    return sense_;
}

double Constraint::rhs() const noexcept
{
    return rhs_;
}

} // namespace sovopt::core