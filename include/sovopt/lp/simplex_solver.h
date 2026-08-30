#pragma once

#include "sovopt/lp/lp_model.h"
#include "sovopt/lp/lp_result.h"
#include "sovopt/math/numerics.h"

#include <cstddef>
#include <vector>

namespace sovopt::lp
{

class SimplexSolver
{
public:

    explicit SimplexSolver(
        sovopt::math::NumericalSettings settings = {});

    LPResult solve(const LPModel& model);

private:

    struct Tableau
    {
        // Constraint rows + objective row.
        // Last column is the RHS.
        std::vector<std::vector<double>> a;

        // RHS column index.
        std::size_t rhs_column = 0;

        // Basic variable for each constraint row.
        std::vector<std::size_t> basis;

        // True for artificial-variable columns.
        std::vector<bool> artificial;

        // Original LP variable represented by each tableau column.
        // -1 means slack/surplus/artificial variable.
        std::vector<int> original_variable;

        std::size_t rows = 0;
        std::size_t columns = 0;
    };

    sovopt::math::NumericalSettings settings_;

    LPResult solve_standard(const LPModel& model);

    bool run_simplex(
        Tableau& tableau,
        std::vector<double>& objective,
        std::size_t& iterations,
        bool& unbounded);

    bool pivot(
        Tableau& tableau,
        std::size_t row,
        std::size_t column);

    int choose_entering_variable(
        const Tableau& tableau,
        const std::vector<double>& objective) const;

    int choose_leaving_row(
        const Tableau& tableau,
        std::size_t entering_column) const;

    double calculate_objective(
        const LPModel& model,
        const std::vector<double>& solution) const;

    bool validate_solution(
        const LPModel& model,
        const std::vector<double>& solution,
        double& residual) const;
};

} // namespace sovopt::lp
