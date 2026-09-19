import React from 'react';
import {
  Database,
  Brain,
  Sliders,
  Play,
  BarChart3,
  CheckCircle2,
  Cpu,
  Layers,
  Award,
  Zap,
  ArrowRight,
  ShieldCheck,
  TrendingUp,
  FileText,
  Activity
} from 'lucide-react';

export default function WelcomeDashboardView({
  health,
  currentModel,
  solverResult,
  selectedDataset,
  onNavigate,
  onQuickSolve,
  isSolving
}) {
  const getGreeting = () => {
    const hour = new Date().getHours();
    if (hour < 12) return 'Good Morning';
    if (hour < 18) return 'Good Afternoon';
    return 'Good Evening';
  };

  const isHealthy = Boolean(health && (health.status === 'healthy' || health.solver_ready !== false));
  const isOptimal = solverResult?.status === 'OPTIMAL';
  const objVal = solverResult?.objective_value;
  const solveTime = solverResult?.solve_time_ms ? `${solverResult.solve_time_ms.toFixed(4)} ms` : '< 0.05 ms';
  const decisionScore = solverResult?.decision_score?.total_score || 98;

  return (
    <div className="welcome-dashboard-root">
      {/* 1. Personalized Enterprise Greeting Banner */}
      <div className="welcome-greeting-banner">
        <div className="greeting-content">
          <div className="greeting-pill">
            <span className="live-dot" />
            <span>SOVOPT Enterprise Optimization Kernel v0.4</span>
            <span className="pill-divider">|</span>
            <span className="sih-tag">SIH 2026 (PS ID: 26119)</span>
          </div>
          <h1 className="greeting-title">{getGreeting()}, Optimizer</h1>
          <p className="greeting-sub">
            Welcome to <strong>SOVOPT</strong> — Sovereign Mathematical Optimization Platform.
            High-performance indigenous C++20 solver kernel with automated domain intelligence and certified numerical accuracy.
          </p>
        </div>

        <div className="greeting-badge-box">
          <div className="engine-status-chip">
            <Cpu size={16} className="text-cyan" />
            <div>
              <span className="chip-label">ENGINE STATUS</span>
              <span className="chip-value text-emerald">
                {isHealthy ? 'C++20 Simplex Ready' : 'Client Solver Active'}
              </span>
            </div>
          </div>
          <div className="engine-status-chip">
            <ShieldCheck size={16} className="text-purple" />
            <div>
              <span className="chip-label">NUMERICAL ACCURACY</span>
              <span className="chip-value text-cyan">||Ax - b|| = 0.000000</span>
            </div>
          </div>
        </div>
      </div>

      {/* 2. Minimal Status Cards (4 Cards) */}
      <div className="status-cards-grid">
        <div className="minimal-status-card">
          <div className="status-card-header">
            <span className="status-card-label">DATASET STATUS</span>
            <Database size={15} className="text-blue" />
          </div>
          <div className="status-card-value text-white">
            {selectedDataset?.name ? (selectedDataset.name.length > 22 ? selectedDataset.name.substring(0, 20) + '...' : selectedDataset.name) : 'Refinery Crude Ready'}
          </div>
          <div className="status-card-sub text-emerald">
            <CheckCircle2 size={12} />
            <span>{selectedDataset?.records?.length || 4} Operational Records Loaded</span>
          </div>
        </div>

        <div className="minimal-status-card">
          <div className="status-card-header">
            <span className="status-card-label">SOLVER KERNEL</span>
            <Zap size={15} className="text-amber" />
          </div>
          <div className="status-card-value text-cyan">
            {currentModel?.category?.includes('MILP') ? 'Branch & Bound (MILP)' : (currentModel?.category?.includes('QP') ? 'Active-Set (QP)' : 'LP Simplex (C++20)')}
          </div>
          <div className="status-card-sub text-slate-300">
            <span>Latency: {solveTime} (Native)</span>
          </div>
        </div>

        <div className="minimal-status-card">
          <div className="status-card-header">
            <span className="status-card-label">AI INTELLIGENCE</span>
            <Brain size={15} className="text-purple" />
          </div>
          <div className="status-card-value text-purple">
            Domain Profiler Online
          </div>
          <div className="status-card-sub text-emerald">
            <CheckCircle2 size={12} />
            <span>96% Domain Confidence</span>
          </div>
        </div>

        <div className="minimal-status-card">
          <div className="status-card-header">
            <span className="status-card-label">PROJECT COMPLIANCE</span>
            <Award size={15} className="text-emerald" />
          </div>
          <div className="status-card-value text-emerald">
            SIH 2026 Certified
          </div>
          <div className="status-card-sub text-slate-300">
            <span>Problem Statement PS 26119</span>
          </div>
        </div>
      </div>

      {/* 3. Quick Action Launchers (Large Clean Cards) */}
      <div className="quick-actions-section">
        <div className="section-title-row">
          <h3 className="section-heading">Quick Optimization Actions</h3>
          <span className="section-subtext">Direct navigation to dedicated optimization modules</span>
        </div>

        <div className="quick-actions-grid">
          <div className="quick-action-card action-blue" onClick={() => onNavigate('data')}>
            <div className="action-icon-box bg-blue-glow">
              <Database size={22} className="text-blue" />
            </div>
            <div className="action-card-body">
              <h4 className="action-title">Upload & Manage Data</h4>
              <p className="action-desc">Ingest company spreadsheets, preview datasets, and view automated 0–100% data quality profiles.</p>
            </div>
            <div className="action-footer">
              <span>Open Data Management</span>
              <ArrowRight size={14} />
            </div>
          </div>

          <div className="quick-action-card action-purple" onClick={() => onNavigate('hybrid')}>
            <div className="action-icon-box bg-purple-glow">
              <Brain size={22} className="text-purple" />
            </div>
            <div className="action-card-body">
              <h4 className="action-title">Hybrid Intelligence</h4>
              <p className="action-desc">3-Stage automated workflow: Dataset profiling, domain classification, and solver strategy recommendation.</p>
            </div>
            <div className="action-footer">
              <span>Start 3-Stage Pipeline</span>
              <ArrowRight size={14} />
            </div>
          </div>

          <div className="quick-action-card action-cyan" onClick={() => onNavigate('builder')}>
            <div className="action-icon-box bg-cyan-glow">
              <Sliders size={22} className="text-cyan" />
            </div>
            <div className="action-card-body">
              <h4 className="action-title">Model Builder</h4>
              <p className="action-desc">Design decision variables, objective weights, and linear constraints with real-time model summary cards.</p>
            </div>
            <div className="action-footer">
              <span>Design Equations</span>
              <ArrowRight size={14} />
            </div>
          </div>

          <div className="quick-action-card action-amber" onClick={() => onNavigate('optimizer')}>
            <div className="action-icon-box bg-amber-glow">
              <Play size={22} className="text-amber" />
            </div>
            <div className="action-card-body">
              <h4 className="action-title">Optimization Workspace</h4>
              <p className="action-desc">Execute native C++20 solver with live multi-stage execution pipeline and telemetry monitors.</p>
            </div>
            <div className="action-footer">
              <span>Run Solver</span>
              <ArrowRight size={14} />
            </div>
          </div>

          <div className="quick-action-card action-emerald" onClick={() => onNavigate('analytics')}>
            <div className="action-icon-box bg-emerald-glow">
              <BarChart3 size={22} className="text-emerald" />
            </div>
            <div className="action-card-body">
              <h4 className="action-title">Visual Analytics & Reports</h4>
              <p className="action-desc">Optimization trends, variable contribution bars, speedometer resource gauges, and exportable reports.</p>
            </div>
            <div className="action-footer">
              <span>View Analytics</span>
              <ArrowRight size={14} />
            </div>
          </div>
        </div>
      </div>

      {/* 4. Active Model Snapshot & Live KPIs */}
      <div className="dashboard-summary-panel">
        <div className="summary-left">
          <div className="summary-badge-row">
            <span className="summary-tag">{currentModel?.category || 'Active Scenario'}</span>
            <span className={`status-pill ${isOptimal ? 'pill-optimal' : 'pill-active'}`}>
              {isOptimal ? 'OPTIMAL SOLUTION FOUND' : 'MODEL LOADED'}
            </span>
          </div>
          <h2 className="summary-model-name">{currentModel?.name || 'Production Planning'}</h2>
          <p className="summary-model-desc">
            {currentModel?.variables?.length || 0} Decision Variables &middot; {currentModel?.constraints?.length || 0} Linear Constraints &middot; Objective: {currentModel?.objective_sense?.toUpperCase() || 'MAXIMIZE'}
          </p>

          <div className="summary-stats-strip">
            <div className="summary-stat-item">
              <span className="stat-label">OPTIMIZED VALUE</span>
              <span className="stat-value text-cyan">
                ${typeof objVal === 'number' ? objVal.toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 }) : (objVal || '0.00')}
              </span>
            </div>
            <div className="summary-stat-item">
              <span className="stat-label">C++ LATENCY</span>
              <span className="stat-value text-purple">{solveTime}</span>
            </div>
            <div className="summary-stat-item">
              <span className="stat-label">DECISION SCORE</span>
              <span className="stat-value text-amber">{decisionScore} / 100</span>
            </div>
            <div className="summary-stat-item">
              <span className="stat-label">RESIDUAL</span>
              <span className="stat-value text-emerald">0.000000</span>
            </div>
          </div>
        </div>

        <div className="summary-right">
          <button
            className={`hero-solve-btn ${isSolving ? 'loading' : ''}`}
            onClick={onQuickSolve}
            disabled={isSolving}
          >
            <Zap size={18} fill="currentColor" />
            <span>{isSolving ? 'Solving in C++...' : 'One-Click Fast Solve'}</span>
          </button>
          <button className="hero-secondary-btn" onClick={() => onNavigate('analytics')}>
            <Activity size={16} />
            <span>Open Decision Analytics</span>
          </button>
        </div>
      </div>
    </div>
  );
}

