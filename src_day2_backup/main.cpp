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

    // --------------------------------------------------------
    // Example LP
    //
    // Maximize:
    //
    //     3x1 + 5x2
    //
    // Subject to:
    //
    //     2x1 +  x2 <= 8
    //      x1 + 2x2 <= 8
    //
    //     x1, x2 >= 0
    //
    // Expected solution:
    //
    //     x1 = 8/3
    //     x2 = 8/3
    //
    // Objective = 64/3 = 21.333...
    // --------------------------------------------------------

    LPModel model;

    model.name =
        "Production Planning LP";

    model.resize_variables(2);

    model.objective_sense =
        ObjectiveSense::Maximize;

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
        << to_string(model.objective_sense)
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

    if (result.optimal())
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

    return result.optimal() ? 0 : 1;
}
