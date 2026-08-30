#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace sovopt::math {

struct SparseEntry {
    std::size_t row{};
    std::size_t column{};
    double value{};
};

class SparseMatrix {
public:
    SparseMatrix() = default;

    SparseMatrix(std::size_t rows, std::size_t columns);

    void resize(std::size_t rows, std::size_t columns);

    void add(std::size_t row, std::size_t column, double value);

    void set(std::size_t row, std::size_t column, double value);

    [[nodiscard]]
    double get(std::size_t row, std::size_t column) const;

    void finalize();

    [[nodiscard]]
    std::size_t rows() const noexcept;

    [[nodiscard]]
    std::size_t columns() const noexcept;

    [[nodiscard]]
    std::size_t nonZeros() const noexcept;

    [[nodiscard]]
    const std::vector<SparseEntry>& entries() const noexcept;

    [[nodiscard]]
    std::vector<double> multiply(
        const std::vector<double>& vector) const;

    [[nodiscard]]
    std::vector<double> multiplyTranspose(
        const std::vector<double>& vector) const;

    [[nodiscard]]
    bool finalized() const noexcept;

private:
    void validateIndex(
        std::size_t row,
        std::size_t column) const;

    std::size_t rows_{0};
    std::size_t columns_{0};

    std::vector<SparseEntry> pending_;
    std::vector<SparseEntry> entries_;

    bool finalized_{false};
};

} // namespace sovopt::math