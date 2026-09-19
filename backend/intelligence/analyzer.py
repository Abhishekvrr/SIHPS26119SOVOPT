import csv
import io
from typing import List, Dict, Any, Tuple, Optional

class DataAnalyzer:
    """
    Analyzes raw tabular or JSON company datasets to profile columns,
    compute data quality metrics, and detect domain-specific semantic roles.
    """

    RESOURCE_KEYWORDS = [
        "capacity", "hour", "machine", "limit", "max", "inventory", "supply",
        "available", "space", "material", "power", "energy", "kg", "ton", "storage",
        "crude", "feedstock", "labor", "time", "facility", "quota"
    ]

    FINANCIAL_KEYWORDS = [
        "cost", "price", "revenue", "profit", "margin", "wage", "expense",
        "rate", "return", "yield", "dollar", "inr", "rupee", "budget", "val"
    ]

    DEMAND_KEYWORDS = [
        "demand", "requirement", "target", "output", "order", "min", "quota",
        "volume", "quota", "market", "need", "minimum"
    ]

    @classmethod
    def parse_csv_content(cls, content: str) -> List[Dict[str, Any]]:
        """Parses CSV text into a list of row dictionaries."""
        f = io.StringIO(content.strip())
        reader = csv.DictReader(f)
        rows = []
        for r in reader:
            parsed_row = {}
            for k, v in r.items():
                if k is None:
                    continue
                clean_k = k.strip()
                clean_v = v.strip() if isinstance(v, str) else v
                # Try float parsing
                try:
                    parsed_row[clean_k] = float(clean_v)
                except (ValueError, TypeError):
                    parsed_row[clean_k] = clean_v
            rows.append(parsed_row)
        return rows

    @classmethod
    def analyze_dataset(cls, dataset_name: str, records: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Profiles the dataset and calculates metrics:
        - Row & column counts
        - Numerical vs categorical columns
        - Missing value ratio
        - Resource, financial, and demand column classification
        - Overall Data Quality Score (0 - 100%)
        """
        if not records:
            return {
                "dataset_name": dataset_name,
                "rows_count": 0,
                "columns_count": 0,
                "numerical_columns": [],
                "categorical_columns": [],
                "potential_resource_columns": [],
                "potential_financial_columns": [],
                "potential_demand_columns": [],
                "missing_values_pct": 0.0,
                "data_quality_pct": 0.0,
                "summary": "Dataset is empty."
            }

        columns = list(records[0].keys())
        num_rows = len(records)
        num_cols = len(columns)

        numerical_cols = []
        categorical_cols = []
        null_count = 0
        total_cells = num_rows * num_cols

        column_stats = {}

        for col in columns:
            col_lower = col.lower()
            num_numeric = 0
            num_nulls = 0
            values = []

            for r in records:
                val = r.get(col)
                if val is None or val == "" or str(val).lower() in ["nan", "null", "none"]:
                    num_nulls += 1
                elif isinstance(val, (int, float)):
                    num_numeric += 1
                    values.append(float(val))
                else:
                    try:
                        f_val = float(str(val).replace(",", "").replace("$", "").replace("₹", ""))
                        num_numeric += 1
                        values.append(f_val)
                    except ValueError:
                        pass

            null_count += num_nulls
            is_numeric = (num_numeric / num_rows) >= 0.75 if num_rows > 0 else False

            if is_numeric:
                numerical_cols.append(col)
                col_min = min(values) if values else 0.0
                col_max = max(values) if values else 0.0
                col_avg = sum(values) / len(values) if values else 0.0
                column_stats[col] = {
                    "type": "numeric",
                    "min": round(col_min, 2),
                    "max": round(col_max, 2),
                    "avg": round(col_avg, 2),
                    "sample_count": len(values)
                }
            else:
                categorical_cols.append(col)
                column_stats[col] = {
                    "type": "categorical",
                    "sample_count": num_rows - num_nulls
                }

        # Semantic column detection
        resource_cols = [
            c for c in numerical_cols
            if any(k in c.lower() for k in cls.RESOURCE_KEYWORDS)
        ]
        financial_cols = [
            c for c in numerical_cols
            if any(k in c.lower() for k in cls.FINANCIAL_KEYWORDS)
        ]
        demand_cols = [
            c for c in numerical_cols
            if any(k in c.lower() for k in cls.DEMAND_KEYWORDS)
        ]

        missing_pct = round((null_count / total_cells * 100.0), 1) if total_cells > 0 else 0.0

        # Data Quality Score calculation
        # Factors: Completeness (40%), Type Consistency (30%), Row Sufficiency (15%), Numeric Diversity (15%)
        completeness_pts = max(0.0, 40.0 - (missing_pct * 1.5))
        type_pts = 30.0 if len(numerical_cols) >= 2 else (15.0 if len(numerical_cols) == 1 else 5.0)
        row_pts = min(15.0, (num_rows / 10.0) * 15.0) if num_rows >= 5 else 5.0
        diversity_pts = 15.0 if len(numerical_cols) >= 3 else 10.0

        data_quality_pct = round(min(100.0, max(10.0, completeness_pts + type_pts + row_pts + diversity_pts)), 1)

        return {
            "dataset_name": dataset_name,
            "rows_count": num_rows,
            "columns_count": num_cols,
            "numerical_columns": numerical_cols,
            "categorical_columns": categorical_cols,
            "column_stats": column_stats,
            "potential_resource_columns": resource_cols,
            "potential_financial_columns": financial_cols,
            "potential_demand_columns": demand_cols,
            "missing_values_pct": missing_pct,
            "data_quality_pct": data_quality_pct,
            "summary": f"Analyzed {num_rows} operational records across {num_cols} columns with {data_quality_pct}% data quality index."
        }

