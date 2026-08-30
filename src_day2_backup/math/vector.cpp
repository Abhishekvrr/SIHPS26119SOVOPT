#include "sovopt/math/vector.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace sovopt::math {

Vector::Vector(std::size_t size)
    : values_(size, 0.0)
{
}

Vector::Vector(std::size_t size, double value)
    : values_(size, value)
{
}

Vector::Vector(std::vector<double> values)
    : values_(std::move(values))
{
}

std::size_t Vector::size() const noexcept
{
    return values_.size();
}

double& Vector::operator[](std::size_t index)
{
    return values_.at(index);
}

const double& Vector::operator[](std::size_t index) const
{
    return values_.at(index);
}

void Vector::resize(std::size_t size)
{
    values_.resize(size);
}

void Vector::fill(double value)
{
    std::fill(values_.begin(), values_.end(), value);
}

double Vector::dot(const Vector& other) const
{
    if (size() != other.size()) {
        throw std::invalid_argument("Vector size mismatch");
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

const std::vector<double>& Vector::data() const noexcept
{
    return values_;
}

std::vector<double>& Vector::data() noexcept
{
    return values_;
}

} // namespace sovopt::math
