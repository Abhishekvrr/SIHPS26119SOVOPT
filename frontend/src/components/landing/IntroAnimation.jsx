import React, { useEffect, useState } from 'react';
import { Layers } from 'lucide-react';

export default function IntroAnimation({ onFinish, onComplete, onSkip }) {
  const [stage, setStage] = useState(0);

  const handleFinish = onFinish || onComplete || onSkip || (() => {});

  useEffect(() => {
    // Stage 0: Initial dark computational grid (0 - 600ms)
    // Stage 1: Logo & SOVOPT title reveal (600 - 1400ms)
    // Stage 2: Subtitle & Tagline reveal (1400 - 2200ms)
    // Stage 3: Fade out transition (2300 - 2700ms)
    const t1 = setTimeout(() => setStage(1), 600);
    const t2 = setTimeout(() => setStage(2), 1400);
    const t3 = setTimeout(() => setStage(3), 2300);
    const t4 = setTimeout(() => handleFinish(), 2700);

    return () => {
      clearTimeout(t1);
      clearTimeout(t2);
      clearTimeout(t3);
      clearTimeout(t4);
    };
  }, [handleFinish]);

  return (
    <div className={`intro-overlay ${stage === 3 ? 'intro-fade-out' : ''}`}>
      <button className="intro-skip-btn" onClick={onSkip || handleFinish}>
        Skip Intro →
      </button>

      <div className="intro-content">
        <div className={`intro-logo-box ${stage >= 0 ? 'visible' : ''}`}>
          <Layers size={36} className="intro-icon" />
        </div>

        <h1 className={`intro-brand-title ${stage >= 0 ? 'visible' : ''}`}>
          <span className="letter">S</span>
          <span className="letter">O</span>
          <span className="letter">V</span>
          <span className="letter">O</span>
          <span className="letter">P</span>
          <span className="letter">T</span>
        </h1>

        <p className={`intro-brand-sub ${stage >= 1 ? 'visible' : ''}`}>
          Sovereign Mathematical Optimization
        </p>

        <div className={`intro-tagline-wrap ${stage >= 2 ? 'visible' : ''}`}>
          <span className="intro-tagline">Optimize.</span>
          <span className="intro-tagline-dot">•</span>
          <span className="intro-tagline">Analyze.</span>
          <span className="intro-tagline-dot">•</span>
          <span className="intro-tagline">Decide.</span>
        </div>
      </div>
    </div>
  );
}
