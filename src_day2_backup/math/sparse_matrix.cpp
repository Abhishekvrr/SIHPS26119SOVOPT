#include "sovopt/math/sparse_matrix.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace sovopt::math {

SparseMatrix::SparseMatrix(std::size_t rows,
                           std::size_t columns)
    : rows_(rows),
      columns_(columns),
      row_ptr_(rows + 1, 0)
{
}

void SparseMatrix::reserve(std::size_t nonzeros)
{
    pending_entries_.reserve(nonzeros);
}

void SparseMatrix::add_entry(std::size_t row,
                             std::size_t column,
                             double value)
{
    if (row >= rows_ || column >= columns_) {
        throw std::out_of_range("SparseMatrix index out of range");
    }

    if (compressed_) {
        throw std::logic_error(
            "Cannot add entries after matrix compression");
    }

    if (std::abs(value) < 1e-12) {
        return;
    }

    pending_entries_.push_back({row, column, value});
}

void SparseMatrix::compress()
{
    std::sort(
        pending_entries_.begin(),
        pending_entries_.end(),
        [](const Entry& a, const Entry& b) {
            if (a.row != b.row) {
                return a.row < b.row;
            }

            return a.column < b.column;
        });

    std::vector<Entry> merged;

    for (const auto& entry : pending_entries_) {
        if (!merged.empty() &&
            merged.back().row == entry.row &&
            merged.back().column == entry.column) {

            merged.back().value += entry.value;
        }
        else {
            merged.push_back(entry);
        }
    }

    row_ptr_.assign(rows_ + 1, 0);

    for (const auto& entry : merged) {
        ++row_ptr_[entry.row + 1];
    }

    for (std::size_t i = 1; i < row_ptr_.size(); ++i) {
        row_ptr_[i] += row_ptr_[i - 1];
    }

    column_indices_.resize(merged.size());
    values_.resize(merged.size());

    std::vector<std::size_t> offsets = row_ptr_;

    for (const auto& entry : merged) {
        const std::size_t position = offsets[entry.row]++;

        column_indices_[position] = entry.column;
        values_[position] = entry.value;
    }

    pending_entries_.clear();
    pending_entries_.shrink_to_fit();

    compressed_ = true;
}

std::size_t SparseMatrix::rows() const noexcept
{
    return rows_;
}

std::size_t SparseMatrix::columns() const noexcept
{
    return columns_;
}

std::size_t SparseMatrix::nonzeros() const noexcept
{
    return values_.size();
}

double SparseMatrix::coefficient(std::size_t row,
                                 std::size_t column) const
{
    if (!compressed_) {
        throw std::logic_error(
            "Matrix must be compressed before access");
    }

    if (row >= rows_ || column >= columns_) {
        throw std::out_of_range("SparseMatrix index out of range");
    }

    const auto begin = row_ptr_[row];
    const auto end = row_ptr_[row + 1];

    for (std::size_t i = begin; i < end; ++i) {
        if (column_indices_[i] == column) {
            return values_[i];
        }
    }

    return 0.0;
}

std::vector<double>
SparseMatrix::multiply(std::span<const double> vector) const
{
    if (!compressed_) {
        throw std::logic_error(
            "Matrix must be compressed before multiplication");
    }

    if (vector.size() != columns_) {
        throw std::invalid_argument(
            "Matrix/vector dimension mismatch");
    }

    std::vector<double> result(rows_, 0.0);

    for (std::size_t row = 0; row < rows_; ++row) {
        for (std::size_t i = row_ptr_[row];
             i < row_ptr_[row + 1];
             ++i) {

            result[row] +=
                values_[i] * vector[column_indices_[i]];
        }
    }

    return result;
}

std::vector<double>
SparseMatrix::transpose_multiply(
    std::span<const double> vector) const
{
    if (!compressed_) {
        throw std::logic_error(
            "Matrix must be compressed before multiplication");
    }

    if (vector.size() != rows_) {
        throw std::invalid_argument(
            "Transpose matrix/vector dimension mismatch");
    }

    std::vector<double> result(columns_, 0.0);

    for (std::size_t row = 0; row < rows_; ++row) {
        for (std::size_t i = row_ptr_[row];
             i < row_ptr_[row + 1];
             ++i) {

            result[column_indices_[i]] +=
                values_[i] * vector[row];
        }
    }

    return result;
}

std::span<const std::size_t>
SparseMatrix::row_columns(std::size_t row) const
{
    if (row >= rows_) {
        throw std::out_of_range("SparseMatrix row out of range");
    }

    return std::span<const std::size_t>(
        column_indices_.data() + row_ptr_[row],
        row_ptr_[row + 1] - row_ptr_[row]);
}

std::span<const double>
SparseMatrix::row_values(std::size_t row) const
{
    if (row >= rows_) {
        throw std::out_of_range("SparseMatrix row out of range");
    }

    return std::span<const double>(
        values_.data() + row_ptr_[row],
        row_ptr_[row + 1] - row_ptr_[row]);
}

} // namespace sovopt::math
