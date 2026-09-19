#include "sovopt/math/sparse_matrix.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace sovopt::math
{

SparseMatrix::SparseMatrix(
    std::size_t rows,
    std::size_t columns)
{
    resize(rows, columns);
}

void SparseMatrix::resize(
    std::size_t rows,
    std::size_t columns)
{
    rows_ = rows;
    columns_ = columns;

    pending_.clear();
    entries_.clear();

    finalized_ = false;
}

void SparseMatrix::set_dimensions(
    std::size_t rows,
    std::size_t columns)
{
    if (finalized_)
    {
        throw std::logic_error(
            "Cannot change dimensions of finalized SparseMatrix.");
    }

    rows_ = rows;
    columns_ = columns;
}

void SparseMatrix::validateIndex(
    std::size_t row,
    std::size_t column) const
{
    if (row >= rows_)
    {
        throw std::out_of_range(
            "SparseMatrix row index out of range.");
    }

    if (column >= columns_)
    {
        throw std::out_of_range(
            "SparseMatrix column index out of range.");
    }
}

void SparseMatrix::add(
    std::size_t row,
    std::size_t column,
    double value)
{
    validateIndex(row, column);

    if (finalized_)
    {
        throw std::logic_error(
            "Cannot modify finalized SparseMatrix.");
    }

    if (std::abs(value) <= 1e-12)
    {
        return;
    }

    pending_.push_back(
        SparseEntry{
            row,
            column,
            value
        }
    );
}

void SparseMatrix::set(
    std::size_t row,
    std::size_t column,
    double value)
{
    validateIndex(row, column);

    if (finalized_)
    {
        throw std::logic_error(
            "Cannot modify finalized SparseMatrix.");
    }

    for (auto& entry : pending_)
    {
        if (entry.row == row &&
            entry.column == column)
        {
            entry.value = value;

            if (std::abs(value) <= 1e-12)
            {
                entry.value = 0.0;
            }

            return;
        }
    }

    if (std::abs(value) > 1e-12)
    {
        pending_.push_back(
            SparseEntry{
                row,
                column,
                value
            }
        );
    }
}

void SparseMatrix::finalize()
{
    if (finalized_)
    {
        return;
    }

    std::sort(
        pending_.begin(),
        pending_.end(),
        [](const SparseEntry& lhs,
           const SparseEntry& rhs)
        {
            if (lhs.row != rhs.row)
            {
                return lhs.row < rhs.row;
            }

            return lhs.column < rhs.column;
        }
    );

    entries_.clear();

    for (const auto& entry : pending_)
    {
        if (!entries_.empty() &&
            entries_.back().row == entry.row &&
            entries_.back().column == entry.column)
        {
            entries_.back().value += entry.value;
        }
        else
        {
            entries_.push_back(entry);
        }
    }

    entries_.erase(
        std::remove_if(
            entries_.begin(),
            entries_.end(),
            [](const SparseEntry& entry)
            {
                return std::abs(entry.value) <= 1e-12;
            }
        ),
        entries_.end()
    );

    pending_.clear();

    finalized_ = true;
}

double SparseMatrix::get(
    std::size_t row,
    std::size_t column) const
{
    validateIndex(row, column);

    const auto& source =
        finalized_ ? entries_ : pending_;

    for (const auto& entry : source)
    {
        if (entry.row == row &&
            entry.column == column)
        {
            return entry.value;
        }
    }

    return 0.0;
}

std::size_t SparseMatrix::rows() const noexcept
{
    return rows_;
}

std::size_t SparseMatrix::columns() const noexcept
{
    return columns_;
}

std::size_t SparseMatrix::nonZeros() const noexcept
{
    return finalized_
        ? entries_.size()
        : pending_.size();
}

const std::vector<SparseEntry>&
SparseMatrix::entries() const noexcept
{
    return entries_;
}

std::vector<double> SparseMatrix::multiply(
    const std::vector<double>& vector) const
{
    if (vector.size() != columns_)
    {
        throw std::invalid_argument(
            "SparseMatrix::multiply dimension mismatch.");
    }

    const auto& source =
        finalized_ ? entries_ : pending_;

    std::vector<double> result(
        rows_,
        0.0
    );

    for (const auto& entry : source)
    {
        result[entry.row] +=
            entry.value *
            vector[entry.column];
    }

    return result;
}

std::vector<double>
SparseMatrix::multiplyTranspose(
    const std::vector<double>& vector) const
{
    if (vector.size() != rows_)
    {
        throw std::invalid_argument(
            "SparseMatrix::multiplyTranspose dimension mismatch.");
    }

    const auto& source =
        finalized_ ? entries_ : pending_;

    std::vector<double> result(
        columns_,
        0.0
    );

    for (const auto& entry : source)
    {
        result[entry.column] +=
            entry.value *
            vector[entry.row];
    }

    return result;
}

bool SparseMatrix::finalized() const noexcept
{
    return finalized_;
}

} // namespace sovopt::math