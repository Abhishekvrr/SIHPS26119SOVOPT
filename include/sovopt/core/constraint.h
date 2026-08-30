#pragma once

#include "types.h"

#include <string>
#include <utility>
#include <vector>

namespace sovopt::core
{

struct LinearTerm
{
    VariableIndex variable;
    double coefficient;
};

class Constraint
{
public:
    Constraint(
        ConstraintIndex index,
        std::string name,
        std::vector<LinearTerm> terms,
        ConstraintSense sense,
        double rhs
    );

    ConstraintIndex index() const noexcept;
    const std::string& name() const noexcept;
    const std::vector<LinearTerm>& terms() const noexcept;

    ConstraintSense sense() const noexcept;
    double rhs() const noexcept;

private:
    ConstraintIndex index_;
    std::string name_;
    std::vector<LinearTerm> terms_;
    ConstraintSense sense_;
    double rhs_;
};

} // namespace sovopt::core