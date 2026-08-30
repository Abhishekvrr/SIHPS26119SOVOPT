#include "sovopt/math/linear_solver.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace sovopt::math {

LinearSolveResult solve_linear_system(
    const std::vector<std::vector<double>>& input,
    const std::vector<double>& rhs,
    double tolerance)
{
    LinearSolveResult result;

    const std::size_t n = input.size();

    if (n == 0 || rhs.size() != n) {
        return result;
    }

    for (const auto& row : input) {
        if (row.size() != n) {
            return result;
        }
    }

    std::vector<std::vector<double>> a = input;
    std::vector<double> b = rhs;

    for (std::size_t column = 0; column < n; ++column) {

        std::size_t pivot = column;
        double maximum = std::abs(a[column][column]);

        for (std::size_t row = column + 1; row < n; ++row) {
            const double value = std::abs(a[row][column]);

            if (value > maximum) {
                maximum = value;
                pivot = row;
            }
        }

        if (maximum <= tolerance) {
            return result;
        }

        if (pivot != column) {
            std::swap(a[pivot], a[column]);
            std::swap(b[pivot], b[column]);
        }

        for (std::size_t row = column + 1; row < n; ++row) {

            const double factor =
                a[row][column] / a[column][column];

            if (std::abs(factor) <= tolerance) {
                continue;
            }

            for (std::size_t j = column; j < n; ++j) {
                a[row][j] -= factor * a[column][j];
            }

            b[row] -= factor * b[column];
        }
    }

    std::vector<double> x(n, 0.0);

    for (std::size_t i = n; i-- > 0;) {

        double value = b[i];

        for (std::size_t j = i + 1; j < n; ++j) {
            value -= a[i][j] * x[j];
        }

        if (std::abs(a[i][i]) <= tolerance) {
            return result;
        }

        x[i] = value / a[i][i];
    }

    double residual = 0.0;

    for (std::size_t i = 0; i < n; ++i) {

        double calculated = 0.0;

        for (std::size_t j = 0; j < n; ++j) {
            calculated += input[i][j] * x[j];
        }

        residual =
            std::max(residual,
                     std::abs(calculated - rhs[i]));
    }

    result.success = true;
    result.solution = std::move(x);
    result.residual = residual;

    return result;
}

} // namespace sovopt::math
