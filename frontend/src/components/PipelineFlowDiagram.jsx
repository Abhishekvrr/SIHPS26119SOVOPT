import React from 'react';
import { Database, Search, Brain, FileCode, CheckCircle2, Cpu, TrendingUp, Sparkles } from 'lucide-react';

export default function PipelineFlowDiagram({ currentStep }) {
  const nodes = [
    { id: 1, label: 'Company Data', icon: Database, desc: 'CSV / JSON / Stream' },
    { id: 2, label: 'Data Profiling', icon: Search, desc: 'Quality & Semantic Types' },
    { id: 3, label: 'Intelligence', icon: Brain, desc: 'Problem Classification' },
    { id: 4, label: 'Math Model', icon: FileCode, desc: 'Variables & Constraints' },
    { id: 5, label: 'Validation', icon: CheckCircle2, desc: 'Independent Residual Check' },
    { id: 6, label: 'C++ Solver', icon: Cpu, desc: 'Two-Phase Simplex Kernel' },
    { id: 7, label: 'Optimal Decision', icon: TrendingUp, desc: 'Objective & Bottlenecks' }
  ];

  return (
    <div className="panel-card mt-6 pipeline-flow-card">
      <div className="panel-header">
        <div className="panel-title-wrap">
          <Sparkles size={17} className="text-purple" />
          <h3>Sovereign Optimization Pipeline Architecture</h3>
        </div>
        <span className="panel-badge">Hybrid Intelligence Flow</span>
      </div>

      <div className="pipeline-flow-track">
        {nodes.map((n, idx) => {
          const Icon = n.icon;
          const isPassed = currentStep >= n.id;
          const isCurrent = currentStep === n.id;

          return (
            <React.Fragment key={n.id}>
              <div className={`flow-node ${isPassed ? 'passed' : ''} ${isCurrent ? 'current' : ''}`}>
                <div className="flow-icon-circle">
                  <Icon size={16} />
                </div>
                <span className="flow-node-label">{n.label}</span>
                <span className="flow-node-desc">{n.desc}</span>
              </div>

              {idx < nodes.length - 1 && (
                <div className={`flow-connector ${currentStep > n.id ? 'active-flow' : ''}`} />
              )}
            </React.Fragment>
          );
        })}
      </div>
    </div>
  );
}

