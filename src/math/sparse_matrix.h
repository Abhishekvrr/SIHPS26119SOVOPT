#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

namespace sovopt::math
{

using Index = std::size_t;

struct SparseEntry
{
    Index row;
    Index column;
    double value;
};

class SparseMatrixCSR
{
public:
    SparseMatrixCSR() = default;

    SparseMatrixCSR(
        Index rows,
        Index columns,
        std::vector<double> values,
        std::vector<Index> column_indices,
        std::vector<Index> row_offsets
    );

    static SparseMatrixCSR from_entries(
        Index rows,
        Index columns,
        std::span<const SparseEntry> entries
    );

    Index rows() const noexcept;
    Index columns() const noexcept;
    Index non_zeros() const noexcept;

    std::span<const double> values() const noexcept;
    std::span<const Index> column_indices() const noexcept;
    std::span<const Index> row_offsets() const noexcept;

    std::vector<double> multiply(
        std::span<const double> vector
    ) const;

    double coefficient(
        Index row,
        Index column
    ) const;

private:
    Index rows_{0};
    Index columns_{0};

    std::vector<double> values_;
    std::vector<Index> column_indices_;
    std::vector<Index> row_offsets_;

    void validate_structure() const;
};

} // namespace sovopt::math