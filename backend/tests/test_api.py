"""
SOVOPT Backend API Integration & Validation Test Suite
Tests FastAPI endpoints, request validation, error handling, SQLite persistence, and C++ solver integration.
"""

import sys
import os
import unittest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from fastapi.testclient import TestClient
from backend.main import app

client = TestClient(app)


class TestSOVOPTBackendAPI(unittest.TestCase):
    """Automated integration tests for SOVOPT backend API."""

    def test_01_root_endpoint(self):
        """Test GET / returns platform metadata and SIH 2026 context."""
        response = client.get("/")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertEqual(data["platform"], "SOVOPT")
        self.assertIn("26119", data["sih_context"])
        self.assertEqual(data["status"], "operational")

    def test_02_health_endpoint(self):
        """Test GET /api/v1/health returns solver status and executable path."""
        response = client.get("/api/v1/health")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertIn("status", data)
        self.assertIn("solver_ready", data)
        self.assertIn("solver_executable", data)

    def test_03_demo_datasets(self):
        """Test GET /api/v1/demo-datasets returns pre-loaded industry scenarios."""
        response = client.get("/api/v1/demo-datasets")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertIn("datasets", data)
        self.assertGreaterEqual(len(data["datasets"]), 3)
        for ds in data["datasets"]:
            self.assertIn("id", ds)
            self.assertIn("name", ds)
            self.assertIn("records", ds)
            self.assertIn("analysis", ds)
            self.assertIn("classification", ds)

    def test_04_data_analysis_endpoint(self):
        """Test POST /api/v1/analyze-data profiles CSV text into structured analysis."""
        csv_sample = (
            "product,processing_capacity_tpd,selling_price_inr_per_ton,demand_tpd\n"
            "Petrol,4800,62000,4000\n"
            "Diesel,5200,58000,4500\n"
        )
        payload = {
            "dataset_name": "Test Refinery Sample",
            "csv_text": csv_sample
        }
        response = client.post("/api/v1/analyze-data", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertEqual(data["status"], "success")
        self.assertEqual(data["analysis"]["row_count"], 2)
        self.assertEqual(data["analysis"]["column_count"], 4)
        self.assertEqual(data["analysis"]["data_quality_score"], 100)

    def test_05_optimize_phase3_production_model(self):
        """Test POST /api/v1/optimize solves standard Max 40*x1 + 30*x2 model."""
        payload = {
            "model_name": "Production Maximize Model",
            "objective_sense": "maximize",
            "variables": [
                {"name": "x1", "type": "continuous", "lower_bound": 0.0, "upper_bound": 1e20, "objective": 40.0},
                {"name": "x2", "type": "continuous", "lower_bound": 0.0, "upper_bound": 1e20, "objective": 30.0}
            ],
            "constraints": [
                {"name": "C1", "sense": "<=", "rhs": 100.0, "coefficients": [2.0, 1.0]},
                {"name": "C2", "sense": "<=", "rhs": 80.0, "coefficients": [1.0, 2.0]}
            ]
        }
        response = client.post("/api/v1/optimize", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertEqual(data["status"], "OPTIMAL")
        self.assertAlmostEqual(data["objective_value"], 2200.0, places=2)
        self.assertEqual(len(data["solution"]), 2)
        self.assertAlmostEqual(data["solution"][0]["value"], 40.0, places=2)
        self.assertAlmostEqual(data["solution"][1]["value"], 20.0, places=2)
        self.assertLessEqual(data["max_primal_violation"], 1e-6)
        self.assertIn("decision_score", data)
        self.assertGreaterEqual(data["decision_score"]["total_score"], 90)

    def test_06_optimize_minimize_model(self):
        """Test POST /api/v1/optimize solves Min 2*x1 + 3*x2 model with >= constraints."""
        payload = {
            "model_name": "Diet Minimize Model",
            "objective_sense": "minimize",
            "variables": [
                {"name": "x1", "type": "continuous", "lower_bound": 0.0, "upper_bound": 1e20, "objective": 2.0},
                {"name": "x2", "type": "continuous", "lower_bound": 0.0, "upper_bound": 1e20, "objective": 3.0}
            ],
            "constraints": [
                {"name": "C1", "sense": ">=", "rhs": 6.0, "coefficients": [1.0, 1.0]},
                {"name": "C2", "sense": ">=", "rhs": 8.0, "coefficients": [2.0, 1.0]}
            ]
        }
        response = client.post("/api/v1/optimize", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertEqual(data["status"], "OPTIMAL")
        self.assertAlmostEqual(data["objective_value"], 12.0, places=2)
        self.assertAlmostEqual(data["solution"][0]["value"], 6.0, places=2)
        self.assertAlmostEqual(data["solution"][1]["value"], 0.0, places=2)

    def test_07_invalid_json_handling(self):
        """Test POST /api/v1/optimize rejects missing variables or empty payloads gracefully."""
        payload = {
            "model_name": "Invalid Model",
            "objective_sense": "maximize"
        }
        response = client.post("/api/v1/optimize", json=payload)
        self.assertEqual(response.status_code, 422)

    def test_08_empty_analysis_request(self):
        """Test POST /api/v1/analyze-data returns 400 when no records or CSV text provided."""
        payload = {
            "dataset_name": "Empty Dataset"
        }
        response = client.post("/api/v1/analyze-data", json=payload)
        self.assertEqual(response.status_code, 400)

    def test_09_database_dataset_persistence(self):
        """Test GET /api/v1/datasets returns persisted dataset list from SQLite."""
        csv_sample = "product,demand,margin\nPetrol,1000,5000\nDiesel,2000,4500\n"
        client.post("/api/v1/analyze-data", json={"dataset_name": "SQLite Test Dataset", "csv_text": csv_sample})

        response = client.get("/api/v1/datasets")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertIn("datasets", data)
        self.assertGreaterEqual(len(data["datasets"]), 1)
        found = any(d["dataset_name"] == "SQLite Test Dataset" for d in data["datasets"])
        self.assertTrue(found)

    def test_10_database_run_history(self):
        """Test GET /api/v1/optimization-runs returns history from SQLite."""
        response = client.get("/api/v1/optimization-runs")
        self.assertEqual(response.status_code, 200)
        data = response.json()
        self.assertIn("runs", data)
        self.assertGreaterEqual(len(data["runs"]), 1)

    def test_11_dynamic_issue_reporting_on_dirty_data(self):
        """Test dynamic issue detection for missing values and duplicates."""
        dirty_csv = (
            "product,energy_mwh_per_ton,margin_inr_per_ton\n"
            "Petrol,,62000\n"
            "Diesel,1.2,58000\n"
            "Diesel,1.2,58000\n"
        )
        response = client.post("/api/v1/analyze-data", json={"dataset_name": "Dirty Data", "csv_text": dirty_csv})
        self.assertEqual(response.status_code, 200)
        data = response.json()
        analysis = data["analysis"]
        self.assertEqual(analysis["quality_status"], "Issues Detected")
        self.assertGreater(len(analysis["issues"]), 0)
        self.assertEqual(analysis["duplicate_count"], 1)


if __name__ == "__main__":
    unittest.main()
