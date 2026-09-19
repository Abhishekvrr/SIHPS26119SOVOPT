"""
SOVOPT Hybrid Intelligence Layer - Problem Classifier
Rule-based deterministic classification for industrial domains and mathematical problem classes (LP, QP, MILP, NLP).
"""

from typing import Any, Dict, List


class ProblemClassifier:
    """Classifies dataset columns and domain attributes into standard optimization problem formulations."""

    DOMAIN_RULES = [
        {
            "category": "Refinery & Feedstock Blending",
            "industry": "Petroleum & Chemical (SIH Problem Domain / MRPL Reference)",
            "keywords": ["crude", "distillation", "sulfur", "octane", "blend", "barrel", "api gravity", "refinery", "yield"],
            "suggested_sense": "maximize",
            "problem_type": "LP",
            "base_confidence": 0.96,
            "reasoning": "Detected crude distillation streams, sulfur/octane quality limits, and barrel margin attributes. Formulated as a Linear Blending Program to maximize gross margin under quality tolerances."
        },
        {
            "category": "Industrial Production Planning",
            "industry": "Manufacturing & Assembly",
            "keywords": ["machine", "labor", "hours", "production", "assembly", "raw material", "units", "batch", "product"],
            "suggested_sense": "maximize",
            "problem_type": "LP",
            "base_confidence": 0.93,
            "reasoning": "Identified machine hour capacities, raw material inventories, and product profit margins. Formulated as an Industrial Production Planning model."
        },
        {
            "category": "Supply Chain & Freight Routing",
            "industry": "Logistics & Transport",
            "keywords": ["freight", "transport", "route", "shipping", "distance", "fleet", "hub", "delivery", "vehicle"],
            "suggested_sense": "minimize",
            "problem_type": "LP",
            "base_confidence": 0.90,
            "reasoning": "Detected transportation routes, delivery demands, and freight costs. Formulated as a Cost Minimization Network Flow / Transport LP."
        },
        {
            "category": "Sovereign Asset Allocation",
            "industry": "Finance & Investment",
            "keywords": ["asset", "capital", "yield", "bond", "equity", "esg"],
            "suggested_sense": "maximize",
            "problem_type": "LP",
            "base_confidence": 0.92,
            "reasoning": "Identified asset yields, liquidity bounds, and capital budget constraints. Formulated as a Capital Allocation LP."
        },
        {
            "category": "Markowitz Mean-Variance Portfolio",
            "industry": "Quantitative Finance & Investment (QP)",
            "keywords": ["markowitz", "covariance", "volatility", "portfolio", "variance", "risk-return", "risk term"],
            "suggested_sense": "maximize",
            "problem_type": "QP",
            "base_confidence": 0.94,
            "reasoning": "Detected portfolio covariance risk terms and quadratic variance minimization objectives. Formulated as an Active-Set Quadratic Program (QP)."
        },
        {
            "category": "Workforce Shift Scheduling",
            "industry": "Healthcare & Emergency Operations (MILP)",
            "keywords": ["shift", "worker", "staff", "headcount", "roster", "hourly", "overtime", "nurse", "doctor"],
            "suggested_sense": "minimize",
            "problem_type": "MILP",
            "base_confidence": 0.91,
            "reasoning": "Detected discrete whole-integer workforce shift coverage requirements. Formulated as a Mixed-Integer Linear Program (MILP)."
        }
    ]

    @staticmethod
    def classify(analysis: Dict[str, Any], records: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Inspects column names, records, and analysis profiles to classify problem domain and mathematical type."""
        all_text = " ".join([analysis.get("dataset_name", "")] + [c["name"] for c in analysis.get("columns", [])]).lower()
        
        # Check record values for domain hints
        if records:
            for r in records[:5]:
                for v in r.values():
                    if isinstance(v, str):
                        all_text += " " + v.lower()

        matched_rule = None
        highest_score = 0

        for rule in ProblemClassifier.DOMAIN_RULES:
            matches = sum(1 for kw in rule["keywords"] if kw in all_text)
            if matches > highest_score:
                highest_score = matches
                matched_rule = rule

        if matched_rule and highest_score > 0:
            confidence = min(0.98, matched_rule["base_confidence"] + (highest_score * 0.01))
            return {
                "category": matched_rule["category"],
                "industry": matched_rule["industry"],
                "problem_type": matched_rule["problem_type"],
                "suggested_sense": matched_rule["suggested_sense"],
                "confidence": round(confidence, 2),
                "reasoning": matched_rule["reasoning"],
                "matched_keywords_count": highest_score
            }

        # Fallback to General Linear Programming
        return {
            "category": "General Linear Optimization",
            "industry": "Enterprise Analytics",
            "problem_type": "LP",
            "suggested_sense": "maximize",
            "confidence": 0.82,
            "reasoning": "Standard continuous numerical attributes and resource limits detected. Formulated as a canonical Linear Program (LP).",
            "matched_keywords_count": 0
        }

