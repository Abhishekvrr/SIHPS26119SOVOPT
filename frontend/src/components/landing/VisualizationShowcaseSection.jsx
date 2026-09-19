import React from 'react';
import { TrendingUp, Activity, Grid, Sparkles, CheckCircle2 } from 'lucide-react';

export default function VisualizationShowcaseSection() {
  const iterationPoints = [
    { iter: 0, z: 0.0 },
    { iter: 1, z: 12.0 },
    { iter: 2, z: 21.33 }
  ];

  const sampleConstraints = [
    { name: 'Raw Material Inventory', usage: 8.0, limit: 8.0, pct: 100, isBinding: true },
    { name: 'Machine Operating Hours', usage: 8.0, limit: 8.0, pct: 100, isBinding: true },
    { name: 'Warehouse Storage Capacity', usage: 5.34, limit: 10.0, pct: 53.4, isBinding: false }
  ];

  return (
    <section id="visuals" className="landing-section">
      <div className="section-header-wrap text-center">
        <span className="section-eyebrow">VISUAL DECISION SUPPORT</span>
        <h2 className="section-main-title">Visual Decision Analytics Showcase</h2>
        <p className="section-subtitle">
          Optimization results should never be a wall of numbers. SOVOPT translates raw mathematical solutions into actionable visual insights.
        </p>
      </div>

      <div className="showcase-grid mt-8">
        {/* Card 1: Simplex Convergence Trajectory */}
        <div className="panel-card showcase-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <TrendingUp size={17} className="text-blue" />
              <h3>Simplex Objective Convergence</h3>
            </div>
            <span className="panel-badge font-mono">2 Iterations</span>
          </div>

          <div className="viz-demo-box">
            <svg viewBox="0 0 300 120" className="chart-svg">
              <line x1="30" y1="100" x2="280" y2="100" stroke="#cbd5e1" strokeWidth="1" />
              <line x1="30" y1="20" x2="30" y2="100" stroke="#cbd5e1" strokeWidth="1" />
              
              {/* Curve */}
              <polyline
                points="40,95 150,55 260,25"
                fill="none"
                stroke="#2563eb"
                strokeWidth="3"
              />
              
              {/* Points */}
              <circle cx="40" cy="95" r="4" fill="#2563eb" />
              <text x="35" y="112" className="chart-axis-label font-mono">Iter 0 (z=0)</text>

              <circle cx="150" cy="55" r="4" fill="#2563eb" />
              <text x="135" y="112" className="chart-axis-label font-mono">Iter 1 (z=12)</text>

              <circle cx="260" cy="25" r="5" fill="#059669" />
              <text x="210" y="20" className="chart-optimal-label font-mono">z* = 21.33 [OPT]</text>
            </svg>
          </div>
          <p className="showcase-caption">Monotonic improvement along basic feasible vertices toward global optimality.</p>
        </div>

        {/* Card 2: Constraint Utilization & Slack */}
        <div className="panel-card showcase-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <Activity size={17} className="text-amber" />
              <h3>Resource Bottlenecks & Slack</h3>
            </div>
            <span className="panel-badge">Sensitivity</span>
          </div>

          <div className="demo-constraints-list">
            {sampleConstraints.map((c, i) => (
              <div key={i} className="demo-c-item">
                <div className="demo-c-top">
                  <span className="demo-c-name">{c.name}</span>
                  <span className={`binding-pill ${c.isBinding ? 'pill-binding' : 'pill-slack'}`}>
                    {c.isBinding ? '100% BINDING' : `${c.pct}% UTILIZED`}
                  </span>
                </div>
                <div className="progress-bar-wrap">
                  <div
                    className={`progress-bar-fill ${c.isBinding ? 'fill-binding' : 'fill-slack'}`}
                    style={{ width: `${c.pct}%` }}
                  />
                </div>
                <div className="demo-c-bot font-mono">
                  <span>Usage: {c.usage} / {c.limit}</span>
                  <span>Slack: {c.isBinding ? '0.00' : (c.limit - c.usage).toFixed(2)}</span>
                </div>
              </div>
            ))}
          </div>
          <p className="showcase-caption">Instantly distinguishes 100% capacity bottlenecks from available slack reserves.</p>
        </div>

        {/* Card 3: Optimization Matrix Heatmap */}
        <div className="panel-card showcase-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <Grid size={17} className="text-purple" />
              <h3>Constraint × Variable Matrix</h3>
            </div>
            <span className="panel-badge font-mono">Coefficients</span>
          </div>

          <div className="demo-matrix-box">
            <table className="demo-matrix-table font-mono">
              <thead>
                <tr>
                  <th>Constraint</th>
                  <th>x1 (Prod A)</th>
                  <th>x2 (Prod B)</th>
                </tr>
              </thead>
              <tbody>
                <tr>
                  <td>Raw Material</td>
                  <td className="heat-cell-high">2.0</td>
                  <td className="heat-cell-med">1.0</td>
                </tr>
                <tr>
                  <td>Machine Hours</td>
                  <td className="heat-cell-med">1.0</td>
                  <td className="heat-cell-high">2.0</td>
                </tr>
              </tbody>
            </table>
          </div>
          <p className="showcase-caption">Visual representation of linear coefficient matrix density and interaction strength.</p>
        </div>
      </div>
    </section>
  );
}

