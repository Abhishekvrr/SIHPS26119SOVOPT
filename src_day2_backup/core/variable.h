#pragma once

#include "types.h"

#include <string>

namespace sovopt::core
{

class Variable
{
public:
    Variable(
        VariableIndex index,
        std::string name,
        VariableType type,
        double lower_bound,
        double upper_bound
    );

    VariableIndex index() const noexcept;
    const std::string& name() const noexcept;
    VariableType type() const noexcept;

    double lower_bound() const noexcept;
    double upper_bound() const noexcept;

private:
    VariableIndex index_;
    std::string name_;
    VariableType type_;
    double lower_bound_;
    double upper_bound_;
};

} // namespace sovopt::core