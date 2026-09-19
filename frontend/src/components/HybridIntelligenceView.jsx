import React, { useState } from 'react';
import { Brain, Database, CheckCircle2, ArrowRight, Layers, Sliders, AlertCircle, Play, ShieldCheck, Tag } from 'lucide-react';

export default function HybridIntelligenceView({
  dataAnalysis,
  classification,
  suggestedModel,
  onProceedToBuilder,
  onProceedToOptimize
}) {
  const [activeStage, setActiveStage] = useState(1);

  const qualityScore = dataAnalysis?.data_quality_score ?? 100.0;
  const qualityStatus = dataAnalysis?.quality_status || (dataAnalysis?.issues?.length > 0 ? "Issues Detected" : "Suitable");
  const qualityVerdict = dataAnalysis?.quality_verdict || (dataAnalysis?.issues?.length > 0 ? "Dataset contains issues that may affect optimization." : "Dataset is suitable for the current analysis pipeline.");
  const issuesList = dataAnalysis?.issues || [];

  const domainName = classification?.category || classification?.problem_name || "Industrial Production Planning";
  const problemType = classification?.problem_type || "LP";
  const reasoning = classification?.reasoning || "Identified linear capacity boundaries and profit objectives. Formulated as a Linear Program.";

  const isFormulationValid = suggestedModel?.is_valid_formulation !== false && suggestedModel?.variables?.length > 0;
  const mappingNotice = suggestedModel?.mapping_notice || "Dataset analyzed successfully, but optimization model mapping requires additional schema information.";

  return (
    <div className="view-container">
      {/* Header Banner */}
      <div className="panel-card flex-between mb-4">
        <div>
          <div className="badge-row">
            <span className="category-tag">Hybrid Intelligence Pipeline</span>
            <span className="problem-id-tag">Data-to-Model Engine</span>
          </div>
          <h2 className="problem-main-title">Semantic Profiling & Model Formulation</h2>
          <p className="problem-desc">
            Automated transformation of tabular industrial data into a mathematically verified Linear Programming model.
          </p>
        </div>

        <div className="sub-tab-pill-group">
          <button
            className={`sub-tab-pill ${activeStage === 1 ? 'active' : ''}`}
            onClick={() => setActiveStage(1)}
          >
            <span className="stage-num">1</span>
            <span>Data Understanding</span>
          </button>
          <button
            className={`sub-tab-pill ${activeStage === 2 ? 'active' : ''}`}
            onClick={() => setActiveStage(2)}
          >
            <span className="stage-num">2</span>
            <span>Semantic Classification</span>
          </button>
          <button
            className={`sub-tab-pill ${activeStage === 3 ? 'active' : ''}`}
            onClick={() => setActiveStage(3)}
          >
            <span className="stage-num">3</span>
            <span>Model Formulation</span>
          </button>
        </div>
      </div>

      {/* STAGE 1: Data Understanding */}
      {activeStage === 1 && (
        <div className="panel-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <Database size={16} className="text-cyan" />
              <h3>Stage 1 — Data Understanding & Statistical Profiling</h3>
            </div>
            <button className="solve-btn text-xs" onClick={() => setActiveStage(2)}>
              <span>Next: Semantic Classification</span>
              <ArrowRight size={12} />
            </button>
          </div>

          {/* 4-Column Metric Grid */}
          <div className="sov-grid-4">
            <div className="sov-kpi-box">
              <span className="sov-kpi-label">Quality Score</span>
              <span className="sov-kpi-value cyan">{qualityScore}%</span>
              <span className="sov-kpi-sub">Profile Accuracy</span>
            </div>
            <div className="sov-kpi-box">
              <span className="sov-kpi-label">Dimensions</span>
              <span className="sov-kpi-value">
                {dataAnalysis?.row_count ?? 0} × {dataAnalysis?.column_count ?? 0}
              </span>
              <span className="sov-kpi-sub">Rows × Columns</span>
            </div>
            <div className="sov-kpi-box">
              <span className="sov-kpi-label">Missing Cells</span>
              <span className="sov-kpi-value amber">
                {dataAnalysis?.missing_cells_pct ?? 0.0}%
              </span>
              <span className="sov-kpi-sub">{dataAnalysis?.missing_count ?? 0} missing cell(s)</span>
            </div>
            <div className="sov-kpi-box">
              <span className="sov-kpi-label">Duplicate Rows</span>
              <span className="sov-kpi-value purple">{dataAnalysis?.duplicate_count ?? 0}</span>
              <span className="sov-kpi-sub">Identical Records</span>
            </div>
          </div>

          {/* Quality Status & Verdict Callout */}
          <div className={`callout-box ${qualityStatus === 'Suitable' ? 'success' : 'warning'}`}>
            <div className="callout-title">
              {qualityStatus === 'Suitable' ? <CheckCircle2 size={16} /> : <AlertCircle size={16} />}
              <span>{qualityVerdict}</span>
            </div>
            {issuesList.length > 0 && (
              <ul className="callout-list mt-2">
                {issuesList.map((issue, idx) => (
                  <li key={idx}>{issue}</li>
                ))}
              </ul>
            )}
          </div>

          {/* 3-Column Semantic Roles Breakdown */}
          <div className="sov-grid-3">
            <div className="semantic-card-box">
              <span className="semantic-card-title text-cyan">Resource Capacity Columns</span>
              <ul className="semantic-card-list">
                {(dataAnalysis?.semantic_categories?.resource_columns || []).length > 0 ? (
                  dataAnalysis.semantic_categories.resource_columns.map((c, i) => (
                    <li key={i} className="semantic-card-item">
                      <Tag size={12} className="text-cyan" /> <span>{c}</span>
                    </li>
                  ))
                ) : (
                  <li className="text-muted italic">No resource capacity columns detected</li>
                )}
              </ul>
            </div>

            <div className="semantic-card-box">
              <span className="semantic-card-title text-emerald">Financial Margin Columns</span>
              <ul className="semantic-card-list">
                {(dataAnalysis?.semantic_categories?.financial_columns || []).length > 0 ? (
                  dataAnalysis.semantic_categories.financial_columns.map((c, i) => (
                    <li key={i} className="semantic-card-item">
                      <Tag size={12} className="text-emerald" /> <span>{c}</span>
                    </li>
                  ))
                ) : (
                  <li className="text-muted italic">No financial margin columns detected</li>
                )}
              </ul>
            </div>

            <div className="semantic-card-box">
              <span className="semantic-card-title text-purple">Demand & Quota Columns</span>
              <ul className="semantic-card-list">
                {(dataAnalysis?.semantic_categories?.demand_columns || []).length > 0 ? (
                  dataAnalysis.semantic_categories.demand_columns.map((c, i) => (
                    <li key={i} className="semantic-card-item">
                      <Tag size={12} className="text-purple" /> <span>{c}</span>
                    </li>
                  ))
                ) : (
                  <li className="text-muted italic">No demand quota columns detected</li>
                )}
              </ul>
            </div>
          </div>
        </div>
      )}

      {/* STAGE 2: Semantic Classification */}
      {activeStage === 2 && (
        <div className="panel-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <Brain size={16} className="text-cyan" />
              <h3>Stage 2 — Problem & Domain Classification</h3>
            </div>
            <button className="solve-btn text-xs" onClick={() => setActiveStage(3)}>
              <span>Next: Model Formulation</span>
              <ArrowRight size={12} />
            </button>
          </div>

          <div className="sov-grid-2 mb-4">
            <div className="sov-kpi-box">
              <span className="sov-kpi-label">Identified Problem Domain</span>
              <span className="text-base font-bold text-cyan mb-1">{domainName}</span>
              <p className="sov-kpi-sub leading-relaxed">{reasoning}</p>
            </div>

            <div className="sov-kpi-box">
              <span className="sov-kpi-label">Mathematical Classification</span>
              <div className="flex-row-wrap mb-2">
                <span className="category-tag">Linear Program (LP)</span>
                <span className="sense-tag">Two-Phase Primal Simplex</span>
              </div>
              <p className="sov-kpi-sub leading-relaxed">
                Target Solver: <strong>Native C++20 Simplex Core</strong>. All objective functions and constraints exhibit strict linear properties.
              </p>
            </div>
          </div>
        </div>
      )}

      {/* STAGE 3: Model Formulation */}
      {activeStage === 3 && (
        <div className="panel-card">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <Sliders size={16} className="text-cyan" />
              <h3>Stage 3 — Mathematical Model Formulation</h3>
            </div>
            <div className="flex-row-wrap">
              <button className="secondary-btn text-xs" onClick={onProceedToBuilder}>
                Review in Builder
              </button>
              {isFormulationValid && (
                <button className="solve-btn text-xs" onClick={onProceedToOptimize}>
                  <Play size={12} />
                  Run in Workspace
                </button>
              )}
            </div>
          </div>

          {!isFormulationValid ? (
            <div className="callout-box warning">
              <div className="callout-title">
                <AlertCircle size={16} />
                <span>Schema Requirement Notice</span>
              </div>
              <p className="text-xs text-slate-200">{mappingNotice}</p>
            </div>
          ) : (
            <div>
              {/* Objective Formula */}
              <div className="math-formula-box">
                <span className="math-formula-label">OBJECTIVE FUNCTION (MAXIMIZE PROFIT):</span>
                <div className="text-cyan font-bold">
                  MAXIMIZE Z = {' '}
                  {(suggestedModel?.variables || []).map((v, i) => (
                    <span key={i}>
                      {i > 0 ? " + " : ""}
                      {v.objective}·x<sub>{i + 1}</sub>
                    </span>
                  ))}
                </div>
              </div>

              {/* Constraints Formulas */}
              <div className="math-formula-box">
                <span className="math-formula-label">RESOURCE BOUNDARIES (SUBJECT TO):</span>
                <div className="space-y-1 text-slate-300">
                  {(suggestedModel?.constraints || []).map((c, i) => (
                    <div key={i}>
                      [{c.name}]: {c.coefficients.map((coeff, j) => (
                        <span key={j}>
                          {j > 0 && coeff >= 0 ? " + " : ""}
                          {coeff}·x<sub>{j + 1}</sub>
                        </span>
                      ))} {c.sense} {c.rhs}
                    </div>
                  ))}
                  <div className="text-muted mt-1">Non-negativity: 0 ≤ x_i ≤ demand_i (∀ i)</div>
                </div>
              </div>

              {/* Decision Variables Table */}
              <div className="data-table-container mt-4">
                <table className="enterprise-data-table">
                  <thead>
                    <tr>
                      <th>Var Symbol</th>
                      <th>Decision Description</th>
                      <th>Type</th>
                      <th>Objective Coeff (₹/ton)</th>
                      <th>Lower Bound</th>
                      <th>Upper Bound (Demand)</th>
                    </tr>
                  </thead>
                  <tbody>
                    {(suggestedModel?.variables || []).map((v, i) => (
                      <tr key={i}>
                        <td className="font-mono text-cyan font-bold">x<sub>{i + 1}</sub></td>
                        <td className="font-semibold text-primary">{v.name}</td>
                        <td className="font-mono text-xs text-muted">{v.type}</td>
                        <td className="font-mono text-emerald-400 font-bold">₹{v.objective?.toLocaleString()}</td>
                        <td className="font-mono text-xs">0.0</td>
                        <td className="font-mono text-xs text-cyan">
                          {v.upper_bound === 1e100 || v.upper_bound > 1e10 ? "∞ (Unbounded)" : v.upper_bound?.toLocaleString()}
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          )}
        </div>
      )}
    </div>
  );
}
