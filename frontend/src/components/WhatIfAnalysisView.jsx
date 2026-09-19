import React, { useState } from 'react';
import { Sliders, Play, TrendingUp, ArrowRight, RefreshCw, Layers } from 'lucide-react';
import { solveOptimizationProblem } from '../services/api';

export default function WhatIfAnalysisView({ baseModel, baseResult }) {
  const [scenarioModel, setScenarioModel] = useState(() => JSON.parse(JSON.stringify(baseModel)));
  const [scenarioResult, setScenarioResult] = useState(null);
  const [isSimulating, setIsSimulating] = useState(false);
  const [error, setError] = useState(null);

  const handleAdjustRHS = (idx, multiplier) => {
    const updated = JSON.parse(JSON.stringify(scenarioModel));
    updated.constraints[idx].rhs = parseFloat((updated.constraints[idx].rhs * multiplier).toFixed(2));
    setScenarioModel(updated);
  };

  const handleRunScenario = async () => {
    setIsSimulating(true);
    setError(null);
    try {
      const res = await solveOptimizationProblem(scenarioModel);
      setScenarioResult(res);
    } catch (err) {
      setError(err.message);
    } finally {
      setIsSimulating(false);
    }
  };

  const objDelta = scenarioResult && baseResult
    ? scenarioResult.objective_value - baseResult.objective_value
    : 0;

  const objDeltaPct = scenarioResult && baseResult && baseResult.objective_value !== 0
    ? ((objDelta / Math.abs(baseResult.objective_value)) * 100).toFixed(1)
    : '0.0';

  return (
    <div className="view-container">
      {/* Header */}
      <div className="panel-card flex-between">
        <div>
          <div className="badge-row">
            <span className="category-tag">Sensitivity & Simulation</span>
            <span className="sense-tag">What-If Analysis Sandbox</span>
          </div>
          <h2 className="problem-main-title">What-If Operational Scenario Simulation</h2>
          <p className="problem-desc">
            Simulate operational perturbations, resource expansions, or supply shortages to evaluate their impact on total objective return and capacity bottlenecks.
          </p>
        </div>

        <button
          className="solve-btn"
          onClick={handleRunScenario}
          disabled={isSimulating}
        >
          {isSimulating ? (
            <>
              <RefreshCw className="spin-icon" size={15} />
              <span>Simulating...</span>
            </>
          ) : (
            <>
              <Play size={15} fill="currentColor" />
              <span>Execute What-If Simulation</span>
            </>
          )}
        </button>
      </div>

      {error && (
        <div className="alert-box error-alert mt-6">
          <span>Simulation Error: {error}</span>
        </div>
      )}

      {/* Side-by-Side Comparison if simulated */}
      {scenarioResult && baseResult && (
        <div className="kpi-grid mt-6">
          <div className="kpi-card">
            <div className="kpi-icon-wrap icon-blue">
              <Layers size={22} />
            </div>
            <div className="kpi-body">
              <span className="kpi-label">Base Case Objective</span>
              <span className="kpi-value font-mono">
                {Number(baseResult.objective_value).toFixed(2)}
              </span>
              <span className="kpi-sub">Original Model Baseline</span>
            </div>
          </div>

          <div className="kpi-card kpi-optimal">
            <div className="kpi-icon-wrap icon-emerald">
              <TrendingUp size={22} />
            </div>
            <div className="kpi-body">
              <span className="kpi-label">Scenario Objective</span>
              <span className="kpi-value font-mono text-emerald">
                {Number(scenarioResult.objective_value).toFixed(2)}
              </span>
              <span className="kpi-sub">
                Delta: {objDelta >= 0 ? `+${objDelta.toFixed(2)}` : objDelta.toFixed(2)} ({objDeltaPct}%)
              </span>
            </div>
          </div>

          <div className="kpi-card">
            <div className="kpi-icon-wrap icon-purple">
              <Sliders size={22} />
            </div>
            <div className="kpi-body">
              <span className="kpi-label">Scenario Solve Time</span>
              <span className="kpi-value font-mono">
                {scenarioResult.solve_time_ms ? `${scenarioResult.solve_time_ms.toFixed(4)} ms` : '< 0.05 ms'}
              </span>
              <span className="kpi-sub">{scenarioResult.iterations} Simplex Iterations</span>
            </div>
          </div>
        </div>
      )}

      {/* Interactive Sliders / Modifiers */}
      <div className="panel-card mt-6">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <Sliders size={17} className="text-blue" />
            <h3>Resource Constraint Perturbation Controls</h3>
          </div>
          <span className="panel-badge">Adjust Limits & Re-solve</span>
        </div>

        <div className="scenario-controls-grid">
          {scenarioModel.constraints?.map((c, idx) => (
            <div key={idx} className="scenario-adjust-card">
              <div className="scenario-adjust-top">
                <span className="font-semibold text-primary">{c.name}</span>
                <span className="font-mono text-blue font-bold">RHS: {c.rhs}</span>
              </div>

              <div className="scenario-quick-actions">
                <button className="quick-adj-btn" onClick={() => handleAdjustRHS(idx, 0.85)}>
                  -15% Cap
                </button>
                <button className="quick-adj-btn" onClick={() => handleAdjustRHS(idx, 0.90)}>
                  -10% Cap
                </button>
                <button className="quick-adj-btn pill-green" onClick={() => handleAdjustRHS(idx, 1.10)}>
                  +10% Cap
                </button>
                <button className="quick-adj-btn pill-green" onClick={() => handleAdjustRHS(idx, 1.25)}>
                  +25% Cap
                </button>
              </div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}

