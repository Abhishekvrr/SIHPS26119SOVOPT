#ifndef SOVOPT_SIMPLEX_SOLVER_H
#define SOVOPT_SIMPLEX_SOLVER_H

#include "sovopt/core/model.h"
#include "sovopt/core/solution.h"
#include <vector>
#include <string>

namespace sovopt {
namespace algorithms {

enum class SolverStatus {
    OPTIMAL,
    INFEASIBLE,
    UNBOUNDED,
    MAX_ITERATIONS_REACHED,
    ERROR
};

struct SolverResult {
    SolverStatus status;
    core::Solution solution;
    double optimal_value{0.0};
    int iterations{0};
    std::string message;
};

class SimplexSolver {
public:
    explicit SimplexSolver(int max_iterations = 1000, double tolerance = 1e-7);

    SolverResult solve(const core::Model& model);

private:
    int max_iterations_;
    double tolerance_;
};

} // namespace algorithms
} // namespace sovopt

#endif // SOVOPT_SIMPLEX_SOLVER_H