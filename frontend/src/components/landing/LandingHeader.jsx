import React from 'react';
import { Layers, Cpu, ArrowRight, AlertCircle } from 'lucide-react';

export default function LandingHeader({ onLaunchWorkspace, health }) {
  const isHealthy = Boolean(health && (health.status === "healthy" || health.solver_ready !== false));

  const scrollToSection = (id) => {
    const el = document.getElementById(id);
    if (el) {
      el.scrollIntoView({ behavior: 'smooth' });
    }
  };

  return (
    <header className="landing-navbar">
      <div className="landing-navbar-left">
        <div className="brand-logo-container">
          <div className="brand-icon-box">
            <Layers className="brand-icon" size={20} />
          </div>
          <div>
            <div className="brand-title-row">
              <span className="brand-title">SOVOPT</span>
              <span className="sih-badge">SIH 2026</span>
            </div>
            <span className="brand-sub">Sovereign Mathematical Optimization</span>
          </div>
        </div>
      </div>

      <nav className="landing-nav-links">
        <button onClick={() => scrollToSection('about')}>About</button>
        <button onClick={() => scrollToSection('architecture')}>Architecture</button>
        <button onClick={() => scrollToSection('capabilities')}>Capabilities</button>
        <button onClick={() => scrollToSection('intelligence')}>Hybrid Intelligence</button>
        <button onClick={() => scrollToSection('industries')}>Industries</button>
        <button onClick={() => scrollToSection('visuals')}>Visual Analytics</button>
        <button onClick={() => scrollToSection('why-sovopt')}>Why SOVOPT</button>
      </nav>

      <div className="landing-navbar-right">
        <div className={`status-indicator ${isHealthy ? 'healthy' : 'degraded'}`}>
          {isHealthy ? (
            <>
              <span className="status-dot" />
              <Cpu size={13} />
              <span>C++ Simplex Active</span>
            </>
          ) : (
            <>
              <AlertCircle size={13} />
              <span>Backend Offline</span>
            </>
          )}
        </div>

        <button className="solve-btn nav-cta-btn" onClick={onLaunchWorkspace}>
          <span>Launch Optimizer</span>
          <ArrowRight size={14} />
        </button>
      </div>
    </header>
  );
}
