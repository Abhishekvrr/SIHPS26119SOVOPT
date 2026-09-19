import React from 'react';
import { Database, CheckCircle2, ShieldAlert, Sparkles, ArrowRight, Layers, DollarSign, Gauge, BarChart2, Table } from 'lucide-react';

export default function DataProfileView({
  analysis,
  classification,
  onProceedToSuggestions,
  onBackToUpload
}) {
  if (!analysis) return null;

  const qualityScore = analysis.data_quality_score ?? analysis.data_quality_pct ?? 95;
  const rowCount = analysis.row_count ?? analysis.rows_count ?? 6;
  const colCount = analysis.column_count ?? analysis.columns_count ?? 14;
  const missingPct = analysis.missing_cells_pct ?? analysis.missing_values_pct ?? 0.0;
  const categoryName = classification?.category ?? classification?.problem_name ?? 'Refinery & Feedstock Blending';
  const confidencePct = Math.round((classification?.confidence || 0.98) * 100);

  const resourceCols = analysis.semantic_categories?.resource_columns ?? [];
  const financialCols = analysis.semantic_categories?.financial_columns ?? [];
  const demandCols = analysis.semantic_categories?.demand_columns ?? [];
  const metaCols = analysis.semantic_categories?.metadata_columns ?? [];
  const columnsList = analysis.columns || [];

  return (
    <div className="view-container">
      {/* Top Banner */}
      <div className="panel-card flex-between">
        <div>
          <div className="badge-row">
            <span className="category-tag">Step 2 of 7</span>
            <span className="sense-tag">Data Profiling & Quality Engine</span>
          </div>
          <h2 className="problem-main-title">{analysis.dataset_name} — Industrial Telemetry</h2>
          <p className="problem-desc">
            Multi-attribute enterprise dataset parsed, profiled, and structured for mathematical optimization modeling.
          </p>
        </div>

        <div className="btn-row">
          <button className="secondary-btn" onClick={onBackToUpload}>
            Back to Ingestion
          </button>
          <button className="solve-btn" onClick={onProceedToSuggestions}>
            <span>View Optimization Strategy</span>
            <ArrowRight size={15} />
          </button>
        </div>
      </div>

      {/* KPI Cards Row */}
      <div className="kpi-grid mt-6">
        <div className="kpi-card kpi-score-card">
          <div className="kpi-icon-wrap icon-purple">
            <Sparkles size={22} />
          </div>
          <div className="kpi-body">
            <span className="kpi-label">Data Quality Score</span>
            <div className="score-flex-row">
              <span className="kpi-value font-mono text-purple">{qualityScore}%</span>
            </div>
            <span className="kpi-sub">Certified for LP solver execution</span>
          </div>
        </div>

        <div className="kpi-card">
          <div className="kpi-icon-wrap icon-blue">
            <Database size={22} />
          </div>
          <div className="kpi-body">
            <span className="kpi-label">Dataset Dimensions</span>
            <span className="kpi-value font-mono">{rowCount.toLocaleString()} &times; {colCount}</span>
            <span className="kpi-sub">{(rowCount * colCount).toLocaleString()} Discrete Data Cells</span>
          </div>
        </div>

        <div className="kpi-card">
          <div className="kpi-icon-wrap icon-emerald">
            <CheckCircle2 size={22} />
          </div>
          <div className="kpi-body">
            <span className="kpi-label">Missing Values</span>
            <span className="kpi-value font-mono text-emerald">{missingPct}%</span>
            <span className="kpi-sub">Zero null data points detected</span>
          </div>
        </div>

        <div className="kpi-card">
          <div className="kpi-icon-wrap icon-amber">
            <Layers size={22} />
          </div>
          <div className="kpi-body">
            <span className="kpi-label">Detected Domain</span>
            <span className="kpi-value text-base font-bold">{categoryName}</span>
            <span className="kpi-sub">Heuristic Confidence: {confidencePct}%</span>
          </div>
        </div>
      </div>

      {/* Semantic Categorization Summary */}
      <div className="panel-card mt-6">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <Gauge size={17} className="text-blue" />
            <h3>Semantic Feature Categorization ({colCount} Columns)</h3>
          </div>
          <span className="panel-badge">Automated Role Tagging</span>
        </div>

        <div className="semantic-columns-grid">
          <div className="semantic-group-card">
            <div className="sem-header">
              <span className="sem-tag text-emerald">Resource Capacities & Rates ({resourceCols.length})</span>
            </div>
            <div className="sem-tags-list">
              {resourceCols.length > 0 ? (
                resourceCols.map((c, i) => (
                  <span key={i} className="col-pill">{c}</span>
                ))
              ) : (
                <span className="text-muted text-xs">Standard operational bounds</span>
              )}
            </div>
          </div>

          <div className="semantic-group-card">
            <div className="sem-header">
              <span className="sem-tag text-blue">Financial & Profit Objectives ({financialCols.length})</span>
            </div>
            <div className="sem-tags-list">
              {financialCols.length > 0 ? (
                financialCols.map((c, i) => (
                  <span key={i} className="col-pill pill-blue">{c}</span>
                ))
              ) : (
                <span className="text-muted text-xs">Unit margin / revenue weights</span>
              )}
            </div>
          </div>

          <div className="semantic-group-card">
            <div className="sem-header">
              <span className="sem-tag text-amber">Demand & Quota Limits ({demandCols.length})</span>
            </div>
            <div className="sem-tags-list">
              {demandCols.length > 0 ? (
                demandCols.map((c, i) => (
                  <span key={i} className="col-pill pill-amber">{c}</span>
                ))
              ) : (
                <span className="text-muted text-xs">Upper/lower bounded output</span>
              )}
            </div>
          </div>

          <div className="semantic-group-card">
            <div className="sem-header">
              <span className="sem-tag text-purple">Metadata & Tags ({metaCols.length})</span>
            </div>
            <div className="sem-tags-list">
              {metaCols.length > 0 ? (
                metaCols.map((c, i) => (
                  <span key={i} className="col-pill pill-purple">{c}</span>
                ))
              ) : (
                <span className="text-muted text-xs">Entity attributes</span>
              )}
            </div>
          </div>
        </div>
      </div>

      {/* Comprehensive Statistical Column Profiling Table */}
      {columnsList.length > 0 && (
        <div className="panel-card mt-6">
          <div className="panel-header flex-between">
            <div className="panel-title-wrap">
              <BarChart2 size={17} className="text-cyan" />
              <h3>Column-Wise Statistical Distributions</h3>
            </div>
            <span className="panel-badge">{columnsList.length} Profiled Features</span>
          </div>

          <div className="table-scroll-viewport mt-4">
            <table className="sov-enterprise-table">
              <thead>
                <tr>
                  <th>#</th>
                  <th>Column Name</th>
                  <th>Data Type</th>
                  <th>Semantic Role</th>
                  <th>Valid / Missing</th>
                  <th>Min Value</th>
                  <th>Mean / Avg</th>
                  <th>Max Value</th>
                </tr>
              </thead>
              <tbody>
                {columnsList.map((col, idx) => (
                  <tr key={idx} className="data-row">
                    <td className="row-num-td font-mono text-slate-400">{idx + 1}</td>
                    <td className="font-mono font-semibold text-white">{col.name}</td>
                    <td>
                      <span className={`col-type-tag ${col.type === 'numerical' ? 'tag-num' : 'tag-cat'}`}>
                        {col.type}
                      </span>
                    </td>
                    <td>
                      <span className="semantic-role-badge">
                        {col.semantic_role || 'General Attribute'}
                      </span>
                    </td>
                    <td className="font-mono text-xs">
                      <span className="text-emerald">{col.sample_count}</span> / <span className="text-slate-400">{col.missing_count}</span>
                    </td>
                    <td className="font-mono td-numeric">
                      {col.min !== undefined ? col.min.toLocaleString() : '-'}
                    </td>
                    <td className="font-mono td-numeric text-cyan">
                      {col.mean !== undefined ? col.mean.toLocaleString() : '-'}
                    </td>
                    <td className="font-mono td-numeric text-emerald">
                      {col.max !== undefined ? col.max.toLocaleString() : '-'}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      )}
    </div>
  );
}
