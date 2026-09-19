import React from 'react';
import { Sliders, Brain, Cpu } from 'lucide-react';

export default function ModeSelector({ currentMode, onSelectMode }) {
  const modes = [
    {
      id: 'manual',
      label: 'Manual Mode',
      tagline: 'Direct Model Formulation',
      icon: Sliders
    },
    {
      id: 'intelligent',
      label: 'Intelligent Mode',
      tagline: 'Data Profiling & Auto-Classification',
      icon: Brain
    },
    {
      id: 'hybrid',
      label: 'Hybrid Mode',
      tagline: 'Human-in-the-Loop AI + C++ Solver',
      icon: Cpu,
      isPrimary: true
    }
  ];

  return (
    <div className="mode-selector-bar">
      <div className="mode-selector-label">
        <span>Operating Mode:</span>
      </div>
      <div className="mode-btn-group">
        {modes.map((m) => {
          const Icon = m.icon;
          const isActive = currentMode === m.id;
          return (
            <button
              key={m.id}
              className={`mode-btn ${isActive ? 'active' : ''} ${m.isPrimary ? 'primary-mode' : ''}`}
              onClick={() => onSelectMode(m.id)}
            >
              <Icon size={16} />
              <div className="mode-text-wrap">
                <span className="mode-title">{m.label}</span>
                <span className="mode-sub">{m.tagline}</span>
              </div>
            </button>
          );
        })}
      </div>
    </div>
  );
}

