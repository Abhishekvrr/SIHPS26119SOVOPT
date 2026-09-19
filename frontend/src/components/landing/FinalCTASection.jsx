import React from 'react';
import { Play, ArrowRight, Layers, Sparkles } from 'lucide-react';

export default function FinalCTASection({ onLaunchWorkspace }) {
  return (
    <section className="landing-final-cta-section">
      <div className="final-cta-card">
        <div className="final-cta-badge">
          <Sparkles size={14} className="text-blue" />
          <span>SIH 2026 Sovereign Mathematical Platform</span>
        </div>

        <h2 className="final-cta-title">
          Ready to Optimize Your Industrial Decisions?
        </h2>

        <p className="final-cta-desc">
          Build custom models, ingest enterprise datasets, review AI suggestions, and execute the native C++ simplex solver with real-time explainability.
        </p>

        <button className="final-cta-btn" onClick={onLaunchWorkspace}>
          <Play size={18} fill="currentColor" />
          <span>Launch SOVOPT Optimizer</span>
          <ArrowRight size={18} />
        </button>

        <div className="final-cta-meta">
          <span>Native C++20 Core</span>
          <span>•</span>
          <span>FastAPI Service</span>
          <span>•</span>
          <span>Hybrid Intelligence Workflow</span>
        </div>
      </div>
    </section>
  );
}

