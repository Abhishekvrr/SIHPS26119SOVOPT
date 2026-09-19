#include "sovopt/optimization/optimization_result.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace sovopt::optimization
{

const char* to_string(OptimizationStatus status) noexcept
{
    switch (status)
    {
        case OptimizationStatus::Optimal:
            return "OPTIMAL";
        case OptimizationStatus::Feasible:
            return "FEASIBLE";
        case OptimizationStatus::Infeasible:
            return "INFEASIBLE";
        case OptimizationStatus::Unbounded:
            return "UNBOUNDED";
        case OptimizationStatus::IterationLimit:
            return "ITERATION_LIMIT";
        case OptimizationStatus::TimeLimit:
            return "TIME_LIMIT";
        case OptimizationStatus::NodeLimit:
            return "NODE_LIMIT";
        case OptimizationStatus::NumericalError:
            return "NUMERICAL_ERROR";
        case OptimizationStatus::InvalidModel:
            return "INVALID_MODEL";
        case OptimizationStatus::Error:
            return "ERROR";
    }
    return "UNKNOWN";
}

double OptimizationResult::get_variable_value(std::size_t index) const
{
    if (index >= variable_values.size())
    {
        throw std::out_of_range("Variable index out of range in OptimizationResult.");
    }
    return variable_values[index];
}

double OptimizationResult::get_variable_value(const std::string& name) const
{
    for (std::size_t i = 0; i < variable_names.size(); ++i)
    {
        if (variable_names[i] == name)
        {
            if (i < variable_values.size())
            {
                return variable_values[i];
            }
            break;
        }
    }
    throw std::invalid_argument("Variable name '" + name + "' not found in OptimizationResult.");
}

std::unordered_map<std::string, double> OptimizationResult::get_solution_map() const
{
    std::unordered_map<std::string, double> map;
    for (std::size_t i = 0; i < variable_values.size(); ++i)
    {
        const std::string name = (i < variable_names.size() && !variable_names[i].empty())
                                     ? variable_names[i]
                                     : ("x" + std::to_string(i + 1));
        map[name] = variable_values[i];
    }
    return map;
}

std::string OptimizationResult::to_string_summary() const
{
    std::ostringstream oss;
    oss << "----------------------------------------\n"
        << "SOVOPT Optimization Result Summary\n"
        << "----------------------------------------\n"
        << "Status             : " << to_string(status) << "\n"
        << "Problem Type       : " << problem_type << "\n"
        << "Solver Used        : " << solver_used << "\n"
        << "Iterations         : " << iterations << "\n";
    if (nodes_explored > 0)
    {
        oss << "Nodes Explored     : " << nodes_explored << "\n";
    }
    oss << std::fixed << std::setprecision(6)
        << "Objective Value    : " << objective_value << "\n"
        << "Solve Time         : " << solve_time_ms << " ms\n"
        << "Validation         : " << (is_valid ? "PASSED" : "FAILED") << "\n"
        << "Primal Violation   : " << max_primal_violation << "\n"
        << "Message            : " << message << "\n";

    if (!variable_values.empty())
    {
        oss << "\nDecision Variables:\n";
        for (std::size_t i = 0; i < variable_values.size(); ++i)
        {
            const std::string name = (i < variable_names.size() && !variable_names[i].empty())
                                         ? variable_names[i]
                                         : ("x" + std::to_string(i + 1));
            oss << "  " << name << " = " << variable_values[i] << "\n";
        }
    }
    oss << "----------------------------------------\n";
    return oss.str();
}

} // namespace sovopt::optimization

