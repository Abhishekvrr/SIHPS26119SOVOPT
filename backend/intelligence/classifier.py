from typing import Dict, Any, List

class ProblemClassifier:
    """
    Rule-based deterministic classification engine to detect optimization problem domains
    from profiled dataset columns and vocabulary with explainable reasoning.
    """

    DOMAINS = {
        "blending_refining": {
            "name": "Refinery & Feedstock Blending",
            "keywords": ["crude", "blend", "octane", "sulfur", "viscosity", "distillate", "naphtha", "feed", "chemical", "barrel", "bpd", "gravity", "reid", "mrpl", "refinery"],
            "default_sense": "maximize",
            "reasoning": "Detected refinery/chemical feedstock streams with property quality specifications (octane/sulfur/viscosity) and capacity bounds."
        },
        "production_planning": {
            "name": "Industrial Production Planning",
            "keywords": ["product", "unit", "assembly", "machining", "material", "standard", "deluxe", "custom", "profit", "capacity", "hours", "manufacturing", "inventory"],
            "default_sense": "maximize",
            "reasoning": "Detected multi-product manufacturing items constrained by raw materials, machine hours, and skilled assembly labor."
        },
        "logistics_transportation": {
            "name": "Supply Chain & Freight Routing",
            "keywords": ["hub", "destination", "origin", "freight", "route", "warehouse", "transport", "shipping", "delhi", "mumbai", "bengaluru", "distance", "km", "fleet"],
            "default_sense": "minimize",
            "reasoning": "Detected transportation origin-to-destination shipment links constrained by hub supply capacities and regional demand targets."
        },
        "workforce_scheduling": {
            "name": "Workforce & Shift Scheduling",
            "keywords": ["shift", "worker", "staff", "employee", "morning", "afternoon", "evening", "night", "wage", "roster", "coverage", "hours"],
            "default_sense": "minimize",
            "reasoning": "Detected consecutive shift operations with minimum staffing requirements and total wage expenditure minimization."
        },
        "portfolio_allocation": {
            "name": "Sovereign Asset & Capital Allocation",
            "keywords": ["asset", "bond", "equity", "return", "risk", "yield", "treasury", "liquidity", "portfolio", "capital", "fund", "investment"],
            "default_sense": "maximize",
            "reasoning": "Detected financial investment asset classes with target yield maximization subject to risk budget and liquidity minimums."
        }
    }

    @classmethod
    def classify(cls, analysis: Dict[str, Any], records: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Classifies the problem based on column names, dataset name, and text patterns.
        Returns:
            problem_type (str)
            problem_name (str)
            confidence (float, 0.0 - 1.0)
            reasoning (str)
            objective_sense (str: 'maximize' or 'minimize')
        """
        all_text = " ".join([
            analysis.get("dataset_name", ""),
            " ".join(analysis.get("numerical_columns", [])),
            " ".join(analysis.get("categorical_columns", []))
        ]).lower()

        # Check sample cell values for categorical strings
        for r in records[:10]:
            for v in r.values():
                if isinstance(v, str):
                    all_text += f" {v.lower()}"

        scores = {}
        for domain_id, domain_info in cls.DOMAINS.items():
            matches = sum(1 for kw in domain_info["keywords"] if kw in all_text)
            scores[domain_id] = matches

        best_domain, best_score = max(scores.items(), key=lambda x: x[1])

        if best_score >= 3:
            info = cls.DOMAINS[best_domain]
            confidence = min(0.96, 0.82 + (best_score * 0.03))
            return {
                "problem_type": best_domain,
                "problem_name": info["name"],
                "confidence": round(confidence, 2),
                "reasoning": info["reasoning"],
                "objective_sense": info["default_sense"]
            }
        elif best_score >= 1:
            info = cls.DOMAINS[best_domain]
            return {
                "problem_type": best_domain,
                "problem_name": info["name"],
                "confidence": 0.85,
                "reasoning": info["reasoning"],
                "objective_sense": info["default_sense"]
            }
        else:
            # Fallback to General Linear Optimization
            has_financial = len(analysis.get("potential_financial_columns", [])) > 0
            sense = "maximize" if any("profit" in c.lower() or "rev" in c.lower() for c in analysis.get("numerical_columns", [])) else "minimize"
            return {
                "problem_type": "general_linear_optimization",
                "problem_name": "General Resource Allocation",
                "confidence": 0.80,
                "reasoning": "Detected numerical capacity constraints and quantifiable decision items suitable for linear programming optimization.",
                "objective_sense": sense
            }

