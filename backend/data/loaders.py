"""
SOVOPT Data Ingestion Layer - Loaders
Provides robust ingestion for CSV, JSON, and tabular data records.
"""

import csv
import io
import json
from typing import Any, Dict, List, Union


class DataLoader:
    """Ingests tabular records from CSV text, file buffers, or raw JSON structures."""

    @staticmethod
    def load_from_csv_text(csv_text: str) -> List[Dict[str, Any]]:
        """Parses CSV text into a list of row dictionaries with auto-type casting."""
        if not csv_text or not csv_text.strip():
            return []

        f = io.StringIO(csv_text.strip())
        reader = csv.DictReader(f)
        records: List[Dict[str, Any]] = []

        for row in reader:
            parsed_row: Dict[str, Any] = {}
            for col_name, raw_val in row.items():
                if col_name is None:
                    continue
                clean_col = col_name.strip()
                if raw_val is None:
                    parsed_row[clean_col] = None
                    continue

                clean_val = raw_val.strip()
                # Try float / int conversion
                try:
                    if "." in clean_val or "e" in clean_val.lower():
                        parsed_row[clean_col] = float(clean_val)
                    else:
                        parsed_row[clean_col] = int(clean_val)
                except ValueError:
                    # Boolean or string
                    if clean_val.lower() == "true":
                        parsed_row[clean_col] = True
                    elif clean_val.lower() == "false":
                        parsed_row[clean_col] = False
                    elif clean_val == "" or clean_val.lower() in ("null", "none", "nan", "n/a"):
                        parsed_row[clean_col] = None
                    else:
                        parsed_row[clean_col] = clean_val

            if parsed_row:
                records.append(parsed_row)

        return records

    @staticmethod
    def load_from_csv_path(filepath: str) -> List[Dict[str, Any]]:
        """Loads and parses a CSV file from the filesystem."""
        with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
            return DataLoader.load_from_csv_text(f.read())

    @staticmethod
    def load_from_json(json_input: Union[str, List[Dict[str, Any]], Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Parses JSON text or structure into a list of standardized record dictionaries."""
        if isinstance(json_input, str):
            try:
                data = json.loads(json_input)
            except Exception as ex:
                raise ValueError(f"Invalid JSON string format: {ex}")
        else:
            data = json_input

        if isinstance(data, list):
            return [dict(item) for item in data if isinstance(item, dict)]
        elif isinstance(data, dict):
            # Check if records/rows key exists
            for k in ("records", "data", "rows", "items"):
                if k in data and isinstance(data[k], list):
                    return [dict(item) for item in data[k] if isinstance(item, dict)]
            return [data]
        return []

