import React from 'react';
import { ShieldCheck, Lock, Unlock, CheckCircle2, Cpu, Eye } from 'lucide-react';

export default function WhySovoptSection() {
  const differentiators = [
    {
      icon: ShieldCheck,
      title: 'Algorithmic Sovereignty',
      desc: 'Eliminates strategic dependence on expensive foreign commercial solvers (CPLEX, Gurobi, Xpress) through an indigenous C++20 optimization stack.'
    },
    {
      icon: Unlock,
      title: 'Inspectable Open Architecture',
      desc: 'Transparent, modular mathematical layers from sparse matrix allocation to basis matrix factorization without hidden black-box binaries.'
    },
    {
      icon: CheckCircle2,
      title: 'Independent Solution Validation',
      desc: 'Every optimal solution is verified by a decoupled Evaluator checking $\|Ax - b\| \\le 10^{-9}$ feasibility tolerance.'
    },
    {
      icon: Eye,
      title: 'Explainable Decision Intelligence',
      desc: 'Translates raw vectors into operational intelligence: automatically identifies 100% capacity bottlenecks and computes transparent Optimization Quality Scores.'
    },
    {
      icon: Cpu,
      title: 'High-Performance C++20 Core',
      desc: 'Direct compiled machine code execution on x64 AVX2 architectures delivering sub-millisecond solve latency.'
    },
    {
      icon: Lock,
      title: 'Human-in-the-Loop Governance',
      desc: 'Intelligence proposes, the human confirms, the C++ solver calculates, and visual analytics explain.'
    }
  ];

  return (
    <section id="why-sovopt" className="landing-section">
      <div className="section-header-wrap text-center">
        <span className="section-eyebrow">STRATEGIC VALUE</span>
        <h2 className="section-main-title">Why SOVOPT?</h2>
        <p className="section-subtitle">
          An indigenous, mathematically rigorous platform built to empower Indian industrial decision-making.
        </p>
      </div>

      <div className="why-grid mt-8">
        {differentiators.map((d, i) => {
          const Icon = d.icon;
          return (
            <div key={i} className="panel-card why-card">
              <div className="why-icon-wrap">
                <Icon size={20} className="text-blue" />
              </div>
              <h3 className="why-title">{d.title}</h3>
              <p className="why-desc">{d.desc}</p>
            </div>
          );
        })}
      </div>
    </section>
  );
}

