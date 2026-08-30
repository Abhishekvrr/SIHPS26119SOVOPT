#include "sovopt/core/solution.h"

#include <stdexcept>

namespace sovopt::core
{

Solution::Solution(std::size_t variable_count)
    : values_(variable_count, 0.0)
{
}

std::size_t Solution::size() const noexcept
{
    return values_.size();
}

double Solution::value(VariableIndex variable) const
{
    if (variable >= values_.size())
    {
        throw std::out_of_range(
            "Solution variable index is out of range."
        );
    }

    return values_[variable];
}

void Solution::set_value(
    VariableIndex variable,
    double value
)
{
    if (variable >= values_.size())
    {
        throw std::out_of_range(
            "Solution variable index is out of range."
        );
    }

    values_[variable] = value;
}

const std::vector<double>& Solution::values() const noexcept
{
    return values_;
}

std::vector<double>& Solution::values() noexcept
{
    return values_;
}

} // namespace sovopt::core