import React from 'react';
import { ArrowUpRight, TrendingUp, Gauge, CheckCircle2 } from 'lucide-react';

export default function BeforeAfterCard({ beforeAfter, objectiveSense }) {
  if (!beforeAfter || !beforeAfter.is_available) return null;

  const isPositiveGain = beforeAfter.improvement_pct >= 0;

  return (
    <div className="panel-card before-after-container mt-6">
      <div className="panel-header">
        <div className="panel-title-wrap">
          <TrendingUp size={17} className="text-emerald" />
          <h3>Operational Impact: Baseline vs. SOVOPT Optimized</h3>
        </div>
        <span className="panel-badge font-bold text-emerald">
          {beforeAfter.improvement_pct > 0 ? `+${beforeAfter.improvement_pct}% Gain` : `${beforeAfter.improvement_pct}%`}
        </span>
      </div>

      <div className="before-after-grid">
        {/* Objective Comparison Card */}
        <div className="ba-metric-card">
          <span className="ba-card-label">
            {objectiveSense === 'maximize' ? 'Gross Return / Margin' : 'Total Operational Cost'}
          </span>
          <div className="ba-val-comparison">
            <div className="ba-val-col">
              <span className="ba-sub-label">Current Baseline</span>
              <span className="ba-val-text font-mono text-muted">
                {Number(beforeAfter.baseline_objective).toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
              </span>
            </div>
            <div className="ba-arrow-col">
              <ArrowUpRight size={20} className={isPositiveGain ? 'text-emerald' : 'text-rose'} />
            </div>
            <div className="ba-val-col">
              <span className="ba-sub-label">SOVOPT Optimized</span>
              <span className="ba-val-text font-mono text-blue font-bold">
                {Number(beforeAfter.optimized_objective).toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
              </span>
            </div>
          </div>
        </div>

        {/* Resource Efficiency Card */}
        <div className="ba-metric-card">
          <span className="ba-card-label">Resource Utilization Efficiency</span>
          <div className="ba-val-comparison">
            <div className="ba-val-col">
              <span className="ba-sub-label">Baseline Capacity</span>
              <span className="ba-val-text font-mono text-muted">{beforeAfter.baseline_resource_usage_pct}%</span>
            </div>
            <div className="ba-arrow-col">
              <Gauge size={20} className="text-emerald" />
            </div>
            <div className="ba-val-col">
              <span className="ba-sub-label">Optimized Packing</span>
              <span className="ba-val-text font-mono text-emerald font-bold">{beforeAfter.optimized_resource_usage_pct}%</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

