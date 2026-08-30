#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace sovopt::lp
{

enum class ObjectiveSense
{
    Maximize,
    Minimize
};

enum class ConstraintSense
{
    LessEqual,
    GreaterEqual,
    Equal
};

enum class LPStatus
{
    Optimal,
    Infeasible,
    Unbounded,
    IterationLimit,
    NumericalError
};

struct LPConstraint
{
    std::vector<double> coefficients;
    ConstraintSense sense;
    double rhs;
};

struct LPModel
{
    std::string name;

    std::size_t variable_count{0};

    ObjectiveSense objective_sense{
        ObjectiveSense::Maximize
    };

    std::vector<double> objective;

    std::vector<LPConstraint> constraints;

    void resize_variables(std::size_t count);

    void set_objective(
        const std::vector<double>& coefficients
    );

    void add_constraint(
        const std::vector<double>& coefficients,
        ConstraintSense sense,
        double rhs
    );
};

struct LPResult
{
    LPStatus status{LPStatus::NumericalError};

    double objective_value{0.0};

    std::vector<double> solution;

    std::size_t iterations{0};

    std::string message;

    bool optimal() const noexcept
    {
        return status == LPStatus::Optimal;
    }
};

class SimplexSolver
{
public:

    struct Options
    {
        double tolerance{1e-9};

        std::size_t maximum_iterations{100000};
    };

    explicit SimplexSolver(
        Options options = {}
    );

    LPResult solve(
        const LPModel& model
    ) const;

private:

    Options options_;

    LPResult solve_maximization(
        const LPModel& model
    ) const;
};

const char* to_string(
    LPStatus status
) noexcept;

const char* to_string(
    ObjectiveSense sense
) noexcept;

const char* to_string(
    ConstraintSense sense
) noexcept;

} // namespace sovopt::lp
