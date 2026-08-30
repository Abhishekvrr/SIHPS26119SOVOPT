#pragma once

#include <cstddef>
#include <vector>

namespace sovopt::math {

class Vector {
public:
    Vector() = default;

    explicit Vector(std::size_t size)
        : values_(size, 0.0) {}

    Vector(std::size_t size, double value)
        : values_(size, value) {}

    explicit Vector(std::vector<double> values)
        : values_(std::move(values)) {}

    std::size_t size() const noexcept {
        return values_.size();
    }

    bool empty() const noexcept {
        return values_.empty();
    }

    double& operator[](std::size_t index) {
        return values_[index];
    }

    const double& operator[](std::size_t index) const {
        return values_[index];
    }

    void resize(std::size_t size) {
        values_.resize(size);
    }

    void assign(std::size_t size, double value) {
        values_.assign(size, value);
    }

    std::vector<double>& data() noexcept {
        return values_;
    }

    const std::vector<double>& data() const noexcept {
        return values_;
    }

    double dot(const Vector& other) const;

    double norm_inf() const;

    void scale(double factor);

private:
    std::vector<double> values_;
};

} // namespace sovopt::math