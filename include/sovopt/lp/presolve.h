#pragma once

#include "sovopt/lp/lp_model.h"
#include "sovopt/lp/lp_result.h"

#include <cstddef>
#include <string>
#include <vector>

namespace sovopt::lp
{

struct PresolveStats
{
    std::size_t original_rows = 0;
    std::size_t original_cols = 0;
    std::size_t presolved_rows = 0;
    std::size_t presolved_cols = 0;
    std::size_t removed_rows = 0;
    std::size_t fixed_variables = 0;
    std::size_t tightened_bounds = 0;
    double presolve_time_ms = 0.0;
    bool is_infeasible = false;
    std::string message;
};

class PresolveEngine
{
public:
    explicit PresolveEngine(double tolerance = 1e-9);

    // Run presolve reductions on LPModel
    bool presolve(const LPModel& original_model, LPModel& presolved_model, PresolveStats& stats);

    // Reconstruct original solution from presolved solution
    std::vector<double> postsolve(
        const LPModel& original_model,
        const LPModel& presolved_model,
        const std::vector<double>& presolved_solution
    );

private:
    double tolerance_;
    std::vector<int> col_mapping_; // maps original col index -> presolved col index (-1 if fixed)
    std::vector<double> fixed_values_;
};

} // namespace sovopt::lp
