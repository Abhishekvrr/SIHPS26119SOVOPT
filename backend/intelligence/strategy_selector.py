"""
SOVOPT Hybrid Intelligence Layer - Strategy Selector
Selects and parameterizes the native optimization algorithm based on problem type and matrix sparsity.
"""

from typing import Any, Dict


class StrategySelector:
    """Selects the mathematical solver algorithm and numerical tolerances for the SOVOPT engine."""

    STRATEGIES = {
        "LP": {
            "algorithm": "Two-Phase Primal Simplex",
            "matrix_format": "Compressed Sparse Row (CSR) / Dense Tableau",
            "feasibility_tolerance": 1e-9,
            "optimality_tolerance": 1e-9,
            "pricing_rule": "Dantzig Most-Negative Reduced Cost",
            "time_complexity": "O(m² n) polynomial average-case",
            "description": "Two-Phase Primal Simplex using exact Gaussian Jordan pivot elimination with numerically certified zero-residual feasibility."
        },
        "MILP": {
            "algorithm": "Branch-and-Bound with Simplex Node Relaxation",
            "matrix_format": "Sparse Matrix with Node Bound Tree",
            "feasibility_tolerance": 1e-7,
            "optimality_tolerance": 1e-7,
            "pricing_rule": "Most Fractional Variable Branching",
            "time_complexity": "Exponential worst-case, pruned branch tree",
            "description": "Branch-and-Bound tree search relaxing integer variables to linear Simplex nodes with dual bound pruning."
        },
        "QP": {
            "algorithm": "Active-Set Quadratic Programming",
            "matrix_format": "KKT System Matrix with Symmetric Hessian",
            "feasibility_tolerance": 1e-8,
            "optimality_tolerance": 1e-8,
            "pricing_rule": "Lagrange Multiplier Gradient Projection",
            "time_complexity": "Iterative KKT linear system solves",
            "description": "Active-Set strategy solving successive quadratic subproblems under linear inequality constraints."
        }
    }

    @staticmethod
    def select_strategy(problem_type: str, num_vars: int, num_constraints: int) -> Dict[str, Any]:
        """Returns the optimal algorithmic strategy and execution parameters."""
        p_type = problem_type.upper() if problem_type else "LP"
        base_strat = StrategySelector.STRATEGIES.get(p_type, StrategySelector.STRATEGIES["LP"])

        # Determine sparse vs dense threshold
        is_sparse = (num_vars * num_constraints) > 1000
        matrix_rep = "Sparse COO / CSR" if is_sparse else "Dense Tableau Matrix"

        return {
            "selected_solver": base_strat["algorithm"],
            "problem_type": p_type,
            "matrix_representation": matrix_rep,
            "pricing_rule": base_strat["pricing_rule"],
            "feasibility_tolerance": base_strat["feasibility_tolerance"],
            "optimality_tolerance": base_strat["optimality_tolerance"],
            "time_complexity": base_strat["time_complexity"],
            "description": base_strat["description"],
            "dimensions": f"{num_vars} variables × {num_constraints} constraints"
        }

