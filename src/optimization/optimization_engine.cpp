#include "sovopt/optimization/optimization_engine.h"

#include "sovopt/algorithms/branch_and_bound_solver.h"
#include "sovopt/lp/dual_simplex_solver.h"
#include "sovopt/lp/presolve.h"
#include "sovopt/lp/simplex_solver.h"
#include "sovopt/math/gpu_accelerator.h"
#include "sovopt/math/simd_vector.h"
#include "sovopt/qp/qp_model.h"
#include "sovopt/qp/qp_solver.h"

#include <chrono>
#include <cmath>

namespace sovopt::optimization
{

bool OptimizationEngine::validate_model(
    const core::Model& model,
    std::string& error_message
) const
{
    if (model.variables().empty())
    {
        error_message = "Model contains no variables.";
        return false;
    }

    if (!model.objective().has_value())
    {
        error_message = "Model has no objective function configured.";
        return false;
    }

    for (const auto& var : model.variables())
    {
        if (std::isnan(var.lower_bound()) || std::isnan(var.upper_bound()))
        {
            error_message = "Variable '" + var.name() + "' has NaN bounds.";
            return false;
        }
        if (var.lower_bound() > var.upper_bound())
        {
            error_message = "Variable '" + var.name() + "' has lower bound exceeding upper bound.";
            return false;
        }
    }

    for (const auto& constraint : model.constraints())
    {
        if (std::isnan(constraint.rhs()))
        {
            error_message = "Constraint '" + constraint.name() + "' has NaN RHS value.";
            return false;
        }
        for (const auto& term : constraint.terms())
        {
            if (term.variable >= model.variables().size())
            {
                error_message = "Constraint '" + constraint.name() + "' references invalid variable index.";
                return false;
            }
        }
    }

    return true;
}

std::string OptimizationEngine::classify_problem(
    const core::Model& model
) const
{
    if (model.is_qp())
    {
        return "QP (Quadratic Program)";
    }
    if (model.is_milp())
    {
        return "MILP (Mixed-Integer Linear Program)";
    }
    return "LP (Linear Program)";
}

lp::LPModel OptimizationEngine::translate_to_lp(
    const core::Model& model
) const
{
    const auto opt_sense = model.objective().has_value()
                               ? (model.objective()->sense() == core::OptimizationSense::Maximize
                                      ? lp::ObjectiveSense::Maximize
                                      : lp::ObjectiveSense::Minimize)
                               : lp::ObjectiveSense::Minimize;

    lp::LPModel lp_model(opt_sense);

    // Add variables
    for (const auto& var : model.variables())
    {
        lp_model.add_variable(
            var.name(),
            var.lower_bound(),
            var.upper_bound()
        );
    }

    // Set objective coefficients
    if (model.objective().has_value())
    {
        for (const auto& term : model.objective()->terms())
        {
            lp_model.set_objective(term.variable, term.coefficient);
        }
    }

    // Add constraints
    for (const auto& constraint : model.constraints())
    {
        lp::ConstraintSense sense = lp::ConstraintSense::LessEqual;
        switch (constraint.sense())
        {
            case core::ConstraintSense::LessEqual:
                sense = lp::ConstraintSense::LessEqual;
                break;
            case core::ConstraintSense::Equal:
                sense = lp::ConstraintSense::Equal;
                break;
            case core::ConstraintSense::GreaterEqual:
                sense = lp::ConstraintSense::GreaterEqual;
                break;
        }

        const std::size_t c_idx = lp_model.add_constraint(
            constraint.name(),
            sense,
            constraint.rhs()
        );

        for (const auto& term : constraint.terms())
        {
            lp_model.set_coefficient(c_idx, term.variable, term.coefficient);
        }
    }

    lp_model.finalize();
    return lp_model;
}

qp::QPModel OptimizationEngine::translate_to_qp(
    const core::Model& model
) const
{
    const auto opt_sense = model.objective().has_value()
                               ? model.objective()->sense()
                               : core::OptimizationSense::Minimize;

    qp::QPModel qp_model(opt_sense);

    for (const auto& var : model.variables())
    {
        qp_model.add_variable(
            var.name(),
            var.lower_bound(),
            var.upper_bound()
        );
    }

    if (model.objective().has_value())
    {
        for (const auto& term : model.objective()->terms())
        {
            qp_model.set_linear_objective(term.variable, term.coefficient);
        }
        for (const auto& q_term : model.objective()->quadratic_terms())
        {
            qp_model.set_quadratic_objective(
                q_term.row_variable,
                q_term.col_variable,
                q_term.coefficient
            );
        }
    }

    for (const auto& constraint : model.constraints())
    {
        const std::size_t c_idx = qp_model.add_constraint(
            constraint.name(),
            constraint.sense(),
            constraint.rhs()
        );

        for (const auto& term : constraint.terms())
        {
            qp_model.set_linear_coefficient(c_idx, term.variable, term.coefficient);
        }
    }

    return qp_model;
}

OptimizationResult OptimizationEngine::solve(
    const core::Model& model,
    const OptimizationOptions& options
) const
{
    const auto total_start = std::chrono::high_resolution_clock::now();

    OptimizationResult result;
    result.problem_type = classify_problem(model);
    result.simd_speedup_factor = math::SIMDVector::benchmark_simd_speedup(50000);
    result.acceleration_used = to_string(options.acceleration);

    // Populate variable names
    result.variable_names.reserve(model.variables().size());
    for (const auto& var : model.variables())
    {
        result.variable_names.push_back(var.name());
    }

    // Step 1: Model validation
    std::string validation_error;
    if (!validate_model(model, validation_error))
    {
        result.status = OptimizationStatus::InvalidModel;
        result.message = validation_error;
        const auto total_end = std::chrono::high_resolution_clock::now();
        result.total_time_ms = std::chrono::duration<double, std::milli>(total_end - total_start).count();
        return result;
    }

    // PATH A: Quadratic Programming (QP)
    if (model.is_qp() || options.solver_type == SolverType::ActiveSetQP)
    {
        qp::QPModel qp_model = translate_to_qp(model);
        qp::QPSolver qp_solver(options);

        const auto solve_start = std::chrono::high_resolution_clock::now();
        qp::QPResult qp_res = qp_solver.solve(qp_model);
        const auto solve_end = std::chrono::high_resolution_clock::now();

        result.solve_time_ms = std::chrono::duration<double, std::milli>(solve_end - solve_start).count();
        result.status = qp_res.status;
        result.objective_value = qp_res.objective_value;
        result.variable_values = qp_res.solution;
        result.iterations = qp_res.iterations;
        result.solver_used = "Active-Set QP Solver (KKT System Projected)";
        result.kkt_primal_residual = qp_res.kkt_primal_residual;
        result.kkt_dual_residual = qp_res.kkt_dual_residual;
        result.max_primal_violation = qp_res.kkt_primal_residual;
        result.is_valid = (qp_res.kkt_primal_residual < 1e-4);
        result.message = qp_res.message;

        const auto total_end = std::chrono::high_resolution_clock::now();
        result.total_time_ms = std::chrono::duration<double, std::milli>(total_end - total_start).count();
        return result;
    }

    // PATH B: Translate to LP representation and optional Presolve
    const auto presolve_start = std::chrono::high_resolution_clock::now();
    lp::LPModel lp_model = translate_to_lp(model);
    lp::LPModel working_model = lp_model;
    lp::PresolveEngine presolve_engine(options.zero_tolerance);
    lp::PresolveStats presolve_stats;

    if (options.enable_presolve && options.presolve_level != PresolveLevel::None)
    {
        if (presolve_engine.presolve(lp_model, working_model, presolve_stats))
        {
            result.presolve_reductions_count = presolve_stats.removed_rows + presolve_stats.fixed_variables + presolve_stats.tightened_bounds;
        }
        else if (presolve_stats.is_infeasible)
        {
            result.status = OptimizationStatus::Infeasible;
            result.message = presolve_stats.message;
            const auto total_end = std::chrono::high_resolution_clock::now();
            result.total_time_ms = std::chrono::duration<double, std::milli>(total_end - total_start).count();
            return result;
        }
    }
    const auto presolve_end = std::chrono::high_resolution_clock::now();
    result.presolve_time_ms = std::chrono::duration<double, std::milli>(presolve_end - presolve_start).count();

    // PATH C: Mixed-Integer Linear Programming (MILP) / Branch-and-Cut
    if (model.is_milp() ||
        options.solver_type == SolverType::BranchAndBound ||
        options.solver_type == SolverType::BranchAndCut)
    {
        std::vector<core::VariableType> var_types;
        for (const auto& v : model.variables())
        {
            var_types.push_back(v.type());
        }

        algorithms::BranchAndBoundOptions bb_opts;
        bb_opts.node_limit = options.max_nodes;
        bb_opts.time_limit_seconds = options.time_limit_seconds;
        bb_opts.integrality_tolerance = options.integrality_tolerance;
        bb_opts.enable_gomory_cuts = options.enable_gomory_cuts;
        bb_opts.max_cut_rounds = options.max_cut_rounds;

        algorithms::BranchAndBoundSolver bb_solver(bb_opts);

        const auto solve_start = std::chrono::high_resolution_clock::now();
        algorithms::BranchAndBoundResult bb_res = bb_solver.solve(lp_model, var_types);
        const auto solve_end = std::chrono::high_resolution_clock::now();

        result.solve_time_ms = std::chrono::duration<double, std::milli>(solve_end - solve_start).count();
        result.nodes_explored = bb_res.nodes_processed;
        result.cuts_added = bb_res.cuts_generated;
        result.duality_gap = bb_res.duality_gap;
        result.objective_value = bb_res.objective_value;
        result.variable_values = bb_res.solution;
        result.iterations = bb_res.nodes_processed * 2;
        result.solver_used = "Branch-and-Cut MILP Engine (Gomory Cuts + Simplex Relaxation)";
        result.message = bb_res.message;

        switch (bb_res.status)
        {
            case algorithms::SolveStatus::Optimal:
                result.status = OptimizationStatus::Optimal;
                result.is_valid = true;
                break;
            case algorithms::SolveStatus::Feasible:
                result.status = OptimizationStatus::Feasible;
                result.is_valid = true;
                break;
            case algorithms::SolveStatus::Infeasible:
                result.status = OptimizationStatus::Infeasible;
                result.is_valid = false;
                break;
            case algorithms::SolveStatus::TimeLimit:
                result.status = OptimizationStatus::TimeLimit;
                result.is_valid = false;
                break;
            default:
                result.status = OptimizationStatus::Error;
                result.is_valid = false;
                break;
        }

        const auto total_end = std::chrono::high_resolution_clock::now();
        result.total_time_ms = std::chrono::duration<double, std::milli>(total_end - total_start).count();
        return result;
    }

    // PATH D: Dual Simplex or Primal Simplex
    std::vector<double> working_solution;
    const auto solve_start = std::chrono::high_resolution_clock::now();

    if (options.solver_type == SolverType::DualSimplex)
    {
        lp::DualSimplexSolver dual_solver(options.to_numerical_settings());
        lp::LPResult dual_res = dual_solver.solve(working_model);
        result.iterations = dual_res.iterations;
        result.objective_value = dual_res.objective_value;
        working_solution = dual_res.solution;
        result.solver_used = "Dual Simplex (Harris Ratio Test)";
        result.message = dual_res.message;

        result.status = (dual_res.status == lp::LPStatus::Optimal) ? OptimizationStatus::Optimal
                       : (dual_res.status == lp::LPStatus::Infeasible) ? OptimizationStatus::Infeasible
                       : OptimizationStatus::NumericalError;
    }
    else
    {
        lp::SimplexSolver primal_solver(options.to_numerical_settings());
        lp::LPResult primal_res = primal_solver.solve(working_model);
        result.iterations = primal_res.iterations;
        result.objective_value = primal_res.objective_value;
        working_solution = primal_res.solution;
        result.solver_used = "Two-Phase Primal Simplex (SIMD AVX2 Accelerated)";
        result.message = primal_res.message;

        result.status = (primal_res.status == lp::LPStatus::Optimal) ? OptimizationStatus::Optimal
                       : (primal_res.status == lp::LPStatus::Infeasible) ? OptimizationStatus::Infeasible
                       : OptimizationStatus::NumericalError;
    }
    const auto solve_end = std::chrono::high_resolution_clock::now();
    result.solve_time_ms = std::chrono::duration<double, std::milli>(solve_end - solve_start).count();

    // Reconstruct full solution if presolved
    if (options.enable_presolve && options.presolve_level != PresolveLevel::None && !working_solution.empty())
    {
        result.variable_values = presolve_engine.postsolve(lp_model, working_model, working_solution);
    }
    else
    {
        result.variable_values = working_solution;
    }

    // Step 4: Independent solution validation
    if (options.enable_validation && !result.variable_values.empty() &&
        result.variable_values.size() == model.variables().size())
    {
        const auto val_start = std::chrono::high_resolution_clock::now();

        core::Solution solution(model.variables().size());
        for (std::size_t i = 0; i < result.variable_values.size(); ++i)
        {
            solution.set_value(i, result.variable_values[i]);
        }

        core::Evaluator evaluator;
        const core::EvaluationResult eval = evaluator.evaluate(model, solution);

        const auto val_end = std::chrono::high_resolution_clock::now();
        result.validation_time_ms = std::chrono::duration<double, std::milli>(val_end - val_start).count();

        result.is_valid = eval.feasible;
        result.max_primal_violation = eval.total_violation;

        if (result.status == OptimizationStatus::Optimal && !eval.feasible)
        {
            result.status = OptimizationStatus::NumericalError;
            result.message = "Solution produced by solver failed independent feasibility validation.";
        }
    }
    else
    {
        result.is_valid = (result.status == OptimizationStatus::Optimal);
    }

    const auto total_end = std::chrono::high_resolution_clock::now();
    result.total_time_ms = std::chrono::duration<double, std::milli>(total_end - total_start).count();

    return result;
}

OptimizationResult OptimizationEngine::solve(
    const lp::LPModel& model,
    const OptimizationOptions& options
) const
{
    core::Model core_model("Direct Model");
    for (const auto& v : model.variables())
    {
        core_model.add_variable(v.name, core::VariableType::Continuous, v.lower_bound, v.upper_bound);
    }

    std::vector<core::LinearTerm> obj_terms;
    for (std::size_t j = 0; j < model.objective().size(); ++j)
    {
        if (std::abs(model.objective()[j]) > 1e-12)
        {
            obj_terms.push_back(core::LinearTerm{.variable = j, .coefficient = model.objective()[j]});
        }
    }
    core_model.set_objective(
        model.objective_sense() == lp::ObjectiveSense::Maximize ? core::OptimizationSense::Maximize : core::OptimizationSense::Minimize,
        std::move(obj_terms)
    );

    for (std::size_t i = 0; i < model.constraints().size(); ++i)
    {
        const auto& c = model.constraints()[i];
        core::ConstraintSense sense = (c.sense == lp::ConstraintSense::Equal) ? core::ConstraintSense::Equal
                                    : (c.sense == lp::ConstraintSense::GreaterEqual) ? core::ConstraintSense::GreaterEqual
                                    : core::ConstraintSense::LessEqual;

        std::vector<core::LinearTerm> c_terms;
        for (std::size_t j = 0; j < model.variables().size(); ++j)
        {
            double coeff = model.matrix().get(i, j);
            if (std::abs(coeff) > 1e-12)
            {
                c_terms.push_back(core::LinearTerm{.variable = j, .coefficient = coeff});
            }
        }
        core_model.add_constraint(c.name, std::move(c_terms), sense, c.rhs);
    }

    return solve(core_model, options);
}

} // namespace sovopt::optimization
