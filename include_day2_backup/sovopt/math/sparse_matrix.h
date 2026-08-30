#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace sovopt::math {

class SparseMatrix {
public:
    SparseMatrix() = default;

    SparseMatrix(std::size_t rows,
                 std::size_t columns);

    void reserve(std::size_t nonzeros);

    void add_entry(std::size_t row,
                   std::size_t column,
                   double value);

    void compress();

    [[nodiscard]] std::size_t rows() const noexcept;
    [[nodiscard]] std::size_t columns() const noexcept;
    [[nodiscard]] std::size_t nonzeros() const noexcept;

    [[nodiscard]] double coefficient(std::size_t row,
                                     std::size_t column) const;

    [[nodiscard]] std::vector<double>
    multiply(std::span<const double> vector) const;

    [[nodiscard]] std::vector<double>
    transpose_multiply(std::span<const double> vector) const;

    [[nodiscard]] std::span<const std::size_t>
    row_columns(std::size_t row) const;

    [[nodiscard]] std::span<const double>
    row_values(std::size_t row) const;

private:
    std::size_t rows_ = 0;
    std::size_t columns_ = 0;

    std::vector<std::size_t> row_ptr_;
    std::vector<std::size_t> column_indices_;
    std::vector<double> values_;

    struct Entry {
        std::size_t row;
        std::size_t column;
        double value;
    };

    std::vector<Entry> pending_entries_;
    bool compressed_ = false;
};

} // namespace sovopt::math
