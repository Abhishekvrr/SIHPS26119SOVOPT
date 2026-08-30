#include "sovopt/math/vector.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace sovopt::math {

double Vector::dot(const Vector& other) const
{
    if (size() != other.size()) {
        throw std::invalid_argument(
            "Vector dot product dimension mismatch");
    }

    double result = 0.0;

    for (std::size_t i = 0; i < size(); ++i) {
        result += values_[i] * other.values_[i];
    }

    return result;
}

double Vector::norm_inf() const
{
    double result = 0.0;

    for (double value : values_) {
        result = std::max(result, std::abs(value));
    }

    return result;
}

void Vector::scale(double factor)
{
    for (double& value : values_) {
        value *= factor;
    }
}

} // namespace sovopt::math