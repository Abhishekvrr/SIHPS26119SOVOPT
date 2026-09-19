import React, { useState, useEffect } from 'react';
import { Play, Compass, Sparkles } from 'lucide-react';

export default function HeroSection({ onLaunchWorkspace, onExplore }) {
  const [activeStep, setActiveStep] = useState(0);

  // Simplex pivot steps along polytope vertices: (0,0) -> (4,0) -> (2.67, 2.67) [Optimum]
  const simplexPath = [
    { x: 60, y: 260, label: 'x₀: (0, 0)', z: 'z = 0.00' },
    { x: 260, y: 260, label: 'x₁: (4.0, 0)', z: 'z = 12.00' },
    { x: 190, y: 130, label: 'x*: (2.67, 2.67)', z: 'z* = 21.33 [OPTIMAL]' }
  ];

  useEffect(() => {
    const timer = setInterval(() => {
      setActiveStep((prev) => (prev + 1) % simplexPath.length);
    }, 2200);
    return () => clearInterval(timer);
  }, [simplexPath.length]);

  return (
    <section className="landing-hero-section">
      <div className="hero-grid-container">
        {/* Left Column: Headline & Narrative */}
        <div className="hero-text-content">
          <div className="hero-eyebrow">
            <span className="sih-tag">SMART INDIA HACKATHON 2026</span>
            <span className="sih-ps-tag">PS ID: 26119</span>
          </div>

          <h1 className="hero-title">
            SOVOPT
          </h1>
          <h2 className="hero-subtitle">
            Sovereign Mathematical Optimization
          </h2>

          <p className="hero-lead">
            An indigenous mathematical optimization platform engineered in modern C++20 to solve complex industrial decision-making problems with complete algorithmic sovereignty.
          </p>

          <div className="hero-cta-group">
            <button className="hero-primary-btn" onClick={onLaunchWorkspace}>
              <Play size={16} fill="currentColor" />
              <span>Launch Optimizer</span>
            </button>
            <button className="hero-secondary-btn" onClick={onExplore}>
              <Compass size={16} />
              <span>Explore Platform</span>
            </button>
          </div>

          <div className="hero-specs-row">
            <div className="spec-item">
              <span className="spec-val font-mono">C++20</span>
              <span className="spec-label">Native Core</span>
            </div>
            <div className="spec-divider" />
            <div className="spec-item">
              <span className="spec-val font-mono">&lt; 0.05 ms</span>
              <span className="spec-label">Simplex Solve</span>
            </div>
            <div className="spec-divider" />
            <div className="spec-item">
              <span className="spec-val font-mono">100%</span>
              <span className="spec-label">Verified Exact</span>
            </div>
          </div>
        </div>

        {/* Right Column: Interactive Mathematical Simplex Polytope Canvas */}
        <div className="hero-viz-container">
          <div className="hero-viz-card">
            <div className="viz-card-header">
              <div className="viz-title-wrap">
                <Sparkles size={16} className="text-cyan" />
                <span className="viz-title">Primal Simplex Polytope Trajectory</span>
              </div>
              <span className="viz-status-pill font-mono">
                {simplexPath[activeStep].z}
              </span>
            </div>

            <div className="svg-polytope-wrapper">
              <svg viewBox="0 0 340 300" className="polytope-svg">
                {/* Grid lines */}
                <defs>
                  <pattern id="math-grid-dark" width="20" height="20" patternUnits="userSpaceOnUse">
                    <path d="M 20 0 L 0 0 0 20" fill="none" stroke="rgba(255, 255, 255, 0.05)" strokeWidth="1" />
                  </pattern>
                </defs>
                <rect width="340" height="300" fill="url(#math-grid-dark)" />

                {/* Axes */}
                <line x1="60" y1="260" x2="310" y2="260" stroke="rgba(255, 255, 255, 0.25)" strokeWidth="1.5" />
                <line x1="60" y1="260" x2="60" y2="30" stroke="rgba(255, 255, 255, 0.25)" strokeWidth="1.5" />
                <text x="315" y="265" className="axis-label font-mono" fill="#94a3b8">x₁</text>
                <text x="55" y="25" className="axis-label font-mono" fill="#94a3b8">x₂</text>

                {/* Feasible Polytope Region */}
                <polygon
                  points="60,260 260,260 190,130 60,130"
                  fill="rgba(59, 130, 246, 0.15)"
                  stroke="#3b82f6"
                  strokeWidth="1.5"
                  strokeDasharray="4 2"
                />

                {/* Constraint Line 1: 2x1 + x2 <= 8 */}
                <line x1="60" y1="60" x2="260" y2="260" stroke="#f59e0b" strokeWidth="1.5" strokeOpacity="0.85" />
                <text x="210" y="210" className="constraint-label font-mono" fill="#f59e0b">2x₁+x₂≤8</text>

                {/* Constraint Line 2: x1 + 2x2 <= 8 */}
                <line x1="60" y1="130" x2="280" y2="240" stroke="#a855f7" strokeWidth="1.5" strokeOpacity="0.85" />
                <text x="85" y="115" className="constraint-label font-mono" fill="#c084fc">x₁+2x₂≤8</text>

                {/* Objective function gradient vector arrow */}
                <line x1="60" y1="260" x2="150" y2="110" stroke="#10b981" strokeWidth="1.5" strokeDasharray="3 3" />
                <polygon points="150,110 144,118 152,118" fill="#10b981" />
                <text x="110" y="170" className="objective-label font-mono" fill="#10b981">∇c=(3,5)</text>

                {/* Simplex Active Pivot Path */}
                {activeStep >= 1 && (
                  <line
                    x1="60"
                    y1="260"
                    x2="260"
                    y2="260"
                    stroke="#38bdf8"
                    strokeWidth="3"
                  />
                )}
                {activeStep >= 2 && (
                  <line
                    x1="260"
                    y1="260"
                    x2="190"
                    y2="130"
                    stroke="#38bdf8"
                    strokeWidth="3"
                  />
                )}

                {/* Polytope Vertices */}
                {simplexPath.map((pt, idx) => {
                  const isCurrent = idx === activeStep;
                  const isOptimal = idx === 2;

                  return (
                    <g key={idx} className="vertex-group">
                      <circle
                        cx={pt.x}
                        cy={pt.y}
                        r={isCurrent ? 6 : 4}
                        fill={isOptimal ? '#10b981' : (isCurrent ? '#38bdf8' : '#64748b')}
                        stroke="#0f172a"
                        strokeWidth="1.5"
                      />
                      {isCurrent && (
                        <circle
                          cx={pt.x}
                          cy={pt.y}
                          r={12}
                          fill="none"
                          stroke={isOptimal ? '#10b981' : '#38bdf8'}
                          strokeWidth="2"
                          className="pulse-circle"
                        />
                      )}
                      <text
                        x={pt.x + 8}
                        y={pt.y - 8}
                        className={`vertex-label font-mono ${isCurrent ? 'active' : ''}`}
                        fill={isCurrent ? '#ffffff' : '#94a3b8'}
                        fontWeight={isCurrent ? '700' : '500'}
                      >
                        {pt.label}
                      </text>
                    </g>
                  );
                })}
              </svg>
            </div>

            <div className="viz-card-footer">
              <div className="viz-step-desc">
                <span className="step-tag font-mono">Step {activeStep + 1}/3:</span>
                <span className="step-info">
                  {activeStep === 0 && 'Initial Basic Feasible Vertex (Phase I)'}
                  {activeStep === 1 && 'Pivot Step 1: Entering Variable x₁'}
                  {activeStep === 2 && 'Global Optimal Solution Verified (z* = 21.33)'}
                </span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}
