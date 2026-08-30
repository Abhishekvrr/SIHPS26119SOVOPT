#pragma once

#include "constraint.h"
#include "types.h"

#include <string>
#include <vector>

namespace sovopt::core
{

class Objective
{
public:
    Objective(
        OptimizationSense sense,
        std::vector<LinearTerm> terms
    );

    OptimizationSense sense() const noexcept;
    const std::vector<LinearTerm>& terms() const noexcept;

private:
    OptimizationSense sense_;
    std::vector<LinearTerm> terms_;
};

} // namespace sovopt::core