"""
SOVOPT Hybrid Intelligence Layer - Data Analyzer
Profiles dataset dimensions, statistical distributions, missing ratios, duplicates,
constant columns, and classifies columns into semantic roles (resource, financial, demand, metadata).
"""

import math
from typing import Any, Dict, List, Optional


class DataAnalyzer:
    """Statistical profiling and semantic role categorization of tabular data."""

    RESOURCE_KEYWORDS = [
        "capacity", "limit", "max", "hours", "available", "inventory",
        "stock", "supply", "machine", "labor", "crude", "sulfur", "time", "quota",
        "energy", "water", "steam", "hydrogen", "feed"
    ]
    FINANCIAL_KEYWORDS = [
        "profit", "revenue", "price", "margin", "cost", "return",
        "yield", "income", "expense", "penalty", "rate", "fee"
    ]
    DEMAND_KEYWORDS = [
        "demand", "target", "requirement", "min", "quota", "order", "need", "commit"
    ]

    @staticmethod
    def analyze_dataset(
        name: str,
        records: List[Dict[str, Any]],
        file_size_bytes: int = 0
    ) -> Dict[str, Any]:
        """Profiles columns, detects types and distributions, duplicates, and returns structured quality telemetry."""
        if not records:
            return {
                "dataset_name": name,
                "file_size_bytes": file_size_bytes,
                "row_count": 0,
                "column_count": 0,
                "total_cells": 0,
                "missing_cells_pct": 100.0,
                "data_quality_score": 0.0,
                "quality_status": "Issues Detected",
                "quality_verdict": "Dataset contains no usable records.",
                "issues": ["Dataset is completely empty."],
                "columns": [],
                "numeric_columns": [],
                "categorical_columns": [],
                "constant_columns": [],
                "duplicate_count": 0,
                "semantic_categories": {
                    "resource_columns": [],
                    "financial_columns": [],
                    "demand_columns": [],
                    "metadata_columns": []
                }
            }

        row_count = len(records)
        columns = list(records[0].keys())
        col_count = len(columns)
        total_cells = row_count * col_count
        missing_count = 0

        # Duplicate row check
        seen_rows = set()
        duplicate_count = 0
        for r in records:
            # Create a frozen tuple representation of row values
            row_key = tuple((k, str(v)) for k, v in sorted(r.items()))
            if row_key in seen_rows:
                duplicate_count += 1
            else:
                seen_rows.add(row_key)

        col_profiles: List[Dict[str, Any]] = []
        resource_cols = []
        financial_cols = []
        demand_cols = []
        meta_cols = []
        numeric_cols = []
        categorical_cols = []
        constant_cols = []
        issues_list: List[str] = []

        if duplicate_count > 0:
            issues_list.append(f"Dataset contains {duplicate_count} duplicate record(s).")

        for col in columns:
            vals = [r.get(col) for r in records]
            non_nulls = [v for v in vals if v is not None and v != ""]
            col_missing = len(vals) - len(non_nulls)
            missing_count += col_missing

            if col_missing > 0:
                issues_list.append(f"Column '{col}' contains {col_missing} missing value(s).")

            # Check if column is constant
            unique_non_nulls = set(str(v) for v in non_nulls)
            if len(unique_non_nulls) == 1 and row_count > 1:
                constant_cols.append(col)

            # Detect data type
            numeric_vals = []
            non_numeric_strings = []
            for v in non_nulls:
                if isinstance(v, (int, float)) and not isinstance(v, bool):
                    numeric_vals.append(float(v))
                elif isinstance(v, str):
                    try:
                        numeric_vals.append(float(v))
                    except ValueError:
                        non_numeric_strings.append(v)

            is_numeric = len(numeric_vals) >= (len(non_nulls) * 0.7) and len(numeric_vals) > 0
            col_type = "numerical" if is_numeric else "categorical"

            if col_type == "numerical":
                numeric_cols.append(col)
                if non_numeric_strings:
                    issues_list.append(f"Column '{col}' contains {len(non_numeric_strings)} non-numeric/invalid value(s).")
            else:
                categorical_cols.append(col)

            profile: Dict[str, Any] = {
                "name": col,
                "type": col_type,
                "sample_count": len(non_nulls),
                "missing_count": col_missing,
                "missing_pct": round((col_missing / row_count) * 100.0, 1)
            }

            if is_numeric and numeric_vals:
                profile["min"] = round(min(numeric_vals), 4)
                profile["max"] = round(max(numeric_vals), 4)
                profile["mean"] = round(sum(numeric_vals) / len(numeric_vals), 4)
                variance = sum((x - profile["mean"]) ** 2 for x in numeric_vals) / len(numeric_vals)
                profile["std_dev"] = round(math.sqrt(variance), 4)

            # Semantic categorization
            col_lower = col.lower()
            if any(k in col_lower for k in DataAnalyzer.RESOURCE_KEYWORDS):
                resource_cols.append(col)
                profile["semantic_role"] = "Resource Capacity / Physical Bound"
            elif any(k in col_lower for k in DataAnalyzer.FINANCIAL_KEYWORDS):
                financial_cols.append(col)
                profile["semantic_role"] = "Financial Metric / Objective Weight"
            elif any(k in col_lower for k in DataAnalyzer.DEMAND_KEYWORDS):
                demand_cols.append(col)
                profile["semantic_role"] = "Demand / Operational Requirement"
            else:
                meta_cols.append(col)
                profile["semantic_role"] = "Entity Identifier / Attribute"

            col_profiles.append(profile)

        missing_pct = round((missing_count / total_cells) * 100.0, 1) if total_cells > 0 else 0.0

        # Data Quality Score calculation
        completeness_pts = max(0.0, 50.0 - (missing_pct * 1.5))
        structure_pts = 30.0 if col_count >= 2 and row_count >= 2 else 15.0
        feature_richness_pts = 20.0 if (resource_cols and (financial_cols or demand_cols)) else 10.0
        dedup_penalty = min(20.0, duplicate_count * 5.0)

        raw_score = completeness_pts + structure_pts + feature_richness_pts - dedup_penalty
        data_quality_score = max(0.0, min(100.0, round(raw_score, 1)))

        if not issues_list:
            quality_status = "Suitable"
            quality_verdict = "Dataset is suitable for the current analysis pipeline."
        else:
            quality_status = "Issues Detected"
            quality_verdict = "Dataset contains issues that may affect optimization."

        return {
            "dataset_name": name,
            "file_size_bytes": file_size_bytes,
            "row_count": row_count,
            "column_count": col_count,
            "total_cells": total_cells,
            "missing_cells_pct": missing_pct,
            "missing_count": missing_count,
            "duplicate_count": duplicate_count,
            "data_quality_score": data_quality_score,
            "quality_status": quality_status,
            "quality_verdict": quality_verdict,
            "issues": issues_list,
            "columns": col_profiles,
            "numeric_columns": numeric_cols,
            "categorical_columns": categorical_cols,
            "constant_columns": constant_cols,
            "semantic_categories": {
                "resource_columns": resource_cols,
                "financial_columns": financial_cols,
                "demand_columns": demand_cols,
                "metadata_columns": meta_cols
            }
        }
