#pragma once

#include "model.h"
#include "solution.h"

#include <cstddef>
#include <vector>

namespace sovopt::core
{

struct ConstraintEvaluation
{
    ConstraintIndex constraint;
    double activity;
    double rhs;
    double violation;
    bool satisfied;
};

struct EvaluationResult
{
    double objective_value;
    bool feasible;
    double total_violation;
    std::vector<ConstraintEvaluation> constraints;
};

class Evaluator
{
public:
    EvaluationResult evaluate(
        const Model& model,
        const Solution& solution
    ) const;
};

} // namespace sovopt::core