import React from 'react';
import {
  LayoutDashboard,
  Database,
  Brain,
  Sliders,
  Play,
  BarChart3,
  Activity,
  History,
  ShieldCheck,
  Zap
} from 'lucide-react';

export default function Sidebar({ activeTab, setActiveTab }) {
  const workflowNav = [
    { id: 'dashboard', label: '1. Dashboard', icon: LayoutDashboard, badge: 'Overview' },
    { id: 'data', label: '2. Data Management', icon: Database, badge: 'Profiling' },
    { id: 'hybrid', label: '3. Data Intelligence', icon: Brain, badge: 'Semantic' },
    { id: 'builder', label: '4. Model Builder', icon: Sliders, badge: 'LP Model' },
    { id: 'optimizer', label: '5. Optimization Workspace', icon: Play, badge: 'C++ Solve' },
    { id: 'analytics', label: '6. Results & Analytics', icon: BarChart3, badge: 'Decisions' },
    { id: 'wave', label: '7. Computational Wave', icon: Activity, badge: 'Simplex' },
    { id: 'history', label: '8. Dataset History', icon: History, badge: 'SQLite' },
    { id: 'architecture', label: '9. Solver / System Status', icon: ShieldCheck, badge: 'C++20' }
  ];

  return (
    <aside className="sov-sidebar">
      <div className="sidebar-top">
        <div className="sidebar-nav-group mb-4">
          <div className="sidebar-section-header">SOLVER WORKFLOW</div>
          <nav className="sidebar-nav">
            {workflowNav.map((item) => {
              const Icon = item.icon;
              const isActive = activeTab === item.id;
              return (
                <button
                  key={item.id}
                  className={`sidebar-nav-item ${isActive ? 'active' : ''}`}
                  onClick={() => setActiveTab(item.id)}
                >
                  <Icon size={16} className="nav-icon" />
                  <span className="nav-label">{item.label}</span>
                  {item.badge && (
                    <span className={`nav-badge ${isActive ? 'active-badge' : ''}`}>
                      {item.badge}
                    </span>
                  )}
                </button>
              );
            })}
          </nav>
        </div>
      </div>

      <div className="sidebar-footer">
        <div className="system-pill">
          <Zap size={14} className="pill-icon text-cyan" />
          <div className="pill-content">
            <span className="pill-title">SOVOPT Kernel v0.4</span>
            <span className="pill-sub">Two-Phase Primal Simplex (C++20)</span>
          </div>
        </div>
      </div>
    </aside>
  );
}
