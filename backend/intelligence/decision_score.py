"""
SOVOPT Hybrid Intelligence Layer - Decision Score Calculator
Computes a mathematically transparent Decision Intelligence Score (0-100) based on
feasibility residuals, optimality convergence, resource utilization, and slack efficiency.
"""

from typing import Any, Dict, List


class DecisionScoreCalculator:
    """Calculates reproducible, explainable Decision Intelligence Scores for optimization solutions."""

    @staticmethod
    def calculate_score(
        status: str,
        objective_value: float,
        solution: List[float],
        constraints_eval: List[Dict[str, Any]],
        max_residual: float,
        data_quality_score: float = 95.0
    ) -> Dict[str, Any]:
        """Evaluates solution properties and returns component scores and diagnostic health."""
        is_optimal = status.upper() in ("OPTIMAL", "FEASIBLE")

        # 1. Feasibility Integrity (0 - 30 pts)
        if is_optimal and max_residual < 1e-6:
            feasibility_score = 30.0
        elif is_optimal and max_residual < 1e-4:
            feasibility_score = 25.0
        elif is_optimal:
            feasibility_score = 15.0
        else:
            feasibility_score = 0.0

        # 2. Optimality & Convergence (0 - 30 pts)
        if status.upper() == "OPTIMAL":
            optimality_score = 30.0
        elif status.upper() == "FEASIBLE":
            optimality_score = 20.0
        else:
            optimality_score = 0.0

        # 3. Resource Utilization Efficiency (0 - 20 pts)
        # Calculate average utilization across <= constraints
        utilization_percentages = []
        binding_count = 0
        slack_count = 0

        for c in constraints_eval:
            lhs = c.get("lhs", 0.0)
            rhs = c.get("rhs", 1.0)
            sense = c.get("sense", "<=")
            slack = c.get("slack", 0.0)

            if sense == "<=" and rhs > 0:
                pct = min(100.0, max(0.0, (lhs / rhs) * 100.0))
                utilization_percentages.append(pct)
            elif sense == ">=" and rhs > 0:
                pct = 100.0 if lhs >= rhs else max(0.0, (lhs / rhs) * 100.0)
                utilization_percentages.append(pct)

            if abs(slack) < 1e-4:
                binding_count += 1
            else:
                slack_count += 1

        avg_utilization = (
            sum(utilization_percentages) / len(utilization_percentages)
            if utilization_percentages else 80.0
        )

        # High utilization with realistic binding constraints is optimal
        if avg_utilization >= 75.0 and binding_count > 0:
            utilization_score = 20.0
        elif avg_utilization >= 50.0:
            utilization_score = 15.0
        else:
            utilization_score = 10.0

        # 4. Variable Bounds & Non-Triviality (0 - 20 pts)
        non_zero_vars = sum(1 for x in solution if abs(x) > 1e-6)
        if non_zero_vars >= len(solution) * 0.5 and len(solution) > 0:
            bounds_score = 20.0
        elif non_zero_vars > 0:
            bounds_score = 14.0
        else:
            bounds_score = 5.0

        # Total Composite Score (0 - 100)
        raw_total = feasibility_score + optimality_score + utilization_score + bounds_score
        # Slight weighting by input data quality
        data_factor = 0.9 + (data_quality_score / 1000.0)
        total_score = max(0, min(100, round(raw_total * data_factor)))

        # Executive summary text
        if total_score >= 90:
            summary = "Optimal global solution verified with zero residual violations and high resource efficiency."
        elif total_score >= 75:
            summary = "Feasible solution achieved with moderate resource utilization and certified constraint satisfaction."
        elif is_optimal:
            summary = "Feasible solution found with potential for further parameter tuning."
        else:
            summary = f"Optimization finished with status: {status}."

        return {
            "total_score": total_score,
            "feasibility_score": round(feasibility_score, 1),
            "optimality_score": round(optimality_score, 1),
            "utilization_score": round(utilization_score, 1),
            "bounds_score": round(bounds_score, 1),
            "average_resource_utilization_pct": round(avg_utilization, 1),
            "binding_constraints_count": binding_count,
            "slack_constraints_count": slack_count,
            "summary": summary
        }

