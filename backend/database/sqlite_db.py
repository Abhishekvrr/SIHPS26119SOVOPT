"""
SOVOPT - Indigenous Mathematical Optimization Solver
SQLite Local Prototype Persistence Layer
Stores dataset metadata, profiling summaries, and optimization runs.
"""

from contextlib import contextmanager
import json
import os
import sqlite3
import time
from typing import Any, Dict, Generator, List, Optional

DB_DIR = os.path.dirname(os.path.abspath(__file__))
DB_FILE = os.path.join(DB_DIR, "sovopt.db")


@contextmanager
def get_db() -> Generator[sqlite3.Connection, None, None]:
    """Context manager that opens and cleanly closes a SQLite connection."""
    conn = sqlite3.connect(DB_FILE)
    conn.row_factory = sqlite3.Row
    try:
        yield conn
    finally:
        conn.close()


def init_db() -> None:
    """Initializes the database schema if not present."""
    os.makedirs(DB_DIR, exist_ok=True)
    with get_db() as conn:
        cursor = conn.cursor()

        # 1. Datasets table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS datasets (
                id TEXT PRIMARY KEY,
                filename TEXT NOT NULL,
                dataset_name TEXT NOT NULL,
                filepath TEXT,
                uploaded_at REAL NOT NULL,
                size_bytes INTEGER DEFAULT 0,
                row_count INTEGER DEFAULT 0,
                column_count INTEGER DEFAULT 0,
                is_demo INTEGER DEFAULT 0
            )
        """)

        # 2. Dataset profiles table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS dataset_profiles (
                id TEXT PRIMARY KEY,
                dataset_id TEXT NOT NULL,
                missing_count INTEGER DEFAULT 0,
                duplicate_count INTEGER DEFAULT 0,
                quality_score REAL DEFAULT 100.0,
                quality_status TEXT DEFAULT 'Suitable',
                numeric_columns TEXT,
                categorical_columns TEXT,
                profile_json TEXT,
                created_at REAL NOT NULL,
                FOREIGN KEY (dataset_id) REFERENCES datasets(id) ON DELETE CASCADE
            )
        """)

        # 3. Optimization runs table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS optimization_runs (
                id TEXT PRIMARY KEY,
                dataset_id TEXT,
                model_name TEXT NOT NULL,
                created_at REAL NOT NULL,
                solver_status TEXT NOT NULL,
                objective_value REAL DEFAULT 0.0,
                iterations INTEGER DEFAULT 0,
                solve_time_ms REAL DEFAULT 0.0,
                max_primal_violation REAL DEFAULT 0.0,
                result_json TEXT,
                FOREIGN KEY (dataset_id) REFERENCES datasets(id) ON DELETE SET NULL
            )
        """)
        conn.commit()


class DatabaseManager:
    """Interface for querying and saving datasets, profiles, and optimization history."""

    @staticmethod
    def save_dataset(
        dataset_id: str,
        filename: str,
        dataset_name: str,
        filepath: Optional[str],
        size_bytes: int,
        row_count: int,
        column_count: int,
        is_demo: bool = False
    ) -> None:
        init_db()
        with get_db() as conn:
            cursor = conn.cursor()
            cursor.execute("""
                INSERT OR REPLACE INTO datasets (
                    id, filename, dataset_name, filepath, uploaded_at, size_bytes, row_count, column_count, is_demo
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
            """, (
                dataset_id,
                filename,
                dataset_name,
                filepath,
                time.time(),
                size_bytes,
                row_count,
                column_count,
                1 if is_demo else 0
            ))
            conn.commit()

    @staticmethod
    def save_profile(
        dataset_id: str,
        missing_count: int,
        duplicate_count: int,
        quality_score: float,
        quality_status: str,
        numeric_columns: List[str],
        categorical_columns: List[str],
        profile_dict: Dict[str, Any]
    ) -> str:
        init_db()
        profile_id = f"prof_{dataset_id}_{int(time.time())}"
        with get_db() as conn:
            cursor = conn.cursor()
            cursor.execute("""
                INSERT OR REPLACE INTO dataset_profiles (
                    id, dataset_id, missing_count, duplicate_count, quality_score, quality_status,
                    numeric_columns, categorical_columns, profile_json, created_at
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """, (
                profile_id,
                dataset_id,
                missing_count,
                duplicate_count,
                quality_score,
                quality_status,
                json.dumps(numeric_columns),
                json.dumps(categorical_columns),
                json.dumps(profile_dict),
                time.time()
            ))
            conn.commit()
        return profile_id

    @staticmethod
    def save_optimization_run(
        dataset_id: Optional[str],
        model_name: str,
        solver_status: str,
        objective_value: float,
        iterations: int,
        solve_time_ms: float,
        max_primal_violation: float,
        result_dict: Dict[str, Any]
    ) -> str:
        init_db()
        run_id = f"run_{int(time.time() * 1000)}"
        with get_db() as conn:
            cursor = conn.cursor()
            cursor.execute("""
                INSERT INTO optimization_runs (
                    id, dataset_id, model_name, created_at, solver_status,
                    objective_value, iterations, solve_time_ms, max_primal_violation, result_json
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """, (
                run_id,
                dataset_id,
                model_name,
                time.time(),
                solver_status,
                objective_value,
                iterations,
                solve_time_ms,
                max_primal_violation,
                json.dumps(result_dict)
            ))
            conn.commit()
        return run_id

    @staticmethod
    def list_datasets() -> List[Dict[str, Any]]:
        init_db()
        with get_db() as conn:
            cursor = conn.cursor()
            cursor.execute("SELECT * FROM datasets ORDER BY uploaded_at DESC")
            rows = cursor.fetchall()
            return [dict(row) for row in rows]

    @staticmethod
    def get_dataset(dataset_id: str) -> Optional[Dict[str, Any]]:
        init_db()
        with get_db() as conn:
            cursor = conn.cursor()
            cursor.execute("SELECT * FROM datasets WHERE id = ?", (dataset_id,))
            row = cursor.fetchone()
            return dict(row) if row else None

    @staticmethod
    def list_optimization_runs(limit: int = 50) -> List[Dict[str, Any]]:
        init_db()
        with get_db() as conn:
            cursor = conn.cursor()
            cursor.execute("""
                SELECT r.*, d.dataset_name, d.filename
                FROM optimization_runs r
                LEFT JOIN datasets d ON r.dataset_id = d.id
                ORDER BY r.created_at DESC
                LIMIT ?
            """, (limit,))
            rows = cursor.fetchall()
            runs = []
            for r in rows:
                run_data = dict(r)
                if run_data.get("result_json"):
                    try:
                        run_data["result"] = json.loads(run_data["result_json"])
                    except Exception:
                        run_data["result"] = None
                runs.append(run_data)
            return runs


# Ensure initialized on import
init_db()

