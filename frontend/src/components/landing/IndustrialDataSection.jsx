import React from 'react';
import { Fuel, Factory, Truck, Zap, Landmark } from 'lucide-react';

export default function IndustrialDataSection() {
  const industries = [
    {
      icon: Fuel,
      domain: 'Petroleum & Refining',
      badge: 'SIH Problem Domain',
      title: 'Feedstock & Crude Blending',
      desc: 'Optimizes refinery throughput across crude cuts to maximize gross margin while meeting sulfur, octane, and distillation constraints.',
      variables: '4 Crude Streams',
      objective: 'Maximize Gross Refining Margin'
    },
    {
      icon: Factory,
      domain: 'Industrial Manufacturing',
      badge: 'Assembly Operations',
      title: 'Multi-Product Production Planning',
      desc: 'Balances standard, deluxe, and EV component production lines constrained by CNC machine hours, skilled labor, and raw steel inventory.',
      variables: '3 Product Lines',
      objective: 'Maximize Monthly Net Profit'
    },
    {
      icon: Truck,
      domain: 'Supply Chain & Logistics',
      badge: 'Freight Routing',
      title: 'Inter-State Freight Dispatch',
      desc: 'Minimizes freight transportation expenditure from central logistics hubs to regional fulfillment centers under vehicle capacity limits.',
      variables: '6 Hub-to-Hub Routes',
      objective: 'Minimize Total Transport Cost'
    },
    {
      icon: Zap,
      domain: 'Energy & Utilities',
      badge: 'Grid Dispatch',
      title: 'Renewable Power & Grid Balancing',
      desc: 'Optimizes thermal and solar/wind generation schedules to satisfy peak hourly electricity demand within transmission line caps.',
      variables: 'Generation Units',
      objective: 'Minimize Levelized Energy Cost'
    },
    {
      icon: Landmark,
      domain: 'Finance & Infrastructure',
      badge: 'Asset Allocation',
      title: 'Sovereign Capital Allocation',
      desc: 'Allocates investment capital across highway trusts, solar InvITs, and treasury bills to maximize return under risk exposure ceilings.',
      variables: '4 Asset Classes',
      objective: 'Maximize Portfolio Yield'
    }
  ];

  return (
    <section id="industries" className="landing-section">
      <div className="section-header-wrap text-center">
        <span className="section-eyebrow">MULTI-INDUSTRY VERSATILITY</span>
        <h2 className="section-main-title">From Company Data to Decisions</h2>
        <p className="section-subtitle">
          SOVOPT is designed as a general-purpose mathematical optimization platform supporting enterprise data from any sector.
        </p>
      </div>

      <div className="industries-grid mt-8">
        {industries.map((ind, i) => {
          const Icon = ind.icon;
          return (
            <div key={i} className="panel-card industry-card">
              <div className="ind-card-top">
                <div className="ind-icon-wrap">
                  <Icon size={20} className="text-blue" />
                </div>
                <span className="category-tag">{ind.badge}</span>
              </div>
              <span className="ind-domain-sub">{ind.domain}</span>
              <h3 className="ind-title">{ind.title}</h3>
              <p className="ind-desc">{ind.desc}</p>
              <div className="ind-footer">
                <div className="ind-meta-item">
                  <span className="ind-meta-label">Decisions:</span>
                  <span className="ind-meta-val font-mono">{ind.variables}</span>
                </div>
                <div className="ind-meta-item">
                  <span className="ind-meta-label">Target:</span>
                  <span className="ind-meta-val">{ind.objective}</span>
                </div>
              </div>
            </div>
          );
        })}
      </div>
    </section>
  );
}

