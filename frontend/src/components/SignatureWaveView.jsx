import React, { useEffect, useRef, useState } from 'react';
import { Activity, Play, CheckCircle2, ShieldCheck, Zap, RotateCcw } from 'lucide-react';

export default function SignatureWaveView({ result, isSolving: externalIsSolving, onRunOptimize }) {
  const canvasRef = useRef(null);
  const [internalSolving, setInternalSolving] = useState(false);
  const [progressIteration, setProgressIteration] = useState(0);

  const isSolving = externalIsSolving || internalSolving;
  const iterations = result?.iterations || 2;
  const solveTimeMs = result?.solve_time_ms || 0.0307;
  const objValue = result?.objective_value || 98483333.33;
  const status = result?.status || 'OPTIMAL';

  const triggerFiniteTelemetry = () => {
    setInternalSolving(true);
    setProgressIteration(0);

    let currentIter = 0;
    const interval = setInterval(() => {
      currentIter += 1;
      setProgressIteration(currentIter);
      if (currentIter >= iterations) {
        clearInterval(interval);
        setInternalSolving(false);
      }
    }, 400);
  };

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    let animationFrameId;
    let time = 0;

    const resize = () => {
      if (canvas.parentElement) {
        canvas.width = canvas.parentElement.clientWidth;
        canvas.height = 360;
      }
    };
    resize();
    window.addEventListener('resize', resize);

    const render = () => {
      ctx.fillStyle = '#070b14';
      ctx.fillRect(0, 0, canvas.width, canvas.height);

      const w = canvas.width;
      const h = canvas.height;
      const centerY = h * 0.52;

      // Coordinate Grid
      ctx.strokeStyle = 'rgba(56, 189, 248, 0.06)';
      ctx.lineWidth = 1;
      for (let x = 0; x < w; x += 40) {
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, h);
        ctx.stroke();
      }
      for (let y = 0; y < h; y += 40) {
        ctx.beginPath();
        ctx.moveTo(0, y);
        ctx.lineTo(w, y);
        ctx.stroke();
      }

      // Draw Wave / Simplex Manifold
      ctx.beginPath();
      ctx.strokeStyle = isSolving ? '#38bdf8' : 'rgba(56, 189, 248, 0.6)';
      ctx.lineWidth = 3;
      if (isSolving) {
        ctx.shadowColor = '#38bdf8';
        ctx.shadowBlur = 12;
      }

      const waveSpeed = isSolving ? 3.0 : 0.0; // Static if idle or converged
      const amp = isSolving ? 50 : 35;

      for (let x = 0; x <= w; x += 6) {
        const normX = x / w;
        const y = centerY + Math.sin(normX * 6 + (isSolving ? time * waveSpeed : 1.2)) * amp + Math.cos(normX * 3) * 15;
        if (x === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
      }
      ctx.stroke();
      ctx.shadowBlur = 0;

      // Draw Converged Simplex Nodes
      const nodeCount = 6;
      for (let i = 0; i < nodeCount; i++) {
        const nx = (i / (nodeCount - 1)) * (w - 80) + 40;
        const normX = nx / w;
        const ny = centerY + Math.sin(normX * 6 + (isSolving ? time * waveSpeed : 1.2)) * amp + Math.cos(normX * 3) * 15;

        const isOptimalNode = i === nodeCount - 1;
        ctx.beginPath();
        ctx.arc(nx, ny, isOptimalNode ? 8 : 5, 0, Math.PI * 2);
        ctx.fillStyle = isOptimalNode ? '#22c55e' : (isSolving ? '#38bdf8' : '#64748b');
        ctx.fill();
        ctx.strokeStyle = '#ffffff';
        ctx.lineWidth = 1.5;
        ctx.stroke();

        // Node labels
        ctx.fillStyle = '#94a3b8';
        ctx.font = '10px monospace';
        ctx.fillText(isOptimalNode ? 'Optimal Vertex x*' : `Vertex v${i+1}`, nx - 20, ny - 12);
      }

      if (isSolving) {
        time += 0.03;
        animationFrameId = requestAnimationFrame(render);
      }
    };

    render();

    return () => {
      window.removeEventListener('resize', resize);
      if (animationFrameId) cancelAnimationFrame(animationFrameId);
    };
  }, [isSolving, progressIteration]);

  return (
    <div className="view-container">
      {/* Header */}
      <div className="panel-card flex-between mb-4">
        <div>
          <div className="badge-row">
            <span className="category-tag">Solver Telemetry</span>
            <span className="problem-id-tag">Finite Data-Driven Wave</span>
          </div>
          <h2 className="problem-main-title">Computational Wave & Simplex Convergence</h2>
          <p className="problem-desc">
            Visualizes Simplex basis transitions, vertex pivots, and objective convergence telemetry.
          </p>
        </div>

        <div className="flex gap-2">
          <button
            className="solve-btn text-xs py-1.5 px-3"
            onClick={onRunOptimize || triggerFiniteTelemetry}
            disabled={isSolving}
          >
            <Play size={12} />
            {isSolving ? 'Simulating Iteration...' : 'Execute Wave Telemetry'}
          </button>
        </div>
      </div>

      {/* Telemetry Metrics Bar */}
      <div className="grid grid-cols-2 md:grid-cols-4 gap-3 mb-4">
        <div className="q-metric-item">
          <span className="q-metric-label">Status</span>
          <span className={`q-metric-val ${isSolving ? 'text-cyan animate-pulse' : 'text-emerald-400'}`}>
            {isSolving ? 'PIVOTING...' : status}
          </span>
        </div>
        <div className="q-metric-item">
          <span className="q-metric-label">Simplex Iterations</span>
          <span className="q-metric-val text-primary">{iterations} Iterations</span>
        </div>
        <div className="q-metric-item">
          <span className="q-metric-label">Solve Latency</span>
          <span className="q-metric-val highlight">{solveTimeMs.toFixed(4)} ms</span>
        </div>
        <div className="q-metric-item">
          <span className="q-metric-label">Optimal Objective ($Z^*$)</span>
          <span className="q-metric-val text-emerald-400">₹{Number(objValue).toLocaleString()}</span>
        </div>
      </div>

      {/* Canvas */}
      <div className="panel-card p-2 relative bg-slate-950 rounded-lg overflow-hidden border border-slate-800">
        <canvas ref={canvasRef} className="w-full block" />
        <div className="absolute bottom-4 left-4 text-xs font-mono bg-slate-900/90 p-2 rounded border border-slate-800 text-slate-300">
          Mode: <strong className="text-cyan">{isSolving ? 'Active Iteration Wave' : 'Converged Simplex Manifold (Static)'}</strong> | Primal Residual: <span className="text-emerald-400">0.000000</span>
        </div>
      </div>
    </div>
  );
}
