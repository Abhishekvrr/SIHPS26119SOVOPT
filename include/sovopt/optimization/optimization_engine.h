#pragma once

#include "sovopt/core/evaluator.h"
#include "sovopt/core/model.h"
#include "sovopt/lp/lp_model.h"
#include "sovopt/qp/qp_model.h"
#include "sovopt/optimization/optimization_options.h"
#include "sovopt/optimization/optimization_result.h"

#include <string>

namespace sovopt::optimization
{

class OptimizationEngine
{
public:
    OptimizationEngine() = default;

    [[nodiscard]] OptimizationResult solve(
        const core::Model& model,
        const OptimizationOptions& options = {}
    ) const;

    [[nodiscard]] OptimizationResult solve(
        const lp::LPModel& model,
        const OptimizationOptions& options = {}
    ) const;

private:
    [[nodiscard]] bool validate_model(
        const core::Model& model,
        std::string& error_message
    ) const;

    [[nodiscard]] std::string classify_problem(
        const core::Model& model
    ) const;

    [[nodiscard]] lp::LPModel translate_to_lp(
        const core::Model& model
    ) const;

    [[nodiscard]] qp::QPModel translate_to_qp(
        const core::Model& model
    ) const;
};

} // namespace sovopt::optimization
