#include "variable.h"

#include <stdexcept>
#include <utility>

namespace sovopt::core
{

Variable::Variable(
    VariableIndex index,
    std::string name,
    VariableType type,
    double lower_bound,
    double upper_bound
)
    : index_(index),
      name_(std::move(name)),
      type_(type),
      lower_bound_(lower_bound),
      upper_bound_(upper_bound)
{
    if (lower_bound > upper_bound)
    {
        throw std::invalid_argument(
            "Variable lower bound cannot exceed upper bound."
        );
    }

    if (type_ == VariableType::Binary)
    {
        if (lower_bound < 0.0 || upper_bound > 1.0)
        {
            throw std::invalid_argument(
                "Binary variable bounds must lie within [0, 1]."
            );
        }
    }
}

VariableIndex Variable::index() const noexcept
{
    return index_;
}

const std::string& Variable::name() const noexcept
{
    return name_;
}

VariableType Variable::type() const noexcept
{
    return type_;
}

double Variable::lower_bound() const noexcept
{
    return lower_bound_;
}

double Variable::upper_bound() const noexcept
{
    return upper_bound_;
}

} // namespace sovopt::core