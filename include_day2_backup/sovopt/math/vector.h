#pragma once

#include <cstddef>
#include <vector>

namespace sovopt::math {

class Vector {
public:
    Vector() = default;
    explicit Vector(std::size_t size);
    Vector(std::size_t size, double value);
    explicit Vector(std::vector<double> values);

    [[nodiscard]] std::size_t size() const noexcept;

    double& operator[](std::size_t index);
    const double& operator[](std::size_t index) const;

    void resize(std::size_t size);
    void fill(double value);

    [[nodiscard]] double dot(const Vector& other) const;
    [[nodiscard]] double norm_inf() const;

    [[nodiscard]] const std::vector<double>& data() const noexcept;
    [[nodiscard]] std::vector<double>& data() noexcept;

private:
    std::vector<double> values_;
};

} // namespace sovopt::math
