# SOVOPT — Sovereign Mathematical Optimization Platform
### Smart India Hackathon 2026 (Problem Statement ID: 26119)
**Indigenous High-Performance Mathematical Optimization Solver for Complex Industrial Supply Chains and Resource Allocation**

---

## Overview

**SOVOPT** is an indigenous, sovereign mathematical optimization platform designed for mission-critical industrial decisions (such as crude oil distillation and fraction blending for MRPL / SIH PS 26119, automotive production scheduling, 24/7 hospital emergency staffing, and sovereign asset portfolio allocation).

The platform features a multi-tiered architecture combining a native **C++20 mathematical solver core**, a **FastAPI backend** with subprocess IPC, and an interactive **React + Vite visualization workspace**.

---

## Key Capabilities & Optimization Paradigms

### 1. Mixed-Integer Linear Programming (MILP)
- **Branch-and-Bound**: Priority-queue (Best-Bound) search tree exploration with integer and binary variable relaxations.
- **Branch-and-Cut**: Dynamic **Gomory Fractional Cutting Plane Generator** to tighten continuous LP relaxations without excluding integer feasible vertices.

### 2. Quadratic Programming (QP)
- Active-Set Quadratic Solver minimizing objectives of the form:
  $$\min \frac{1}{2} x^T Q x + c^T x \quad \text{subject to } A x \le b, \; A_{eq} x = b_{eq}$$
- Karush-Kuhn-Tucker (KKT) equality system assembly, Gaussian elimination, projected search steps, and Lagrange multiplier updates.

### 3. Advanced Presolve & Dual Simplex
- **Presolve Pipeline**: Empty row elimination, singleton constraint reduction, fixed variable elimination, and iterative bound tightening.
- **Postsolve Recovery**: Exact reconstruction of primal and dual solutions.
- **Dual Simplex**: Dual-feasible basis optimization with Harris dual ratio tests for warm-start sensitivity analysis.

### 4. Hardware Acceleration (SIMD & GPU)
- **AVX2 / SSE4.1 SIMD Kernels**: 256-bit vector registers processing 4 double-precision floating-point operations per clock cycle for row pivot elimination, dot products, and ratio tests.
- **Parallel Workgroup GPU Dispatch**: Multi-threaded 256-thread workgroups for pricing operations across massive constraint matrices.

---

## Architecture

```
SOVOPT/
├── include/sovopt/           # Native C++20 Header Declarations
│   ├── algorithms/          # Branch-and-Bound / Branch-and-Cut Solvers
│   ├── api/                 # SolverAPI Declarations
│   ├── core/                # Variables, Constraints, Models, Objectives
│   ├── io/                  # Fast JSON IPC Serialization
│   ├── lp/                  # Primal Simplex, Dual Simplex, Presolve
│   ├── math/                # SIMD Kernels (AVX2) & GPU Accelerator
│   ├── optimization/        # Optimization Engine Dispatcher & Options
│   └── qp/                  # Quadratic Programming Models & Active-Set Solver
├── src/                     # C++20 Implementation Source Files
├── tests/                   # Native C++ Test Suite (sovopt_tests.exe)
├── backend/                 # FastAPI REST API & Subprocess IPC Pipeline
│   ├── main.py              # Application Entry & Endpoints
│   ├── database.py          # SQLite Audit Trail & Run History
│   └── intelligence/        # Data Profiler & Auto-Modeling Classifier
├── frontend/                # Modern React + Vite Operator Workspace
│   ├── src/components/      # Multi-Paradigm Workspace, Model Builder, Analytics
│   └── src/services/        # API Client & Offline Fallback Solver
├── CMakeLists.txt           # Modern CMake Configuration (MSVC / GCC / Clang)
└── README.md
```

---

## Getting Started

### Prerequisites
- **C++ Compiler**: MSVC (Visual Studio 2022 v143), GCC 12+, or Clang 15+ supporting C++20
- **CMake**: 3.20 or newer
- **Python**: 3.10+ (FastAPI, Uvicorn, Pydantic)
- **Node.js**: 18+ (React, Vite)

### 1. Build the C++20 Core Engine
```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Run the automated test suite (54 tests):
```bash
build\Release\sovopt_tests.exe
```

### 2. Start the FastAPI Backend
```bash
cd backend
pip install -r requirements.txt
python main.py
```
Backend API will be running at `http://127.0.0.1:8000` (docs at `http://127.0.0.1:8000/docs`).

### 3. Start the Frontend Workspace
```bash
cd frontend
npm install
npm run dev
```
Open `http://localhost:5173` in your browser.

---

## Verification & Benchmarks
- **Netlib AFIRO**: Optimal solution certified in 6 pivots ($< 0.05$ ms).
- **Netlib ADLITTLE**: Optimal solution certified in 44 pivots ($< 0.20$ ms).
- **MRPL Refinery Crude Distillation (PS 26119)**: Solved in 3 pivots ($< 0.02$ ms).
- **Hospital Shift Scheduling (MILP)**: Exact integer branch-and-cut optimum verified.
- **Markowitz Mean-Variance Portfolio (QP)**: Active-set KKT system verified.

---

## License
Developed for Smart India Hackathon 2026 — Sovereign Mathematical Optimization Platform.
