import React from 'react';
import { ShieldCheck, Cpu, Database, Brain, Layers, CheckCircle2, Circle, Clock, Check } from 'lucide-react';

export default function ArchitectureView() {
  const architectureLayers = [
    {
      id: 'data_layer',
      name: '1. DATA LAYER',
      subtitle: 'Structured CSV / Tabular Industrial Data',
      whatEnters: 'Structured CSV file, tabular industrial planning records, column headers.',
      whatHappens: 'Streaming RFC-4180 parsing, numeric type coercion, header normalization, and SQLite metadata indexing.',
      whatComesOut: 'In-memory tabular record matrix, dimension telemetry (rows × cols, size in bytes).'
    },
    {
      id: 'intelligence_layer',
      name: '2. DATA INTELLIGENCE',
      subtitle: 'Schema Inspection & Parameter Extraction',
      whatEnters: 'Tabular record matrix from Data Layer.',
      whatHappens: 'Missing-value calculation, duplicate detection, statistical profiling (min/max/mean/std-dev), semantic keyword role tagging.',
      whatComesOut: 'Classified optimization parameters (product IDs, capacities, unit margins, resource consumption rates).'
    },
    {
      id: 'model_layer',
      name: '3. MODEL LAYER',
      subtitle: 'Mathematical Linear Program Formulation',
      whatEnters: 'Classified dataset parameters and operational bounds.',
      whatHappens: 'Constructs candidate decision variables (x_i), objective vector (c), constraint matrix (A), and RHS bounds (b).',
      whatComesOut: 'Standard LP model JSON representation (Max/Min c^T x s.t. Ax <= b, l <= x <= u).'
    },
    {
      id: 'sovopt_core',
      name: '4. SOVOPT CORE',
      subtitle: 'Native C++20 Simplex Engine',
      whatEnters: 'Standard LP model specification via direct subprocess JSON pipe.',
      whatHappens: 'Two-Phase Primal Simplex algorithm, sparse matrix basis factorization, Bland pivot selection, reduced cost pricing.',
      whatComesOut: 'Optimal primal decision vector x*, optimal objective value Z*, iteration count, solve latency (ms).'
    },
    {
      id: 'validation_layer',
      name: '5. VALIDATION LAYER',
      subtitle: 'Feasibility & Constraint Residual Verification',
      whatEnters: 'Optimal solution vector x* and original constraint specifications.',
      whatHappens: 'Computes primal residual violation ||Ax - b||, evaluates constraint slacks, and tags binding bottlenecks.',
      whatComesOut: 'Verified feasibility status (residual <= 1e-7), 100% binding bottleneck diagnostics, decision confidence metrics.'
    },
    {
      id: 'app_layer',
      name: '6. APPLICATION LAYER',
      subtitle: 'FastAPI & React Decision Dashboard',
      whatEnters: 'Validated optimal solution and solver execution telemetry.',
      whatHappens: 'Interactive visualization, scenario benchmarking, SQLite run logging, operational production recommendations.',
      whatComesOut: 'Production schedules, resource utilization charts, operator decision view.'
    }
  ];

  return (
    <div className="view-container">
      {/* Header */}
      <div className="panel-card flex-between mb-4">
        <div>
          <div className="badge-row">
            <span className="category-tag">SIH 2026 Problem Statement PS26119</span>
            <span className="problem-id-tag">Industrial Optimization</span>
          </div>
          <h2 className="problem-main-title">SOVOPT — From Industrial Data to Mathematical Decisions</h2>
          <p className="problem-desc">
            SOVOPT is an indigenous mathematical optimization platform that converts structured industrial planning data
            into a Linear Programming model and solves it using a native C++20 Simplex engine.
          </p>
        </div>
      </div>

      {/* Top-to-Bottom Flow Overview */}
      <div className="panel-card mb-4">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <Layers size={16} className="text-cyan" />
            <h3>End-to-End Mathematical Decision Pipeline</h3>
          </div>
          <span className="panel-badge">6-Stage Architecture</span>
        </div>

        <div className="pipeline-flow-pills flex-row-wrap items-center justify-center p-3 bg-slate-900 rounded-lg border border-slate-800">
          <span className="flow-pill font-mono text-xs font-bold text-cyan">INDUSTRIAL DATA</span>
          <span className="text-muted text-xs">→</span>
          <span className="flow-pill font-mono text-xs font-bold text-cyan">DATA PROFILING</span>
          <span className="text-muted text-xs">→</span>
          <span className="flow-pill font-mono text-xs font-bold text-cyan">MODEL PARAMETERS</span>
          <span className="text-muted text-xs">→</span>
          <span className="flow-pill font-mono text-xs font-bold text-cyan">LP MODEL</span>
          <span className="text-muted text-xs">→</span>
          <span className="flow-pill font-mono text-xs font-bold text-emerald-400">SOVOPT C++20 SOLVER</span>
          <span className="text-muted text-xs">→</span>
          <span className="flow-pill font-mono text-xs font-bold text-cyan">VALIDATED SOLUTION</span>
          <span className="text-muted text-xs">→</span>
          <span className="flow-pill font-mono text-xs font-bold text-cyan">DECISION VIEW</span>
        </div>
      </div>

      {/* Layer-by-Layer Architectural Specifications */}
      <div className="panel-card mb-4">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <Cpu size={16} className="text-cyan" />
            <h3>Current Technology Architecture</h3>
          </div>
          <span className="panel-badge">Component Boundaries</span>
        </div>

        <div className="sov-grid-2">
          {architectureLayers.map((layer) => (
            <div key={layer.id} className="arch-layer-card">
              <div className="arch-layer-header">
                <span className="arch-layer-title">{layer.name}</span>
                <span className="arch-layer-sub">{layer.subtitle}</span>
              </div>
              <div className="arch-io-row">
                <div className="arch-io-box">
                  <span className="arch-io-tag cyan">What Enters:</span>
                  <p className="text-secondary">{layer.whatEnters}</p>
                </div>
                <div className="arch-io-box">
                  <span className="arch-io-tag amber">What Happens:</span>
                  <p className="text-secondary">{layer.whatHappens}</p>
                </div>
                <div className="arch-io-box">
                  <span className="arch-io-tag emerald">What Comes Out:</span>
                  <p className="text-secondary">{layer.whatComesOut}</p>
                </div>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Verified Solver Capabilities with Rigorous Status Labels */}
      <div className="panel-card">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <ShieldCheck size={16} className="text-cyan" />
            <h3>Verified Solver Capabilities</h3>
          </div>
          <span className="panel-badge">Technical Ground Truth</span>
        </div>

        <div className="sov-grid-3">
          {/* IMPLEMENTED */}
          <div className="semantic-card-box border-emerald-800">
            <div className="flex-between pb-2 border-b border-emerald-800">
              <span className="font-bold text-sm text-emerald-400">IMPLEMENTED</span>
              <span className="status-pill optimal">Active &amp; Tested</span>
            </div>
            <ul className="semantic-card-list">
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>Linear Programming (LP)</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>LP model construction</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>Objective handling (Max/Min)</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>Constraint handling (&lt;=, &gt;=, ==)</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>Two-Phase Simplex iterations</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>Feasibility / solution status</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>Native C++20 execution (MSVC)</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>CMake build automation</span></li>
              <li className="semantic-card-item"><Check size={14} className="text-emerald-400" /> <span>Solver result integration</span></li>
            </ul>
          </div>

          {/* FOUNDATION */}
          <div className="semantic-card-box border-blue-800">
            <div className="flex-between pb-2 border-b border-blue-800">
              <span className="font-bold text-sm text-cyan">FOUNDATION</span>
              <span className="status-pill demo">Core Data Layer</span>
            </div>
            <ul className="semantic-card-list">
              <li className="semantic-card-item"><span className="w-2 h-2 rounded-full bg-cyan"></span> <span>Sparse matrix structures</span></li>
              <li className="semantic-card-item"><span className="w-2 h-2 rounded-full bg-cyan"></span> <span>Numerical computation layer</span></li>
              <li className="semantic-card-item"><span className="w-2 h-2 rounded-full bg-cyan"></span> <span>Modular solver architecture</span></li>
              <li className="semantic-card-item"><span className="w-2 h-2 rounded-full bg-cyan"></span> <span>Direct stdio JSON protocol bridge</span></li>
              <li className="semantic-card-item"><span className="w-2 h-2 rounded-full bg-cyan"></span> <span>SQLite persistence manager</span></li>
            </ul>
          </div>

          {/* ROADMAP */}
          <div className="semantic-card-box border-slate-800">
            <div className="flex-between pb-2 border-b border-slate-800">
              <span className="font-bold text-sm text-slate-400">ROADMAP</span>
              <span className="status-pill custom">Future Work</span>
            </div>
            <ul className="semantic-card-list text-muted">
              <li className="semantic-card-item"><Circle size={10} className="text-slate-500" /> <span>Mixed-Integer LP (MILP)</span></li>
              <li className="semantic-card-item"><Circle size={10} className="text-slate-500" /> <span>Quadratic Programming (QP)</span></li>
              <li className="semantic-card-item"><Circle size={10} className="text-slate-500" /> <span>Advanced presolve &amp; dual simplex</span></li>
              <li className="semantic-card-item"><Circle size={10} className="text-slate-500" /> <span>Branch-and-bound / cut</span></li>
              <li className="semantic-card-item"><Circle size={10} className="text-slate-500" /> <span>GPU acceleration &amp; SIMD</span></li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  );
}
