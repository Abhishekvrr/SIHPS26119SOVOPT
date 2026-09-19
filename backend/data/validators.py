"""
SOVOPT Data Ingestion Layer - Validators
Validates data integrity, null/missing ratios, schema consistency, and numerical range sanity.
"""

from typing import Any, Dict, List, Tuple


class DataValidator:
    """Performs pre-flight data hygiene and schema validation on ingested company records."""

    @staticmethod
    def validate_records(records: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Inspects records and returns statistical column hygiene, missing % and validity."""
        if not records:
            return {
                "is_valid": False,
                "error": "Dataset is empty.",
                "total_rows": 0,
                "total_columns": 0,
                "missing_values_count": 0,
                "missing_values_pct": 100.0,
                "cleanliness_score": 0.0,
                "columns": []
            }

        total_rows = len(records)
        all_cols = list(records[0].keys())
        total_cells = total_rows * len(all_cols)
        total_missing = 0
        column_metrics = []

        for col in all_cols:
            non_null_vals = []
            col_missing = 0
            numeric_count = 0
            cat_count = 0

            for r in records:
                val = r.get(col)
                if val is None or val == "":
                    col_missing += 1
                else:
                    non_null_vals.append(val)
                    if isinstance(val, (int, float)) and not isinstance(val, bool):
                        numeric_count += 1
                    else:
                        cat_count += 1

            total_missing += col_missing
            col_missing_pct = round((col_missing / total_rows) * 100.0, 2)
            is_numeric = numeric_count > cat_count and numeric_count > 0

            col_info = {
                "name": col,
                "data_type": "numeric" if is_numeric else "categorical",
                "non_null_count": len(non_null_vals),
                "missing_count": col_missing,
                "missing_pct": col_missing_pct,
                "is_valid": col_missing_pct < 50.0
            }

            if is_numeric and non_null_vals:
                numeric_vals = [v for v in non_null_vals if isinstance(v, (int, float)) and not isinstance(v, bool)]
                if numeric_vals:
                    col_info["min"] = round(float(min(numeric_vals)), 4)
                    col_info["max"] = round(float(max(numeric_vals)), 4)
                    col_info["mean"] = round(float(sum(numeric_vals) / len(numeric_vals)), 4)

            column_metrics.append(col_info)

        missing_pct = round((total_missing / total_cells) * 100.0, 2) if total_cells > 0 else 0.0
        # Cleanliness score (0-100)
        cleanliness = max(0.0, min(100.0, round(100.0 - (missing_pct * 1.5), 1)))

        return {
            "is_valid": total_rows > 0 and len(all_cols) > 0,
            "total_rows": total_rows,
            "total_columns": len(all_cols),
            "missing_values_count": total_missing,
            "missing_values_pct": missing_pct,
            "cleanliness_score": cleanliness,
            "columns": column_metrics
        }

