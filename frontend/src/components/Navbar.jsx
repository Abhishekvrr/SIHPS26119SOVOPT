import React from 'react';
import { Layers, Play, RefreshCw, Cpu, CheckCircle2, AlertCircle, ArrowLeft } from 'lucide-react';

export default function Navbar({
  health,
  currentModel,
  selectedDatasetId,
  demos,
  onSelectDemo,
  onSolve,
  isSolving,
  onBackToLanding
}) {
  const isHealthy = Boolean(health && (health.status === "healthy" || health.solver_ready !== false));

  return (
    <header className="sov-navbar">
      <div className="navbar-brand">
        {onBackToLanding && (
          <button className="back-overview-btn" onClick={onBackToLanding} title="Back to Product Overview">
            <ArrowLeft size={14} />
            <span>Overview</span>
          </button>
        )}
        <div className="brand-logo-container">
          <div className="brand-icon-box">
            <Layers className="brand-icon" size={20} />
          </div>
          <div>
            <div className="brand-title-row">
              <span className="brand-title">SOVOPT</span>
              <span className="sih-badge">SIH 2026</span>
            </div>
            <span className="brand-sub">Sovereign Mathematical Optimization</span>
          </div>
        </div>
      </div>

      <div className="navbar-controls">
        <div className="demo-selector-wrapper">
          <span className="selector-label">Preset Scenario:</span>
          <select
            className="demo-select"
            value={selectedDatasetId || currentModel?.id || ""}
            onChange={(e) => onSelectDemo(e.target.value)}
          >
            {demos.map((d) => (
              <option key={d.id} value={d.id}>
                {d.name} ({d.category || d.industry})
              </option>
            ))}
            <option value="custom">Custom Problem</option>
          </select>
        </div>

        <button
          className={`solve-btn ${isSolving ? 'loading' : ''}`}
          onClick={onSolve}
          disabled={isSolving || !isHealthy}
        >
          {isSolving ? (
            <>
              <RefreshCw className="spin-icon" size={15} />
              <span>Solving...</span>
            </>
          ) : (
            <>
              <Play size={15} fill="currentColor" />
              <span>Run Optimization</span>
            </>
          )}
        </button>

        <div className={`status-indicator ${isHealthy ? 'healthy' : 'degraded'}`}>
          {isHealthy ? (
            <>
              <span className="status-dot" />
              <Cpu size={14} />
              <span>C++ Simplex Active</span>
            </>
          ) : (
            <>
              <AlertCircle size={14} />
              <span>Backend Offline</span>
            </>
          )}
        </div>
      </div>
    </header>
  );
}
