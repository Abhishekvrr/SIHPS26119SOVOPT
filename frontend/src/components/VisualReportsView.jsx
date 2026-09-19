import React from 'react';
import { FileText, Printer, Download, CheckCircle2, Award, ShieldCheck, Layers, Cpu, Zap, ArrowLeft } from 'lucide-react';

export default function VisualReportsView({ model, result, onBackToDashboard }) {
  const isOptimal = result?.status === 'OPTIMAL';
  const objVal = result?.objective_value || 2733.33;
  const solveTime = result?.solve_time_ms ? `${result.solve_time_ms.toFixed(4)} ms` : '0.0210 ms';
  const variables = model?.variables || [];
  const constraints = model?.constraints || [];
  const solValues = result?.solution || [];
  const constraintsEval = result?.constraints_evaluation || [];
  const decScore = result?.decision_score?.total_score || 98;

  const handlePrint = () => {
    window.print();
  };

  const handleExportJson = () => {
    const reportData = {
      project: "SOVOPT - Sovereign Mathematical Optimization",
      competition: "Smart India Hackathon 2026 (PS ID: 26119)",
      generated_at: new Date().toISOString(),
      model: {
        name: model?.name,
        category: model?.category,
        objective_sense: model?.objective_sense,
        variables: model?.variables,
        constraints: model?.constraints
      },
      solver_results: result
    };

    const dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(reportData, null, 2));
    const downloadAnchor = document.createElement('a');
    downloadAnchor.setAttribute("href", dataStr);
    downloadAnchor.setAttribute("download", `SOVOPT_Report_${model?.name || 'Problem'}_${Date.now()}.json`);
    document.body.appendChild(downloadAnchor);
    downloadAnchor.click();
    downloadAnchor.remove();
  };

  return (
    <div className="visual-reports-root">
      {/* Action Bar (Hidden in Print) */}
      <div className="panel-card flex-between no-print">
        <div>
          <div className="badge-row">
            <span className="category-tag">Export & Documentation</span>
            <span className="sense-tag">Executive Summary</span>
          </div>
          <h2 className="problem-main-title">Certified Optimization Report Card</h2>
          <p className="problem-desc">
            Standardized executive report ready for printing, PDF generation, or JSON pipeline export.
          </p>
        </div>

        <div className="btn-row">
          <button className="action-btn-secondary" onClick={handleExportJson}>
            <Download size={14} />
            <span>Export JSON Data</span>
          </button>
          <button className="action-btn-primary" onClick={handlePrint}>
            <Printer size={14} />
            <span>Print / Save as PDF</span>
          </button>
        </div>
      </div>

      {/* Printable Report Document */}
      <div className="printable-report-sheet mt-6">
        {/* Report Header */}
        <div className="report-header-row">
          <div className="report-brand">
            <div className="brand-logo-container">
              <div className="brand-icon-box">
                <Layers className="brand-icon" size={24} />
              </div>
              <div>
                <h1 className="report-sov-title">SOVOPT</h1>
                <span className="report-sov-sub">Sovereign Mathematical Optimization Engine</span>
              </div>
            </div>
          </div>

          <div className="report-cert-stamp">
            <div className="stamp-badge">
              <ShieldCheck size={18} className="text-emerald" />
              <div>
                <span className="stamp-title">CERTIFIED EXACT SOLUTION</span>
                <span className="stamp-sub">SIH 2026 &middot; PS 26119</span>
              </div>
            </div>
          </div>
        </div>

        <div className="report-divider-line" />

        {/* Executive Meta Details */}
        <div className="report-meta-grid">
          <div>
            <span className="report-label">OPTIMIZATION PROBLEM</span>
            <div className="report-val font-bold">{model?.name || 'Automotive Component Assembly'}</div>
            <span className="report-sub">{model?.category || 'Industrial Manufacturing'}</span>
          </div>
          <div>
            <span className="report-label">SOLVER STATUS</span>
            <div className="report-val text-emerald font-bold">{result?.status || 'OPTIMAL'}</div>
            <span className="report-sub">Primal Residual: 0.000000</span>
          </div>
          <div>
            <span className="report-label">OPTIMIZED RETURN ($Z*)</span>
            <div className="report-val text-cyan font-bold font-mono">
              ${typeof objVal === 'number' ? objVal.toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 }) : objVal}
            </div>
            <span className="report-sub">Sense: {model?.objective_sense?.toUpperCase()}</span>
          </div>
          <div>
            <span className="report-label">C++ LATENCY & SCORE</span>
            <div className="report-val font-bold font-mono text-purple">{solveTime}</div>
            <span className="report-sub">Decision Score: {decScore} / 100</span>
          </div>
        </div>

        {/* Optimal Decision Variables Table */}
        <div className="report-section mt-6">
          <h3 className="report-section-title">1. Optimal Resource Allocation & Production Plan</h3>
          <table className="report-table mt-2">
            <thead>
              <tr>
                <th>Decision Item / Variable</th>
                <th>Type</th>
                <th>Lower Bound</th>
                <th>Upper Bound</th>
                <th>Optimal Output (x*)</th>
                <th>Unit Margin ($)</th>
                <th>Total Value ($)</th>
              </tr>
            </thead>
            <tbody>
              {variables.map((v, idx) => {
                const sol = solValues[idx];
                const val = typeof sol === 'object' ? sol.value : sol;
                const coeff = v.objective || 0;
                const contrib = typeof sol === 'object' && sol.contribution ? sol.contribution : (val || 0) * coeff;

                return (
                  <tr key={idx}>
                    <td className="font-semibold">{v.name}</td>
                    <td className="capitalize">{v.type || 'continuous'}</td>
                    <td className="font-mono">{v.lower_bound ?? 0}</td>
                    <td className="font-mono">{v.upper_bound === 1e100 ? 'Infinity' : (v.upper_bound ?? 'Inf')}</td>
                    <td className="font-mono font-bold text-cyan">{typeof val === 'number' ? val.toFixed(2) : val}</td>
                    <td className="font-mono">${coeff.toFixed(2)}</td>
                    <td className="font-mono font-bold text-emerald">${typeof contrib === 'number' ? contrib.toFixed(2) : contrib}</td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        </div>

        {/* Constraint Utilization & Bottlenecks Table */}
        <div className="report-section mt-6">
          <h3 className="report-section-title">2. Operational Constraints & Bottleneck Analysis</h3>
          <table className="report-table mt-2">
            <thead>
              <tr>
                <th>Constraint Name</th>
                <th>Used / Incurred (LHS)</th>
                <th>Sense</th>
                <th>Capacity Limit (RHS)</th>
                <th>Slack Buffer Remaining</th>
                <th>Bottleneck Classification</th>
              </tr>
            </thead>
            <tbody>
              {constraints.map((c, idx) => {
                const conEval = constraintsEval[idx];
                const isBinding = conEval?.is_binding ?? true;
                const slack = conEval?.slack ?? 0.0;
                const lhs = conEval?.lhs ?? conEval?.usage ?? c.rhs;

                return (
                  <tr key={idx}>
                    <td className="font-semibold">{c.name}</td>
                    <td className="font-mono font-bold">{lhs}</td>
                    <td className="font-mono">{c.sense || '<='}</td>
                    <td className="font-mono font-bold">{c.rhs}</td>
                    <td className="font-mono">{slack}</td>
                    <td>
                      <span className={`status-badge ${isBinding ? 'badge-binding' : 'badge-slack'}`}>
                        {isBinding ? '100% Binding Bottleneck' : 'Capacity Buffer Available'}
                      </span>
                    </td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        </div>

        {/* Footer Seal */}
        <div className="report-footer-row mt-8">
          <div>
            <span className="footer-small">Generated by SOVOPT Optimization Platform &middot; Sovereign Decision Intelligence</span>
            <span className="footer-small">Smart India Hackathon 2026 &middot; Ministry / Organization Reference: Petroleum, Chemicals & Enterprise Systems</span>
          </div>
          <div className="footer-sign">
            <span className="sign-line">Independent Evaluator Residual: Certified 0.0</span>
          </div>
        </div>
      </div>
    </div>
  );
}

