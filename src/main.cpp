#include "sovopt/lp/lp_solver.h"

#include <iomanip>
#include <iostream>

int main()
{
    using namespace sovopt::lp;

    std::cout
        << "========================================\n"
        << "              SOVOPT\n"
        << " Sovereign Mathematical Optimization\n"
        << "          LP Solver Core\n"
        << "========================================\n\n";

    LPModel model;

    model.name = "Production Planning LP";

    model.resize_variables(2);

    model.objective_sense = ObjectiveSense::Maximize;

    model.set_objective({
        3.0,
        5.0
    });

    model.add_constraint(
        {
            2.0,
            1.0
        },
        ConstraintSense::LessEqual,
        8.0
    );

    model.add_constraint(
        {
            1.0,
            2.0
        },
        ConstraintSense::LessEqual,
        8.0
    );

    SimplexSolver::Options options;

    options.tolerance = 1e-9;
    options.maximum_iterations = 100000;

    SimplexSolver solver(options);

    const LPResult result =
        solver.solve(model);

    std::cout
        << "Model                 : "
        << model.name
        << '\n';

    std::cout
        << "Variables             : "
        << model.variable_count
        << '\n';

    std::cout
        << "Constraints           : "
        << model.constraints.size()
        << '\n';

    std::cout
        << "Objective              : "
        << (
            model.objective_sense == ObjectiveSense::Maximize
                ? "MAXIMIZE"
                : "MINIMIZE"
        )
        << '\n';

    std::cout
        << "\n----------------------------------------\n";

    std::cout
        << "Solver Status          : "
        << to_string(result.status)
        << '\n';

    std::cout
        << "Iterations             : "
        << result.iterations
        << '\n';

    if (result.status == LPStatus::Optimal)
    {
        std::cout
            << std::fixed
            << std::setprecision(6);

        std::cout
            << "Objective Value        : "
            << result.objective_value
            << '\n';

        std::cout
            << "\nSolution:\n";

        for (std::size_t i = 0;
             i < result.solution.size();
             ++i)
        {
            std::cout
                << "  x"
                << (i + 1)
                << " = "
                << result.solution[i]
                << '\n';
        }
    }

    std::cout
        << "\nMessage                : "
        << result.message
        << '\n';

    std::cout
        << "\n========================================\n";

    return result.status == LPStatus::Optimal ? 0 : 1;
}
