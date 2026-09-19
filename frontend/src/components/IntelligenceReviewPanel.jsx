import React from 'react';
import { Brain, Sliders, CheckCircle2, ArrowRight, Plus, Trash2, ShieldCheck, Edit3 } from 'lucide-react';

export default function IntelligenceReviewPanel({
  suggestedModel,
  classification,
  onUpdateModel,
  onProceedToConfirmation,
  onBackToProfile
}) {
  if (!suggestedModel) return null;

  const model = suggestedModel;

  const handleUpdateVar = (idx, field, val) => {
    const updatedVars = [...model.variables];
    updatedVars[idx] = { ...updatedVars[idx], [field]: val };
    onUpdateModel({ ...model, variables: updatedVars });
  };

  const handleRemoveVar = (idx) => {
    if (model.variables.length <= 1) return;
    const updatedVars = model.variables.filter((_, i) => i !== idx);
    const updatedConstraints = model.constraints.map((c) => ({
      ...c,
      coefficients: c.coefficients.filter((_, i) => i !== idx)
    }));
    onUpdateModel({ ...model, variables: updatedVars, constraints: updatedConstraints });
  };

  const handleAddVariable = () => {
    const newIdx = model.variables.length + 1;
    const newVar = {
      name: `Stream / Item ${newIdx}`,
      type: 'continuous',
      lower_bound: 0.0,
      upper_bound: 1e100,
      objective: 10.0
    };
    const updatedConstraints = model.constraints.map((c) => ({
      ...c,
      coefficients: [...c.coefficients, 1.0]
    }));
    onUpdateModel({
      ...model,
      variables: [...model.variables, newVar],
      constraints: updatedConstraints
    });
  };

  const handleUpdateConstraint = (idx, field, val) => {
    const updatedConstraints = [...model.constraints];
    updatedConstraints[idx] = { ...updatedConstraints[idx], [field]: val };
    onUpdateModel({ ...model, constraints: updatedConstraints });
  };

  const handleUpdateConstraintCoeff = (cIdx, vIdx, val) => {
    const updatedConstraints = [...model.constraints];
    const newCoeffs = [...updatedConstraints[cIdx].coefficients];
    newCoeffs[vIdx] = parseFloat(val) || 0.0;
    updatedConstraints[cIdx] = { ...updatedConstraints[cIdx], coefficients: newCoeffs };
    onUpdateModel({ ...model, constraints: updatedConstraints });
  };

  const handleRemoveConstraint = (idx) => {
    if (model.constraints.length <= 1) return;
    const updatedConstraints = model.constraints.filter((_, i) => i !== idx);
    onUpdateModel({ ...model, constraints: updatedConstraints });
  };

  const handleAddConstraint = () => {
    const newIdx = model.constraints.length + 1;
    const newConstraint = {
      name: `Resource Constraint ${newIdx}`,
      sense: '<=',
      rhs: 100.0,
      coefficients: model.variables.map(() => 1.0)
    };
    onUpdateModel({
      ...model,
      constraints: [...model.constraints, newConstraint]
    });
  };

  return (
    <div className="view-container">
      {/* Top Banner */}
      <div className="panel-card flex-between">
        <div>
          <div className="badge-row">
            <span className="category-tag">Step 3 & 4 of 7</span>
            <span className="sense-tag">Human-in-the-Loop Review</span>
          </div>
          <h2 className="problem-main-title">Review & Edit AI Model Recommendations</h2>
          <p className="problem-desc">
            SOVOPT's intelligence layer has proposed candidate decision variables, objective weights, and linear constraints. You have full authority to accept, modify, or reject any parameter before mathematical execution.
          </p>
        </div>

        <div className="btn-row">
          <button className="secondary-btn" onClick={onBackToProfile}>
            Back
          </button>
          <button className="solve-btn" onClick={onProceedToConfirmation}>
            <span>Confirm Model Formulation</span>
            <ArrowRight size={15} />
          </button>
        </div>
      </div>

      {/* AI Recommendation Summary */}
      <div className="panel-card mt-6">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <Brain size={17} className="text-purple" />
            <h3>AI Problem Classification & Explainable Reasoning</h3>
          </div>
          <span className="panel-badge">Confidence: {Math.round((classification?.confidence || 0.88) * 100)}%</span>
        </div>

        <div className="classification-reasoning-box">
          <div className="reasoning-top">
            <span className="reasoning-domain font-bold">{classification?.problem_name || model.problem_name}</span>
            <span className="sense-pill uppercase">{model.objective_sense}</span>
          </div>
          <p className="reasoning-text">{classification?.reasoning || 'Linear programming structure with quantified decision bounds.'}</p>
        </div>
      </div>

      {/* Candidate Decision Variables */}
      <div className="panel-card mt-6">
        <div className="panel-header flex-between">
          <div className="panel-title-wrap">
            <Edit3 size={17} className="text-blue" />
            <h3>Suggested Decision Variables ({model.variables?.length || 0})</h3>
          </div>
          <button className="add-btn" onClick={handleAddVariable}>
            <Plus size={14} />
            <span>Add Custom Variable</span>
          </button>
        </div>

        <div className="table-responsive">
          <table className="sov-table">
            <thead>
              <tr>
                <th>Decision Item / Variable</th>
                <th>Type</th>
                <th>Lower Bound (Min)</th>
                <th>Upper Bound (Max)</th>
                <th>Objective Coeff (c)</th>
                <th>Action</th>
              </tr>
            </thead>
            <tbody>
              {model.variables?.map((v, i) => (
                <tr key={i}>
                  <td>
                    <input
                      type="text"
                      className="table-input font-semibold"
                      value={v.name || ''}
                      onChange={(e) => handleUpdateVar(i, 'name', e.target.value)}
                    />
                  </td>
                  <td>
                    <select
                      className="table-select"
                      value={v.type || 'continuous'}
                      onChange={(e) => handleUpdateVar(i, 'type', e.target.value)}
                    >
                      <option value="continuous">Continuous</option>
                      <option value="integer">Integer</option>
                      <option value="binary">Binary</option>
                    </select>
                  </td>
                  <td>
                    <input
                      type="number"
                      className="table-input font-mono"
                      value={v.lower_bound ?? 0.0}
                      onChange={(e) => handleUpdateVar(i, 'lower_bound', parseFloat(e.target.value) || 0.0)}
                    />
                  </td>
                  <td>
                    <input
                      type="text"
                      className="table-input font-mono"
                      value={v.upper_bound === 1e100 || v.upper_bound === undefined ? 'Infinity' : v.upper_bound}
                      onChange={(e) => {
                        const val = e.target.value === 'Infinity' || e.target.value === 'inf' ? 1e100 : (parseFloat(e.target.value) || 1e100);
                        handleUpdateVar(i, 'upper_bound', val);
                      }}
                    />
                  </td>
                  <td>
                    <input
                      type="number"
                      step="any"
                      className="table-input font-mono text-blue font-bold"
                      value={v.objective ?? 0.0}
                      onChange={(e) => handleUpdateVar(i, 'objective', parseFloat(e.target.value) || 0.0)}
                    />
                  </td>
                  <td>
                    <button
                      className="trash-btn"
                      onClick={() => handleRemoveVar(i)}
                      disabled={model.variables.length <= 1}
                      title="Remove Variable"
                    >
                      <Trash2 size={15} />
                    </button>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>

      {/* Constraints */}
      <div className="panel-card mt-6">
        <div className="panel-header flex-between">
          <div className="panel-title-wrap">
            <Sliders size={17} className="text-amber" />
            <h3>Linear Constraints ({model.constraints?.length || 0})</h3>
          </div>
          <button className="add-btn" onClick={handleAddConstraint}>
            <Plus size={14} />
            <span>Add Constraint</span>
          </button>
        </div>

        <div className="table-responsive">
          <table className="sov-table">
            <thead>
              <tr>
                <th>Constraint Name</th>
                {model.variables?.map((v, i) => (
                  <th key={i} className="font-mono text-blue">{v.name || `x${i+1}`} Coeff</th>
                ))}
                <th>Sense</th>
                <th>RHS Limit (b)</th>
                <th>Action</th>
              </tr>
            </thead>
            <tbody>
              {model.constraints?.map((c, cIdx) => (
                <tr key={cIdx}>
                  <td>
                    <input
                      type="text"
                      className="table-input font-semibold"
                      value={c.name || ''}
                      onChange={(e) => handleUpdateConstraint(cIdx, 'name', e.target.value)}
                    />
                  </td>
                  {model.variables?.map((_, vIdx) => (
                    <td key={vIdx}>
                      <input
                        type="number"
                        step="any"
                        className="table-input font-mono"
                        value={c.coefficients?.[vIdx] ?? 0.0}
                        onChange={(e) => handleUpdateConstraintCoeff(cIdx, vIdx, e.target.value)}
                      />
                    </td>
                  ))}
                  <td>
                    <select
                      className="table-select font-mono text-center font-bold"
                      value={c.sense || '<='}
                      onChange={(e) => handleUpdateConstraint(cIdx, 'sense', e.target.value)}
                    >
                      <option value="<=">≤</option>
                      <option value=">=">≥</option>
                      <option value="=">=</option>
                    </select>
                  </td>
                  <td>
                    <input
                      type="number"
                      step="any"
                      className="table-input font-mono font-bold text-amber"
                      value={c.rhs ?? 0.0}
                      onChange={(e) => handleUpdateConstraint(cIdx, 'rhs', parseFloat(e.target.value) || 0.0)}
                    />
                  </td>
                  <td>
                    <button
                      className="trash-btn"
                      onClick={() => handleRemoveConstraint(cIdx)}
                      disabled={model.constraints.length <= 1}
                      title="Remove Constraint"
                    >
                      <Trash2 size={15} />
                    </button>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}

