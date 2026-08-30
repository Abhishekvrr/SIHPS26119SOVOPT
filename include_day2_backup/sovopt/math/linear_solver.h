#pragma once

#include <vector>

namespace sovopt::math {

struct LinearSolveResult {
    bool success = false;
    std::vector<double> solution;
    double residual = 0.0;
};

LinearSolveResult solve_linear_system(
    const std::vector<std::vector<double>>& matrix,
    const std::vector<double>& rhs,
    double tolerance = 1e-12);

} // namespace sovopt::math
