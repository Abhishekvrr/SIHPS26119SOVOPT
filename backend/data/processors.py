"""
SOVOPT Data Ingestion Layer - Processors
Normalizes tabular fields, handles imputation, and formats numerical matrices for model synthesis.
"""

from typing import Any, Dict, List, Optional


class DataProcessor:
    """Cleans, normalizes, and prepares raw enterprise tabular records for optimization modeling."""

    @staticmethod
    def clean_and_normalize(records: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Cleans headers and imputes missing numeric values with column averages."""
        if not records:
            return []

        # Find column averages for numerical fields
        all_cols = list(records[0].keys())
        col_means: Dict[str, float] = {}

        for col in all_cols:
            vals = [r[col] for r in records if isinstance(r.get(col), (int, float)) and not isinstance(r.get(col), bool)]
            if vals:
                col_means[col] = sum(vals) / len(vals)

        cleaned_records: List[Dict[str, Any]] = []
        for r in records:
            cleaned_row: Dict[str, Any] = {}
            for col in all_cols:
                val = r.get(col)
                if val is None or val == "":
                    # Impute
                    if col in col_means:
                        cleaned_row[col] = round(col_means[col], 4)
                    else:
                        cleaned_row[col] = "N/A"
                else:
                    cleaned_row[col] = val
            cleaned_records.append(cleaned_row)

        return cleaned_records

