import React from 'react';
import { Cpu, FileCode2, Gauge, GitBranch, Layers } from 'lucide-react';

export default function WhatIsSovoptSection() {
  const cards = [
    {
      icon: Cpu,
      title: 'Indigenous C++20 Solver Core',
      desc: 'High-performance mathematical computation powered by our own Two-Phase Primal Simplex engine without foreign black-box solver licenses.',
      colorClass: 'icon-blue'
    },
    {
      icon: FileCode2,
      title: 'Canonical Linear Programming',
      desc: 'Translates raw industrial tables into linear objective functions, continuous decision variables, and physical resource boundaries.',
      colorClass: 'icon-purple'
    },
    {
      icon: Gauge,
      title: 'Industrial Decision Support',
      desc: 'Translates raw mathematical vertex coordinates into operational intelligence: binding bottlenecks, slack margins, and resource utilization.',
      colorClass: 'icon-emerald'
    },
    {
      icon: GitBranch,
      title: 'Modular Architecture',
      desc: 'Decoupled architecture featuring sparse matrix foundations, feasibility certification, and a clear roadmap for MILP & QP.',
      colorClass: 'icon-amber'
    }
  ];

  return (
    <section id="about" className="landing-section">
      <div className="section-header-wrap text-center">
        <span className="section-eyebrow">INDIGENOUS MATHEMATICAL OPTIMIZATION</span>
        <h2 className="section-main-title">SOVOPT — From Industrial Data to Mathematical Decisions</h2>
        <p className="section-subtitle max-w-3xl mx-auto">
          SOVOPT is an indigenous mathematical optimization platform that converts structured industrial planning data
          into a Linear Programming model and solves it using a native C++20 Simplex engine.
        </p>
      </div>

      {/* Actual Flow Diagram */}
      <div className="pipeline-flow-pills flex flex-wrap gap-2 items-center justify-center p-3 my-6 bg-slate-900/90 rounded-lg border border-slate-800 max-w-4xl mx-auto">
        <span className="flow-pill font-mono text-xs font-bold text-cyan">INDUSTRIAL DATA</span>
        <span className="text-muted text-xs">→</span>
        <span className="flow-pill font-mono text-xs font-bold text-cyan">DATA PROFILING</span>
        <span className="text-muted text-xs">→</span>
        <span className="flow-pill font-mono text-xs font-bold text-cyan">MODEL PARAMETERS</span>
        <span className="text-muted text-xs">→</span>
        <span className="flow-pill font-mono text-xs font-bold text-cyan">LP MODEL</span>
        <span className="text-muted text-xs">→</span>
        <span className="flow-pill font-mono text-xs font-bold text-emerald-400">SOVOPT C++20 SOLVER</span>
        <span className="text-muted text-xs">→</span>
        <span className="flow-pill font-mono text-xs font-bold text-cyan">VALIDATED SOLUTION</span>
        <span className="text-muted text-xs">→</span>
        <span className="flow-pill font-mono text-xs font-bold text-cyan">DECISION VIEW</span>
      </div>

      <div className="value-cards-grid mt-6">
        {cards.map((c, i) => {
          const Icon = c.icon;
          return (
            <div key={i} className="panel-card value-card">
              <div className={`value-icon-box ${c.colorClass}`}>
                <Icon size={22} />
              </div>
              <h3 className="value-title">{c.title}</h3>
              <p className="value-desc">{c.desc}</p>
            </div>
          );
        })}
      </div>
    </section>
  );
}
