import React from 'react';
import { Database, Brain, Sliders, Cpu, CheckCircle2, LayoutDashboard } from 'lucide-react';

export default function TechnologySection() {
  const layers = [
    {
      step: '01',
      title: 'DATA LAYER',
      desc: 'Ingests structured CSV / tabular industrial data via RFC-4180 parsing with streaming support for large files.',
      tech: 'Structured CSV • Tabular Records',
      icon: Database
    },
    {
      step: '02',
      title: 'DATA INTELLIGENCE',
      desc: 'Performs schema inspection, data quality checks, missing-value and duplicate detection, and parameter extraction.',
      tech: 'Statistical Profiling • Semantic Tagging',
      icon: Brain
    },
    {
      step: '03',
      title: 'MODEL LAYER',
      desc: 'Formulates decision variables, objective coefficients, constraint matrix, and physical capacity bounds into standard LP JSON.',
      tech: 'Canonical LP Formulation',
      icon: Sliders
    },
    {
      step: '04',
      title: 'SOVOPT CORE',
      desc: 'Native C++20 optimization engine executing Two-Phase Primal Simplex over sparse matrix data structures.',
      tech: 'C++20 Native • Simplex Engine',
      icon: Cpu
    },
    {
      step: '05',
      title: 'VALIDATION LAYER',
      desc: 'Evaluates constraint residuals (||Ax - b|| <= 1e-7), bounds feasibility, and certifies 100% binding bottlenecks.',
      tech: 'Residual Evaluator • Certification',
      icon: CheckCircle2
    },
    {
      step: '06',
      title: 'APPLICATION LAYER',
      desc: 'FastAPI web service, React/Vite user interface, and persistent SQLite database for decision analytics and run logs.',
      tech: 'FastAPI • React • SQLite',
      icon: LayoutDashboard
    }
  ];

  return (
    <section id="architecture" className="landing-section">
      <div className="section-header-wrap text-center">
        <span className="section-eyebrow">SYSTEMS ARCHITECTURE</span>
        <h2 className="section-main-title">Current Technology Architecture</h2>
        <p className="section-subtitle max-w-2xl mx-auto">
          Structured 6-layer architecture cleanly separating presentation, data intelligence, mathematical modeling, compiled numerical solving, and validation.
        </p>
      </div>

      <div className="tech-pipeline-grid mt-8">
        {layers.map((layer, i) => {
          const Icon = layer.icon;
          return (
            <div key={i} className="panel-card tech-pipe-card">
              <div className="pipe-card-top">
                <span className="pipe-step font-mono">{layer.step}</span>
                <div className="pipe-icon-wrap">
                  <Icon size={18} className="text-blue" />
                </div>
              </div>
              <h3 className="pipe-title">{layer.title}</h3>
              <p className="pipe-desc">{layer.desc}</p>
              <div className="pipe-tech-badge font-mono">{layer.tech}</div>
            </div>
          );
        })}
      </div>

      <div className="tech-stack-pills-row mt-6">
        <span className="tech-pill">C++20 Native</span>
        <span className="tech-pill">MSVC x64 Release</span>
        <span className="tech-pill">Two-Phase Primal Simplex</span>
        <span className="tech-pill">Sparse COO / CSR Storage</span>
        <span className="tech-pill">FastAPI Web Service</span>
        <span className="tech-pill">React 19 &amp; Vite</span>
        <span className="tech-pill">SQLite Persistence</span>
        <span className="tech-pill">Zero Foreign Solver Licenses</span>
      </div>
    </section>
  );
}
