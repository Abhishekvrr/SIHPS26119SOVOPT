#pragma once

#include "sovopt/math/sparse_matrix.h"

#include <cstddef>
#include <string>
#include <vector>

namespace sovopt::lp {

enum class ObjectiveSense {
    Minimize,
    Maximize
};

enum class ConstraintSense {
    LessEqual,
    GreaterEqual,
    Equal
};

struct LPConstraint {
    std::vector<std::pair<std::size_t, double>> coefficients;
    ConstraintSense sense = ConstraintSense::LessEqual;
    double rhs = 0.0;
};

class LPModel {
public:
    explicit LPModel(std::size_t variables = 0);

    std::size_t add_variable(double objective_coefficient = 0.0);

    void set_objective_sense(ObjectiveSense sense);

    void set_objective_coefficient(
        std::size_t variable,
        double coefficient);

    void set_lower_bound(
        std::size_t variable,
        double lower_bound);

    void set_upper_bound(
        std::size_t variable,
        double upper_bound);

    void add_constraint(
        std::vector<std::pair<std::size_t, double>> coefficients,
        ConstraintSense sense,
        double rhs);

    [[nodiscard]] std::size_t variable_count() const noexcept;
    [[nodiscard]] std::size_t constraint_count() const noexcept;

    [[nodiscard]] ObjectiveSense objective_sense() const noexcept;

    [[nodiscard]] const std::vector<double>& objective() const noexcept;

    [[nodiscard]] const std::vector<double>& lower_bounds() const noexcept;
    [[nodiscard]] const std::vector<double>& upper_bounds() const noexcept;

    [[nodiscard]] const std::vector<LPConstraint>&
    constraints() const noexcept;

    [[nodiscard]] math::SparseMatrix build_sparse_matrix() const;

private:
    std::vector<double> objective_;
    std::vector<double> lower_bounds_;
    std::vector<double> upper_bounds_;

    ObjectiveSense objective_sense_ =
        ObjectiveSense::Maximize;

    std::vector<LPConstraint> constraints_;
};

} // namespace sovopt::lp
