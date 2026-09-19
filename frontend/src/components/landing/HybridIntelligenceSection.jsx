import React from 'react';
import { Database, Search, Brain, Edit3, Cpu, Sparkles, TrendingUp } from 'lucide-react';

export default function HybridIntelligenceSection() {
  const steps = [
    {
      num: '1',
      title: 'Company Data',
      desc: 'Ingest operational tabular records, supply caps, or CSV data files.',
      icon: Database,
      status: 'Implemented'
    },
    {
      num: '2',
      title: 'Data Profiling',
      desc: 'Automatic data quality scoring, missing value checks, and semantic column roles.',
      icon: Search,
      status: 'Implemented'
    },
    {
      num: '3',
      title: 'AI Model Suggestion',
      desc: 'Rule-based problem classification proposing candidate variables and constraints.',
      icon: Brain,
      status: 'Implemented'
    },
    {
      num: '4',
      title: 'Human Review & Edit',
      desc: 'Decision-maker reviews, customizes, and confirms the mathematical formulation.',
      icon: Edit3,
      status: 'Implemented'
    },
    {
      num: '5',
      title: 'SOVOPT C++ Engine',
      desc: 'Two-phase simplex kernel solves the exact confirmed LP model in milliseconds.',
      icon: Cpu,
      status: 'Implemented'
    },
    {
      num: '6',
      title: 'Explainable Decision',
      desc: 'Identifies 100% capacity bottlenecks, slack margins, and Optimization Score.',
      icon: Sparkles,
      status: 'Implemented'
    },
    {
      num: '7',
      title: 'Visual Analytics',
      desc: 'Before/after comparisons, matrix heatmaps, and what-if sensitivity simulations.',
      icon: TrendingUp,
      status: 'Implemented'
    }
  ];

  return (
    <section id="intelligence" className="landing-section">
      <div className="section-header-wrap text-center">
        <span className="section-eyebrow">HUMAN-IN-THE-LOOP AI</span>
        <h2 className="section-main-title">Hybrid Intelligence Architecture</h2>
        <p className="section-subtitle">
          SOVOPT is not a black-box AI model. It combines transparent data understanding with verified mathematical optimization while keeping the human engineer in complete control.
        </p>
      </div>

      <div className="hybrid-steps-grid mt-8">
        {steps.map((s, i) => {
          const Icon = s.icon;
          return (
            <div key={i} className="panel-card hybrid-step-card">
              <div className="hybrid-card-top">
                <span className="hybrid-num font-mono">{s.num}</span>
                <div className="hybrid-icon-wrap">
                  <Icon size={18} className="text-purple" />
                </div>
              </div>
              <h3 className="hybrid-title">{s.title}</h3>
              <p className="hybrid-desc">{s.desc}</p>
              <div className="hybrid-status-badge">
                <span className="status-dot" />
                <span>{s.status}</span>
              </div>
            </div>
          );
        })}
      </div>
    </section>
  );
}

