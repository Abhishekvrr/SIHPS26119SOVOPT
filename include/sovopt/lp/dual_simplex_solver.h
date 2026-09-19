#pragma once

#include "sovopt/lp/lp_model.h"
#include "sovopt/lp/lp_result.h"
#include "sovopt/math/numerics.h"

#include <cstddef>
#include <vector>

namespace sovopt::lp
{

class DualSimplexSolver
{
public:
    explicit DualSimplexSolver(sovopt::math::NumericalSettings settings = {});

    LPResult solve(const LPModel& model);

    // Warm-start solve from an existing basis/tableau state with modified bounds
    LPResult solve_with_basis(
        const LPModel& model,
        const std::vector<std::size_t>& initial_basis
    );

private:
    struct DualTableau
    {
        std::vector<std::vector<double>> a; // rows + 1 obj row, columns + 1 RHS col
        std::size_t rhs_col = 0;
        std::vector<std::size_t> basis;
        std::vector<int> original_variable;
        std::size_t rows = 0;
        std::size_t cols = 0;
    };

    sovopt::math::NumericalSettings settings_;

    bool run_dual_simplex(DualTableau& tab, std::size_t& iterations, bool& infeasible);
    int choose_leaving_row(const DualTableau& tab) const;
    int choose_entering_column_dual(const DualTableau& tab, std::size_t leaving_row) const;
    bool pivot(DualTableau& tab, std::size_t row, std::size_t col);
};

} // namespace sovopt::lp
