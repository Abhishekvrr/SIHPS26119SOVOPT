#include "sovopt/api/solver_api.h"

#include "sovopt/core/model.h"
#include "sovopt/io/json_io.h"
#include "sovopt/optimization/optimization_engine.h"
#include "sovopt/optimization/optimization_options.h"
#include "sovopt/optimization/optimization_result.h"

namespace sovopt::api
{

int SolverAPI::run(
    const std::string& input_json,
    std::string& output_json)
{
    if (input_json.empty())
    {
        output_json =
            R"({"status":"INVALID_MODEL","message":"Empty request JSON."})";
        return 1;
    }

    core::Model model("API Request");
    optimization::OptimizationOptions options;
    std::string error;

    if (!io::JsonIO::parse_model(input_json, model, options, error))
    {
        optimization::OptimizationResult err_result;
        err_result.status = optimization::OptimizationStatus::InvalidModel;
        err_result.message = "JSON parse error: " + error;
        output_json = io::JsonIO::serialize_result(err_result);
        return 1;
    }

    optimization::OptimizationEngine engine;
    const optimization::OptimizationResult result = engine.solve(model, options);

    output_json = io::JsonIO::serialize_result(result);
    return result.is_optimal() ? 0 : 1;
}

} // namespace sovopt::api