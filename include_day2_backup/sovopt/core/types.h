#pragma once

#include <cstddef>
#include <limits>

namespace sovopt::core
{

using VariableIndex = std::size_t;
using ConstraintIndex = std::size_t;

constexpr double Infinity = std::numeric_limits<double>::infinity();

enum class OptimizationSense
{
    Minimize,
    Maximize
};

enum class ConstraintSense
{
    LessEqual,
    Equal,
    GreaterEqual
};

enum class VariableType
{
    Continuous,
    Integer,
    Binary
};

} // namespace sovopt::core