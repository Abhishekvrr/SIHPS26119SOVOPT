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
    Equal,
    GreaterEqual
};

struct Variable {
    std::string name;
    double lower_bound = 0.0;
    double upper_bound = 1e100;
};

struct Constraint {
    std::string name;
    ConstraintSense sense;
    double rhs = 0.0;
};

class LPModel {
public:
    explicit LPModel(
        ObjectiveSense sense = ObjectiveSense::Minimize);

    std::size_t add_variable(
        const std::string& name,
        double lower_bound = 0.0,
        double upper_bound = 1e100);

    std::size_t add_constraint(
        const std::string& name,
        ConstraintSense sense,
        double rhs);

    void set_objective(
        std::size_t variable,
        double coefficient);

    void set_coefficient(
        std::size_t constraint,
        std::size_t variable,
        double coefficient);

    void set_rhs(
        std::size_t constraint,
        double rhs);

    void set_constraint_sense(
        std::size_t constraint,
        ConstraintSense sense);

    ObjectiveSense objective_sense() const noexcept {
        return objective_sense_;
    }

    const std::vector<Variable>& variables() const noexcept {
        return variables_;
    }

    const std::vector<Constraint>& constraints() const noexcept {
        return constraints_;
    }

    const std::vector<double>& objective() const noexcept {
        return objective_;
    }

    const math::SparseMatrix& matrix() const noexcept {
        return matrix_;
    }

    math::SparseMatrix& matrix() noexcept {
        return matrix_;
    }

    void finalize();

    bool validate(std::string& error) const;

private:
    ObjectiveSense objective_sense_;

    std::vector<Variable> variables_;
    std::vector<Constraint> constraints_;
    std::vector<double> objective_;

    math::SparseMatrix matrix_;

    bool finalized_ = false;
};

} // namespace sovopt::lp