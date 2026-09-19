#pragma once

#include "sovopt/core/model.h"
#include "sovopt/optimization/optimization_options.h"
#include "sovopt/optimization/optimization_result.h"

#include <string>

namespace sovopt::io
{

class JsonIO
{
public:
    static bool parse_model(
        const std::string& json_str,
        core::Model& model,
        optimization::OptimizationOptions& options,
        std::string& error
    );

    static std::string serialize_result(
        const optimization::OptimizationResult& result
    );
};

} // namespace sovopt::io

