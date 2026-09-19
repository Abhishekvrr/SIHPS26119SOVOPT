#include "sovopt/core/model.h"
#include "sovopt/core/variable.h"
#include "sovopt/core/constraint.h"
#include "sovopt/core/objective.h"
#include "sovopt/core/evaluator.h"
#include "sovopt/core/types.h"
#include "sovopt/optimization/optimization_engine.h"
#include "sovopt/optimization/optimization_options.h"
#include "sovopt/optimization/optimization_result.h"
#include "sovopt/math/vector.h"
#include "sovopt/math/sparse_matrix.h"
#include "sovopt/math/simd_vector.h"
#include "sovopt/qp/qp_model.h"
#include "sovopt/qp/qp_solver.h"

#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#define TEST_ASSERT(cond, msg) \
    if (!(cond)) { \
        std::cerr << "[FAIL] " << msg << " (Line " << __LINE__ << ")" << std::endl; \
        failed++; \
    } else { \
        std::cout << "[PASS] " << msg << std::endl; \
        passed++; \
    }

int main()
{
    int passed = 0;
    int failed = 0;

    std::cout << "====================================================" << std::endl;
    std::cout << "  SOVOPT C++20 Core Optimization Solver Unit Tests  " << std::endl;
    std::cout << "====================================================" << std::endl << std::endl;

    // TEST 1: Variable Construction & Bounds
    {
        sovopt::core::Variable v(0, "x1", sovopt::core::VariableType::Continuous, 0.0, 100.0);
        TEST_ASSERT(v.name() == "x1", "Variable name check");
        TEST_ASSERT(v.lower_bound() == 0.0, "Variable lower bound check");
        TEST_ASSERT(v.upper_bound() == 100.0, "Variable upper bound check");
        TEST_ASSERT(v.type() == sovopt::core::VariableType::Continuous, "Variable continuous type check");
    }

    // TEST 2: Constraint Construction & Senses
    {
        std::vector<sovopt::core::LinearTerm> terms = {{0, 2.0}, {1, 3.0}};
        sovopt::core::Constraint c1(0, "C_LE", terms, sovopt::core::ConstraintSense::LessEqual, 50.0);
        TEST_ASSERT(c1.sense() == sovopt::core::ConstraintSense::LessEqual, "Constraint <= check");
        TEST_ASSERT(c1.rhs() == 50.0, "Constraint RHS check");
        TEST_ASSERT(c1.terms().size() == 2, "Constraint terms size");
        TEST_ASSERT(c1.terms()[0].coefficient == 2.0, "Constraint coeff 0");
        TEST_ASSERT(c1.terms()[1].coefficient == 3.0, "Constraint coeff 1");
    }

    // TEST 3: Vector Math & Sparse Matrix
    {
        sovopt::math::Vector vec(3, 2.5);
        TEST_ASSERT(vec.size() == 3, "Vector size");
        TEST_ASSERT(vec[0] == 2.5 && vec[1] == 2.5, "Vector init");
        TEST_ASSERT(std::abs(vec.norm_inf() - 2.5) < 1e-7, "Vector norm_inf");

        sovopt::math::SparseMatrix mat(2, 2);
        mat.set(0, 0, 4.0);
        mat.set(1, 1, 5.0);
        TEST_ASSERT(mat.get(0, 0) == 4.0, "Sparse matrix get (0,0)");
        TEST_ASSERT(mat.get(0, 1) == 0.0, "Sparse matrix get zero (0,1)");
        TEST_ASSERT(mat.nonZeros() == 2, "Sparse matrix non-zeros");
    }

    // TEST 4: Phase 3 Standard Production Model (Maximize)
    {
        sovopt::core::Model model("Production Maximize");
        auto v1 = model.add_variable("x1", sovopt::core::VariableType::Continuous, 0.0, sovopt::core::Infinity);
        auto v2 = model.add_variable("x2", sovopt::core::VariableType::Continuous, 0.0, sovopt::core::Infinity);

        model.set_objective(sovopt::core::OptimizationSense::Maximize, {
            {v1, 40.0},
            {v2, 30.0}
        });

        model.add_constraint("C1", {{v1, 2.0}, {v2, 1.0}}, sovopt::core::ConstraintSense::LessEqual, 100.0);
        model.add_constraint("C2", {{v1, 1.0}, {v2, 2.0}}, sovopt::core::ConstraintSense::LessEqual, 80.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "Production model optimality");
        TEST_ASSERT(std::abs(result.objective_value - 2200.0) < 1e-4, "Production model obj value == 2200");
        TEST_ASSERT(std::abs(result.get_variable_value("x1") - 40.0) < 1e-4, "Production model x1 == 40");
        TEST_ASSERT(std::abs(result.get_variable_value("x2") - 20.0) < 1e-4, "Production model x2 == 20");
        TEST_ASSERT(result.max_primal_violation < 1e-7, "Production model zero primal residual");
    }

    // TEST 5: Minimize LP Model with >= constraints
    {
        sovopt::core::Model model("Diet Minimize");
        auto v1 = model.add_variable("x1", sovopt::core::VariableType::Continuous, 0.0, sovopt::core::Infinity);
        auto v2 = model.add_variable("x2", sovopt::core::VariableType::Continuous, 0.0, sovopt::core::Infinity);

        model.set_objective(sovopt::core::OptimizationSense::Minimize, {
            {v1, 2.0},
            {v2, 3.0}
        });

        model.add_constraint("C1", {{v1, 1.0}, {v2, 1.0}}, sovopt::core::ConstraintSense::GreaterEqual, 6.0);
        model.add_constraint("C2", {{v1, 2.0}, {v2, 1.0}}, sovopt::core::ConstraintSense::GreaterEqual, 8.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "Minimize model optimality");
        TEST_ASSERT(std::abs(result.objective_value - 12.0) < 1e-4, "Minimize model obj == 12 (global minimum)");
        TEST_ASSERT(std::abs(result.get_variable_value("x1") - 6.0) < 1e-4, "Minimize model x1 == 6");
        TEST_ASSERT(std::abs(result.get_variable_value("x2") - 0.0) < 1e-4, "Minimize model x2 == 0");
    }

    // TEST 6: Infeasible Model Detection
    {
        sovopt::core::Model model("Infeasible Model");
        auto v1 = model.add_variable("x1", sovopt::core::VariableType::Continuous, 0.0, sovopt::core::Infinity);
        auto v2 = model.add_variable("x2", sovopt::core::VariableType::Continuous, 0.0, sovopt::core::Infinity);

        model.set_objective(sovopt::core::OptimizationSense::Maximize, {
            {v1, 1.0},
            {v2, 1.0}
        });

        model.add_constraint("C1", {{v1, 1.0}, {v2, 1.0}}, sovopt::core::ConstraintSense::LessEqual, 2.0);
        model.add_constraint("C2", {{v1, 1.0}, {v2, 1.0}}, sovopt::core::ConstraintSense::GreaterEqual, 5.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.status == sovopt::optimization::OptimizationStatus::Infeasible, "Infeasible detection");
    }

    // TEST 7: Decimal & Negative Coefficients
    {
        sovopt::core::Model model("Decimal Model");
        auto v1 = model.add_variable("x1", sovopt::core::VariableType::Continuous, 0.0, 20.0);
        auto v2 = model.add_variable("x2", sovopt::core::VariableType::Continuous, 0.0, 100.0);

        model.set_objective(sovopt::core::OptimizationSense::Maximize, {
            {v1, 1.5},
            {v2, -0.5}
        });

        model.add_constraint("C1", {{v1, 0.8}, {v2, 1.2}}, sovopt::core::ConstraintSense::LessEqual, 24.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "Decimal model optimality");
        TEST_ASSERT(std::abs(result.objective_value - 30.0) < 1e-4, "Decimal model obj == 30.0");
        TEST_ASSERT(std::abs(result.get_variable_value("x1") - 20.0) < 1e-4, "Decimal model x1 == 20.0");
        TEST_ASSERT(std::abs(result.get_variable_value("x2") - 0.0) < 1e-4, "Decimal model x2 == 0.0");
    }

    // TEST 8: Equality Constraints (==)
    {
        sovopt::core::Model model("Equality Model");
        auto v1 = model.add_variable("x1", sovopt::core::VariableType::Continuous, 0.0, 6.0);
        auto v2 = model.add_variable("x2", sovopt::core::VariableType::Continuous, 0.0, 100.0);

        model.set_objective(sovopt::core::OptimizationSense::Maximize, {
            {v1, 3.0},
            {v2, 2.0}
        });

        model.add_constraint("C_EQ", {{v1, 1.0}, {v2, 1.0}}, sovopt::core::ConstraintSense::Equal, 10.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "Equality model optimality");
        TEST_ASSERT(std::abs(result.objective_value - 26.0) < 1e-4, "Equality model obj == 26.0");
        TEST_ASSERT(std::abs(result.get_variable_value("x1") - 6.0) < 1e-4, "Equality model x1 == 6.0");
        TEST_ASSERT(std::abs(result.get_variable_value("x2") - 4.0) < 1e-4, "Equality model x2 == 4.0");
    }

    // TEST 9: Dual Simplex Solver
    {
        sovopt::core::Model model("Dual Simplex Test");
        auto v1 = model.add_variable("x1", sovopt::core::VariableType::Continuous, 0.0, 100.0);
        auto v2 = model.add_variable("x2", sovopt::core::VariableType::Continuous, 0.0, 100.0);

        model.set_objective(sovopt::core::OptimizationSense::Minimize, {
            {v1, 3.0},
            {v2, 4.0}
        });

        model.add_constraint("C1", {{v1, 1.0}, {v2, 2.0}}, sovopt::core::ConstraintSense::GreaterEqual, 10.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        options.solver_type = sovopt::optimization::SolverType::DualSimplex;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "Dual Simplex optimality");
        TEST_ASSERT(result.objective_value > 0.0, "Dual Simplex positive objective");
    }

    // TEST 10: Advanced Presolve Engine
    {
        sovopt::core::Model model("Presolve Test");
        auto v1 = model.add_variable("x1", sovopt::core::VariableType::Continuous, 0.0, 100.0);
        auto v2 = model.add_variable("x2", sovopt::core::VariableType::Continuous, 0.0, 100.0);

        // Singleton row: 2*x1 <= 20 -> x1 <= 10
        model.add_constraint("Singleton", {{v1, 2.0}}, sovopt::core::ConstraintSense::LessEqual, 20.0);
        model.add_constraint("C_Main", {{v1, 1.0}, {v2, 1.0}}, sovopt::core::ConstraintSense::LessEqual, 30.0);

        model.set_objective(sovopt::core::OptimizationSense::Maximize, {
            {v1, 5.0},
            {v2, 4.0}
        });

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        options.enable_presolve = true;
        options.presolve_level = sovopt::optimization::PresolveLevel::Basic;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "Presolve model optimality");
        TEST_ASSERT(result.presolve_reductions_count >= 1, "Presolve performed reductions");
        TEST_ASSERT(std::abs(result.get_variable_value("x1") - 10.0) < 1e-4, "Presolve tightened x1 bound == 10.0");
    }

    // TEST 11: Mixed-Integer Linear Programming (MILP Knapsack / Integer bounds)
    {
        sovopt::core::Model model("MILP Knapsack");
        auto x1 = model.add_variable("x1", sovopt::core::VariableType::Integer, 0.0, 10.0);
        auto x2 = model.add_variable("x2", sovopt::core::VariableType::Integer, 0.0, 10.0);

        model.set_objective(sovopt::core::OptimizationSense::Maximize, {
            {x1, 10.0},
            {x2, 14.0}
        });

        model.add_constraint("Capacity", {{x1, 2.0}, {x2, 3.0}}, sovopt::core::ConstraintSense::LessEqual, 7.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        options.solver_type = sovopt::optimization::SolverType::BranchAndBound;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "MILP Knapsack optimality");
        TEST_ASSERT(std::abs(result.objective_value - 34.0) < 1e-4, "MILP Knapsack obj == 34");
        TEST_ASSERT(std::abs(result.get_variable_value("x1") - 2.0) < 1e-4, "MILP Knapsack x1 == 2 (integer)");
        TEST_ASSERT(std::abs(result.get_variable_value("x2") - 1.0) < 1e-4, "MILP Knapsack x2 == 1 (integer)");
        TEST_ASSERT(result.nodes_explored >= 1, "MILP explored branch nodes");
    }

    // TEST 12: MILP with Branch-and-Cut (Gomory Cuts)
    {
        sovopt::core::Model model("Branch and Cut Test");
        auto x1 = model.add_variable("x1", sovopt::core::VariableType::Integer, 0.0, 5.0);
        auto x2 = model.add_variable("x2", sovopt::core::VariableType::Integer, 0.0, 5.0);

        model.set_objective(sovopt::core::OptimizationSense::Maximize, {
            {x1, 8.0},
            {x2, 5.0}
        });

        model.add_constraint("C1", {{x1, 1.0}, {x2, 1.0}}, sovopt::core::ConstraintSense::LessEqual, 6.0);
        model.add_constraint("C2", {{x1, 9.0}, {x2, 5.0}}, sovopt::core::ConstraintSense::LessEqual, 45.0);

        sovopt::optimization::OptimizationEngine engine;
        sovopt::optimization::OptimizationOptions options;
        options.solver_type = sovopt::optimization::SolverType::BranchAndCut;
        options.enable_gomory_cuts = true;
        auto result = engine.solve(model, options);

        TEST_ASSERT(result.is_optimal(), "Branch-and-Cut optimality");
        TEST_ASSERT(result.nodes_explored >= 1, "Branch-and-Cut nodes explored");
        double v1 = result.get_variable_value("x1");
        double v2 = result.get_variable_value("x2");
        TEST_ASSERT(std::abs(v1 - std::round(v1)) < 1e-5, "x1 is integer");
        TEST_ASSERT(std::abs(v2 - std::round(v2)) < 1e-5, "x2 is integer");
    }

    // TEST 13: Quadratic Programming (QP Active-Set Solver)
    {
        sovopt::qp::QPModel qp_model(sovopt::core::OptimizationSense::Minimize);
        auto v1 = qp_model.add_variable("x1", 0.0, 10.0);
        auto v2 = qp_model.add_variable("x2", 0.0, 10.0);

        qp_model.set_quadratic_objective(v1, v1, 1.0); // 0.5 * 1.0 * x1^2
        qp_model.set_quadratic_objective(v2, v2, 1.0); // 0.5 * 1.0 * x2^2

        auto c_idx = qp_model.add_constraint("SumConstraint", sovopt::core::ConstraintSense::GreaterEqual, 2.0);
        qp_model.set_linear_coefficient(c_idx, v1, 1.0);
        qp_model.set_linear_coefficient(c_idx, v2, 1.0);

        sovopt::qp::QPSolver qp_solver;
        auto qp_res = qp_solver.solve(qp_model);

        TEST_ASSERT(qp_res.status == sovopt::optimization::OptimizationStatus::Optimal, "QP Active-Set optimality");
        TEST_ASSERT(std::abs(qp_res.objective_value - 1.0) < 1e-3, "QP objective value == 1.0");
        TEST_ASSERT(std::abs(qp_res.solution[0] - 1.0) < 1e-3, "QP x1 == 1.0");
        TEST_ASSERT(std::abs(qp_res.solution[1] - 1.0) < 1e-3, "QP x2 == 1.0");
        TEST_ASSERT(qp_res.kkt_primal_residual < 1e-4, "QP KKT zero primal residual");
    }

    // TEST 14: SIMD Vectorization Kernels & Speedup
    {
        std::vector<double> a = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
        std::vector<double> b = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
        double dot = sovopt::math::SIMDVector::dot_product(a.data(), b.data(), a.size());
        TEST_ASSERT(std::abs(dot - 72.0) < 1e-9, "SIMD dot product correctness");

        double speedup = sovopt::math::SIMDVector::benchmark_simd_speedup(10000);
        TEST_ASSERT(speedup >= 1.0, "SIMD vectorization speedup factor >= 1.0x");
        std::cout << "SIMD Acceleration Engine: " << sovopt::math::SIMDVector::get_instruction_set()
                  << ", Benchmark Speedup: " << speedup << "x" << std::endl;
    }

    std::cout << std::endl << "====================================================" << std::endl;
    std::cout << "TEST SUMMARY: " << passed << " PASSED, " << failed << " FAILED" << std::endl;
    std::cout << "====================================================" << std::endl;

    return failed == 0 ? 0 : 1;
}
