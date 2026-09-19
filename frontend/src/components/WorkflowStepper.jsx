import React from 'react';
import { Check } from 'lucide-react';

export default function WorkflowStepper({ currentStep, onSelectStep }) {
  const steps = [
    { num: 1, label: 'Upload Data' },
    { num: 2, label: 'Data Profiling' },
    { num: 3, label: 'AI Suggestions' },
    { num: 4, label: 'Review & Edit' },
    { num: 5, label: 'Confirm Model' },
    { num: 6, label: 'C++ Solve' },
    { num: 7, label: 'Visual Results' }
  ];

  return (
    <div className="workflow-stepper-card">
      <div className="stepper-track">
        {steps.map((s, idx) => {
          const isCompleted = currentStep > s.num;
          const isCurrent = currentStep === s.num;

          return (
            <React.Fragment key={s.num}>
              <div
                className={`step-node ${isCompleted ? 'completed' : ''} ${isCurrent ? 'current' : ''}`}
                onClick={() => onSelectStep(s.num)}
              >
                <div className="step-circle">
                  {isCompleted ? <Check size={13} strokeWidth={3} /> : s.num}
                </div>
                <span className="step-label">{s.label}</span>
              </div>

              {idx < steps.length - 1 && (
                <div className={`step-connector ${currentStep > s.num ? 'active-connector' : ''}`} />
              )}
            </React.Fragment>
          );
        })}
      </div>
    </div>
  );
}

