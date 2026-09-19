import React from 'react';
import NetworkBackground from '../visualizations/NetworkBackground';
import LandingHeader from './LandingHeader';
import HeroSection from './HeroSection';
import WhatIsSovoptSection from './WhatIsSovoptSection';
import TechnologySection from './TechnologySection';
import SolverCapabilitiesSection from './SolverCapabilitiesSection';
import HybridIntelligenceSection from './HybridIntelligenceSection';
import IndustrialDataSection from './IndustrialDataSection';
import VisualizationShowcaseSection from './VisualizationShowcaseSection';
import WhySovoptSection from './WhySovoptSection';
import FinalCTASection from './FinalCTASection';

export default function LandingPage({ onLaunchWorkspace, health }) {
  const handleExplore = () => {
    const el = document.getElementById('about');
    if (el) el.scrollIntoView({ behavior: 'smooth' });
  };

  return (
    <div className="landing-page-root">
      {/* 1. Animated Scientific Background Canvas */}
      <NetworkBackground />

      {/* 2. Top Glass Header */}
      <LandingHeader
        onLaunchWorkspace={onLaunchWorkspace}
        health={health}
      />

      {/* 3. Main Narrative Sections */}
      <main className="landing-main-content">
        <HeroSection
          onLaunchWorkspace={onLaunchWorkspace}
          onExplore={handleExplore}
        />

        <WhatIsSovoptSection />

        <TechnologySection />

        <SolverCapabilitiesSection />

        <HybridIntelligenceSection />

        <IndustrialDataSection />

        <VisualizationShowcaseSection />

        <WhySovoptSection />

        <FinalCTASection onLaunchWorkspace={onLaunchWorkspace} />
      </main>

      {/* 4. Footer */}
      <footer className="landing-footer">
        <div className="footer-content">
          <div className="footer-brand">
            <span className="font-bold text-primary">SOVOPT</span>
            <span className="footer-tagline">Sovereign Mathematical Optimization Platform</span>
          </div>
          <div className="footer-badge font-mono">
            Smart India Hackathon 2026 • PS 26119
          </div>
        </div>
      </footer>
    </div>
  );
}
