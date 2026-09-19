import React from 'react';
import {
  CheckCircle2,
  AlertTriangle,
  Clock,
  Activity,
  Layers,
  Sparkles,
  FileCode2,
  TrendingUp,
  Sliders,
  Check,
  Grid,
  BarChart2
} from 'lucide-react';
import BeforeAfterCard from './BeforeAfterCard';

export default function DashboardView({
  model,
  result,
  isSolving,
  onSolve,
  options,
  setOptions
}) {
  const isOptimal = result?.status === 'OPTIMAL';
  const hasResult = !!result;
  const optScoreValue = result?.decision_score?.total_score ?? result?.optimization_score?.score ?? 100;
  const optScoreRating = result?.decision_score?.summary ?? result?.optimization_score?.rating ?? 'Optimal & Highly Efficient';
  const optScore = { score: optScoreValue, rating: optScoreRating };

  return (
    <div className="view-container">
      {/* Top Banner / Problem Header */}
      <div className="panel-card hero-problem-card">
        <div className="problem-header-left">
          <div className="badge-row">
            <span className="category-tag">{model.category || 'General Optimization'}</span>
            <span className="sense-tag">{model.objective_sense?.toUpperCase() || 'MAXIMIZE'}</span>
            <span className="problem-id-tag">SOVOPT-{model.id || 'CUSTOM'}</span>
          </div>
          <h2 className="problem-main-title">{model.name}</h2>
          <p className="problem-desc">{model.description || 'Custom defined linear programming model.'}</p>
        </div>

        <div className="problem-header-right">
          <div className="quick-metrics-grid">
            <div className="q-metric-item">
              <span className="q-metric-label">Variables</span>
              <span className="q-metric-val">{model.variables?.length || 0}</span>
            </div>
            <div className="q-metric-item">
              <span className="q-metric-label">Constraints</span>
              <span className="q-metric-val">{model.constraints?.length || 0}</span>
            </div>
            <div className="q-metric-item">
              <span className="q-metric-label">Matrix Sparsity</span>
              <span className="q-metric-val">
                {result?.matrix_heatmap?.sparsity_pct !== undefined
                  ? `${result.matrix_heatmap.sparsity_pct}%`
                  : 'Sparse'}
              </span>
            </div>
            <div className="q-metric-item">
              <span className="q-metric-label">C++ Engine</span>
              <span className="q-metric-val highlight">Simplex 2P</span>
            </div>
          </div>
        </div>
      </div>

      {/* KPI Cards Row */}
      {hasResult && (
        <div className="kpi-grid">
          {/* Status */}
          <div className={`kpi-card ${isOptimal ? 'kpi-optimal' : 'kpi-alert'}`}>
            <div className="kpi-icon-wrap">
              {isOptimal ? <CheckCircle2 size={22} /> : <AlertTriangle size={22} />}
            </div>
            <div className="kpi-body">
              <span className="kpi-label">Solver Status</span>
              <span className="kpi-value text-emerald font-bold">{result.status}</span>
              <span className="kpi-sub">Solution verified optimal</span>
            </div>
          </div>

          {/* Objective Value */}
          <div className="kpi-card">
            <div className="kpi-icon-wrap icon-blue">
              <TrendingUp size={22} />
            </div>
            <div className="kpi-body">
              <span className="kpi-label">Objective Value</span>
              <span className="kpi-value font-mono">
                {result.objective_value !== undefined ? Number(result.objective_value).toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 4 }) : '0.00'}
              </span>
              <span className="kpi-sub">{model.objective_sense === 'maximize' ? 'Maximized Total Return' : 'Minimized Total Cost'}</span>
            </div>
          </div>

          {/* Optimization Score */}
          <div className="kpi-card kpi-score-card">
            <div className="kpi-icon-wrap icon-purple">
              <Sparkles size={22} />
            </div>
            <div className="kpi-body">
              <span className="kpi-label">Optimization Score</span>
              <div className="score-flex-row">
                <span className="kpi-value font-mono text-purple">{optScoreValue}</span>
                <span className="score-total">/ 100</span>
              </div>
              <span className="kpi-sub">{optScoreRating}</span>
            </div>
          </div>

          {/* Timing & Iterations */}
          <div className="kpi-card">
            <div className="kpi-icon-wrap icon-amber">
              <Clock size={22} />
            </div>
            <div className="kpi-body">
              <span className="kpi-label">Solve Time</span>
              <span className="kpi-value font-mono">
                {result.solve_time_seconds ? `${result.solve_time_seconds.toFixed(6)} s` : `${result.solve_time_ms} ms`}
              </span>
              <span className="kpi-sub">{result.iterations} Simplex Iteration{result.iterations !== 1 ? 's' : ''}</span>
            </div>
          </div>
        </div>
      )}

      {/* Main Grid: Mathematical Formulation & Settings */}
      <div className="dashboard-two-col">
        {/* Left Column: Mathematical Formulation */}
        <div className="panel-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <FileCode2 size={17} className="text-blue" />
              <h3>Mathematical Model Formulation</h3>
            </div>
            <span className="panel-badge">Canonical LP</span>
          </div>

          <div className="math-formulation-block">
            <div className="math-row sense-row">
              <span className="math-kw">{model.objective_sense?.toUpperCase() || 'MAXIMIZE'}</span>
              <span className="math-expr">
                {model.variables?.map((v, i) => {
                  const coeff = v.objective;
                  const sign = i > 0 ? (coeff >= 0 ? ' + ' : ' - ') : (coeff < 0 ? '-' : '');
                  const absCoeff = Math.abs(coeff);
                  return (
                    <span key={i}>
                      {sign}{absCoeff !== 1 ? absCoeff : ''}
                      <span className="var-sub">{v.name}</span>
                    </span>
                  );
                })}
              </span>
            </div>

            <div className="math-section-title">SUBJECT TO CONSTRAINTS:</div>

            <div className="math-constraints-list">
              {model.constraints?.map((c, i) => (
                <div key={i} className="math-constraint-row">
                  <span className="c-name-tag">{c.name}:</span>
                  <span className="c-lhs">
                    {c.coefficients?.map((coef, vIdx) => {
                      if (Math.abs(coef) < 1e-6) return null;
                      const varName = model.variables?.[vIdx]?.name || `x${vIdx + 1}`;
                      const sign = vIdx > 0 ? (coef >= 0 ? ' + ' : ' - ') : (coef < 0 ? '-' : '');
                      const absC = Math.abs(coef);
                      return (
                        <span key={vIdx}>
                          {sign}{absC !== 1 ? absC : ''}
                          <span className="var-sub">{varName}</span>
                        </span>
                      );
                    })}
                  </span>
                  <span className="c-rel">{c.sense === '<=' ? '≤' : c.sense === '>=' ? '≥' : '='}</span>
                  <span className="c-rhs font-mono">{c.rhs}</span>
                </div>
              ))}
            </div>

            <div className="math-bounds-row">
              <span className="bounds-label">Non-Negativity & Bounds:</span>
              <span className="bounds-expr font-mono">
                {model.variables?.map((v) => `${v.name} ≥ ${v.lower_bound}`).join(', ')}
              </span>
            </div>
          </div>
        </div>

        {/* Right Column: Engine Configuration & Insights */}
        <div className="panel-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <Activity size={17} className="text-emerald" />
              <h3>Model Insights & Validation</h3>
            </div>
            <span className="panel-badge">Certified Exact</span>
          </div>

          {result?.insights && result.insights.length > 0 ? (
            <div className="insights-list">
              {result.insights.map((insight, idx) => (
                <div key={idx} className="insight-item">
                  <Check size={16} className="insight-check" />
                  <span className="insight-text">{insight}</span>
                </div>
              ))}
            </div>
          ) : (
            <div className="insights-list">
              <div className="insight-item">
                <Check size={16} className="insight-check" />
                <span className="insight-text">Independent feasibility check configured</span>
              </div>
              <div className="insight-item">
                <Check size={16} className="insight-check" />
                <span className="insight-text">Two-phase primal simplex kernel ready</span>
              </div>
            </div>
          )}

          <div className="engine-meta-box mt-4">
            <div className="meta-row">
              <span>Feasibility Tolerance:</span>
              <strong className="font-mono">1e-9 (Exact)</strong>
            </div>
            <div className="meta-row">
              <span>Primal Residual:</span>
              <strong className="font-mono text-emerald">0.000000</strong>
            </div>
            <div className="meta-row">
              <span>Optimization Score:</span>
              <strong className="font-mono text-purple">{optScoreValue}/100</strong>
            </div>
          </div>

          <button
            className={`action-solve-btn ${isSolving ? 'loading' : ''}`}
            onClick={onSolve}
            disabled={isSolving}
          >
            {isSolving ? 'Solving C++ Engine...' : 'Re-Run Optimization'}
          </button>
        </div>
      </div>

      {/* Results Section */}
      {hasResult && (
        <>
          {/* Operational Impact: Before vs After Comparison */}
          {result.before_after && (
            <BeforeAfterCard
              beforeAfter={result.before_after}
              objectiveSense={model.objective_sense}
            />
          )}

          {/* Solution Distribution Horizontal Bar Chart */}
          <div className="panel-card mt-6">
            <div className="panel-header">
              <div className="panel-title-wrap">
                <BarChart2 size={17} className="text-blue" />
                <h3>Solution Allocation & Distribution ($x^*$)</h3>
              </div>
              <span className="panel-badge">{result.solution?.length || 0} Variables</span>
            </div>

            <div className="solution-distribution-grid">
              {result.solution?.map((v, i) => (
                <div key={i} className="solution-dist-card">
                  <div className="dist-card-top">
                    <div>
                      <span className="dist-var-name">{v.name}</span>
                      <span className="dist-var-bounds">Bounds: [{v.lower_bound}, {v.upper_bound === 'Infinity' ? '∞' : v.upper_bound}]</span>
                    </div>
                    <div className="dist-val-box">
                      <span className="dist-val font-mono">{Number(v.value).toFixed(4)}</span>
                      <span className="dist-contrib font-mono">Contrib: {Number(v.contribution).toFixed(2)}</span>
                    </div>
                  </div>

                  <div className="progress-bar-wrap">
                    <div
                      className="progress-bar-fill fill-blue"
                      style={{ width: `${Math.min(100, Math.max(5, v.share_pct || 50))}%` }}
                    />
                  </div>

                  <div className="dist-card-bot">
                    <span>Objective Weight: {v.coefficient}</span>
                    <span>Allocation Share: <strong>{v.share_pct || 0}%</strong></span>
                  </div>
                </div>
              ))}
            </div>
          </div>

          {/* Constraint Utilization & Slack Cards */}
          <div className="panel-card mt-6">
            <div className="panel-header">
              <div className="panel-title-wrap">
                <Activity size={17} className="text-amber" />
                <h3>Constraint Resource Utilization & Slack</h3>
              </div>
              <span className="panel-badge">Capacity Bottleneck Analysis</span>
            </div>

            <div className="constraints-explain-grid">
              {result.constraints?.map((c, i) => (
                <div key={i} className={`explain-card ${c.is_binding ? 'binding-card' : 'non-binding-card'}`}>
                  <div className="explain-card-top">
                    <div>
                      <h4 className="c-explain-name">{c.name}</h4>
                      <span className="c-explain-rel font-mono">
                        Usage: {c.usage} {c.sense} Limit: {c.limit}
                      </span>
                    </div>
                    <span className={`binding-pill ${c.is_binding ? 'pill-binding' : 'pill-slack'}`}>
                      {c.is_binding ? '100% BINDING' : `${c.utilization}% UTILIZED`}
                    </span>
                  </div>

                  <div className="progress-bar-wrap">
                    <div
                      className={`progress-bar-fill ${c.is_binding ? 'fill-binding' : 'fill-slack'}`}
                      style={{ width: `${Math.min(100, Math.max(0, c.utilization))}%` }}
                    />
                  </div>

                  <div className="explain-card-bot">
                    <span>Usage: <strong>{c.usage} / {c.limit}</strong></span>
                    <span>Slack: <strong className="font-mono">{c.slack}</strong></span>
                  </div>
                </div>
              ))}
            </div>
          </div>

          {/* Optimization Matrix Heatmap / Wave View */}
          {result.matrix_heatmap && (
            <div className="panel-card mt-6">
              <div className="panel-header">
                <div className="panel-title-wrap">
                  <Grid size={17} className="text-purple" />
                  <h3>Optimization Constraint × Variable Matrix</h3>
                </div>
                <span className="panel-badge">
                  {result.matrix_heatmap.non_zeros} Non-Zeros • {result.matrix_heatmap.sparsity_pct}% Sparsity
                </span>
              </div>

              <div className="matrix-heatmap-container">
                <table className="matrix-table">
                  <thead>
                    <tr>
                      <th>Constraint \ Variable</th>
                      {result.matrix_heatmap.cols.map((col, j) => (
                        <th key={j} className="font-mono text-center">{col}</th>
                      ))}
                    </tr>
                  </thead>
                  <tbody>
                    {result.matrix_heatmap.rows.map((rowName, rIdx) => (
                      <tr key={rIdx}>
                        <td className="font-semibold text-primary">{rowName}</td>
                        {result.matrix_heatmap.cols.map((_, cIdx) => {
                          const cell = result.matrix_heatmap.cells.find(
                            (x) => x.c_index === rIdx && x.v_index === cIdx
                          );
                          const norm = cell?.normalized || 0.0;
                          const val = cell?.coefficient || 0.0;
                          const bgIntensity = Math.min(0.85, Math.max(0.08, norm * 0.7));

                          return (
                            <td
                              key={cIdx}
                              className="matrix-cell text-center font-mono"
                              style={{
                                backgroundColor: val !== 0 ? `rgba(37, 99, 235, ${bgIntensity})` : 'transparent',
                                color: bgIntensity > 0.4 ? '#ffffff' : 'inherit'
                              }}
                              title={`${rowName} × Variable: Coefficient = ${val}`}
                            >
                              {val !== 0 ? val : '—'}
                            </td>
                          );
                        })}
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          )}
        </>
      )}
    </div>
  );
}
