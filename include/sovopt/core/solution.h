#pragma once

#include "types.h"

#include <cstddef>
#include <vector>

namespace sovopt::core
{

class Solution
{
public:
    explicit Solution(std::size_t variable_count = 0);

    std::size_t size() const noexcept;

    double value(VariableIndex variable) const;
    void set_value(VariableIndex variable, double value);

    const std::vector<double>& values() const noexcept;
    std::vector<double>& values() noexcept;

private:
    std::vector<double> values_;
};

} // namespace sovopt::core