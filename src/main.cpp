#include "sovopt/api/solver_api.h"
#include "sovopt/core/model.h"
#include "sovopt/io/json_io.h"
#include "sovopt/optimization/optimization_engine.h"
#include "sovopt/optimization/optimization_options.h"
#include "sovopt/optimization/optimization_result.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

int main(int argc, char* argv[])
{
    using namespace sovopt::core;
    using namespace sovopt::optimization;

    // Check CLI arguments for JSON mode
    if (argc >= 2)
    {
        std::string flag = argv[1];
        if (flag == "--json")
        {
            std::string input_json;
            if (argc >= 3)
            {
                input_json = argv[2];
            }
            else
            {
                // Read from standard input stream
                std::stringstream buffer;
                buffer << std::cin.rdbuf();
                input_json = buffer.str();
            }

            std::string output_json;
            sovopt::api::SolverAPI::run(input_json, output_json);
            std::cout << output_json << std::flush;
            return 0;
        }
        else if (flag == "--file" && argc >= 3)
        {
            std::ifstream file(argv[2]);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open JSON file " << argv[2] << std::endl;
                return 1;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string output_json;
            sovopt::api::SolverAPI::run(buffer.str(), output_json);
            std::cout << output_json << std::flush;
            return 0;
        }
        else if (flag == "--help" || flag == "-h")
        {
            std::cout << "SOVOPT — Sovereign Mathematical Optimization Engine\n"
                      << "Usage:\n"
                      << "  sovopt.exe                    (Run built-in Production Planning Demo)\n"
                      << "  sovopt.exe --json \"<json>\"     (Solve problem from JSON string)\n"
                      << "  sovopt.exe --file <path.json> (Solve problem from JSON file)\n";
            return 0;
        }
    }

    // Default Interactive Console Demo
    std::cout
        << "========================================\n"
        << "              SOVOPT\n"
        << " Sovereign Mathematical Optimization\n"
        << "      Optimization Engine Core\n"
        << "========================================\n\n";

    try
    {
        std::cout << "[1] Creating mathematical model...\n";
        Model model("Production Planning Demo");

        std::cout << "[2] Adding decision variables...\n";
        const VariableIndex x1 = model.add_variable("x1", VariableType::Continuous, 0.0, Infinity);
        const VariableIndex x2 = model.add_variable("x2", VariableType::Continuous, 0.0, Infinity);

        std::cout << "[3] Configuring objective function (MAXIMIZE 3x1 + 5x2)...\n";
        model.set_objective(
            OptimizationSense::Maximize,
            {
                LinearTerm{.variable = x1, .coefficient = 3.0},
                LinearTerm{.variable = x2, .coefficient = 5.0}
            }
        );

        std::cout << "[4] Adding constraint 1: Raw Material (2x1 + x2 <= 8)...\n";
        model.add_constraint(
            "Raw Material",
            {
                LinearTerm{.variable = x1, .coefficient = 2.0},
                LinearTerm{.variable = x2, .coefficient = 1.0}
            },
            ConstraintSense::LessEqual,
            8.0
        );

        std::cout << "[5] Adding constraint 2: Machine Hours (x1 + 2x2 <= 8)...\n";
        model.add_constraint(
            "Machine Hours",
            {
                LinearTerm{.variable = x1, .coefficient = 1.0},
                LinearTerm{.variable = x2, .coefficient = 2.0}
            },
            ConstraintSense::LessEqual,
            8.0
        );

        std::cout << "[6] Initializing OptimizationEngine...\n";
        OptimizationOptions options;
        options.feasibility_tolerance = 1e-9;
        options.optimality_tolerance = 1e-9;
        options.max_iterations = 100000;
        options.enable_validation = true;

        OptimizationEngine engine;

        std::cout << "[7] Solving model...\n";
        const OptimizationResult result = engine.solve(model, options);
        std::cout << "[8] Optimization complete.\n\n";

        std::cout
            << "Model                 : " << model.name() << '\n'
            << "Variables             : " << model.variables().size() << '\n'
            << "Constraints           : " << model.constraints().size() << '\n'
            << "Problem Type          : " << result.problem_type << '\n'
            << "Objective             : "
            << (model.objective().has_value() &&
                model.objective()->sense() == OptimizationSense::Maximize
                    ? "MAXIMIZE"
                    : "MINIMIZE")
            << "\n\n----------------------------------------\n"
            << "Solver Status          : " << to_string(result.status) << '\n'
            << "Solver Used            : " << result.solver_used << '\n'
            << "Iterations             : " << result.iterations << '\n'
            << std::fixed << std::setprecision(6)
            << "Objective Value        : " << result.objective_value << '\n'
            << "Validation Status      : " << (result.is_valid ? "PASSED" : "FAILED") << '\n'
            << "Primal Violation       : " << result.max_primal_violation << '\n'
            << "Solve Time             : " << result.solve_time_ms << " ms\n"
            << "Total Time             : " << result.total_time_ms << " ms\n";

        if (!result.variable_values.empty())
        {
            std::cout << "\nSolution:\n";
            for (std::size_t i = 0; i < result.variable_values.size(); ++i)
            {
                const std::string name = (i < result.variable_names.size() && !result.variable_names[i].empty())
                                             ? result.variable_names[i]
                                             : ("x" + std::to_string(i + 1));
                std::cout << "  " << name << " = " << result.variable_values[i] << '\n';
            }
        }

        std::cout
            << "\nMessage                : " << result.message << '\n'
            << "\n========================================\n";

        return result.is_optimal() ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "\nSOVOPT FATAL ERROR\n"
            << "------------------\n"
            << exception.what()
            << '\n';

        return 1;
    }
    catch (...)
    {
        std::cerr
            << "\nSOVOPT FATAL ERROR\n"
            << "Unknown exception.\n";

        return 1;
    }
}