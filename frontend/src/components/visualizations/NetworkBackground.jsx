import React, { useEffect, useRef } from 'react';

export default function NetworkBackground() {
  const canvasRef = useRef(null);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    let animationFrameId;
    let isRunning = true;

    const resizeCanvas = () => {
      const dpr = window.devicePixelRatio || 1;
      canvas.width = window.innerWidth * dpr;
      canvas.height = window.innerHeight * dpr;
      canvas.style.width = `${window.innerWidth}px`;
      canvas.style.height = `${window.innerHeight}px`;
      ctx.scale(dpr, dpr);
    };

    resizeCanvas();
    window.addEventListener('resize', resizeCanvas);

    const handleVisibilityChange = () => {
      isRunning = !document.hidden;
      if (isRunning) {
        lastTime = performance.now();
        render();
      }
    };
    document.addEventListener('visibilitychange', handleVisibilityChange);

    const isMobile = window.innerWidth < 768;
    const nodeCount = isMobile ? 22 : 45;

    const nodes = [];
    const width = window.innerWidth;
    const height = window.innerHeight;

    for (let i = 0; i < nodeCount; i++) {
      nodes.push({
        x: Math.random() * width,
        y: Math.random() * height,
        vx: (Math.random() - 0.5) * 0.28,
        vy: (Math.random() - 0.5) * 0.28,
        radius: Math.random() * 1.8 + 1.2,
        baseAlpha: Math.random() * 0.4 + 0.3,
        pulseSpeed: Math.random() * 0.02 + 0.01,
        pulseOffset: Math.random() * Math.PI * 2,
        isVertex: i < 8
      });
    }

    const sparseMatrices = [
      { text: '[ 2  1  0 ]', x: width * 0.15, y: height * 0.24, vx: 0.06, vy: -0.03, alpha: 0.16 },
      { text: '[ 1  2  1 ]', x: width * 0.80, y: height * 0.38, vx: -0.05, vy: 0.04, alpha: 0.14 },
      { text: '[ 0  1  3 ]', x: width * 0.20, y: height * 0.76, vx: 0.04, vy: 0.03, alpha: 0.15 },
      { text: 'A·x ≤ b', x: width * 0.66, y: height * 0.82, vx: -0.06, vy: -0.03, alpha: 0.18 },
      { text: 'max cᵀx', x: width * 0.86, y: height * 0.16, vx: 0.04, vy: 0.05, alpha: 0.18 }
    ];

    const keyVertices = [
      { x: width * 0.16, y: height * 0.72, label: 'x₀' },
      { x: width * 0.36, y: height * 0.48, label: 'x₁' },
      { x: width * 0.56, y: height * 0.54, label: 'x₂' },
      { x: width * 0.82, y: height * 0.32, label: 'x*' }
    ];

    let pathProgress = 0;
    let pathSpeed = 0.0038;
    let optimumPulseRadius = 0;
    let lastTime = performance.now();

    const render = () => {
      if (!isRunning) return;

      const now = performance.now();
      lastTime = now;

      const w = window.innerWidth;
      const h = window.innerHeight;

      // 1. Clear background with deep dark space gradient
      ctx.clearRect(0, 0, w, h);
      const bgGrad = ctx.createRadialGradient(w * 0.5, h * 0.35, 60, w * 0.5, h * 0.5, Math.max(w, h));
      bgGrad.addColorStop(0, '#0c1322');
      bgGrad.addColorStop(0.55, '#070b14');
      bgGrad.addColorStop(1, '#030509');
      ctx.fillStyle = bgGrad;
      ctx.fillRect(0, 0, w, h);

      // 2. Render subtle technical grid
      ctx.strokeStyle = 'rgba(255, 255, 255, 0.025)';
      ctx.lineWidth = 1;
      const gridSize = 64;
      for (let x = 0; x < w; x += gridSize) {
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, h);
        ctx.stroke();
      }
      for (let y = 0; y < h; y += gridSize) {
        ctx.beginPath();
        ctx.moveTo(0, y);
        ctx.lineTo(w, y);
        ctx.stroke();
      }

      // 3. Render Floating Sparse Matrices
      ctx.font = '11px "JetBrains Mono", monospace';
      sparseMatrices.forEach((mat) => {
        mat.x += mat.vx;
        mat.y += mat.vy;
        if (mat.x < 40) mat.x = w - 100;
        if (mat.x > w - 40) mat.x = 80;
        if (mat.y < 40) mat.y = h - 80;
        if (mat.y > h - 40) mat.y = 80;

        ctx.fillStyle = `rgba(56, 189, 248, ${mat.alpha})`;
        ctx.fillText(mat.text, mat.x, mat.y);
      });

      // 4. Update and draw nodes
      const maxDistance = 130;
      for (let i = 0; i < nodes.length; i++) {
        const node = nodes[i];
        node.x += node.vx;
        node.y += node.vy;

        if (node.x < 0 || node.x > w) node.vx *= -1;
        if (node.y < 0 || node.y > h) node.vy *= -1;

        const currentAlpha = node.baseAlpha + Math.sin(now * 0.002 + node.pulseOffset) * 0.15;

        // Draw node
        ctx.beginPath();
        ctx.arc(node.x, node.y, node.radius, 0, Math.PI * 2);
        ctx.fillStyle = node.isVertex
          ? `rgba(96, 165, 250, ${Math.max(0.3, currentAlpha)})`
          : `rgba(148, 163, 184, ${Math.max(0.18, currentAlpha * 0.6)})`;
        ctx.fill();

        // Node subtle glow
        if (node.isVertex) {
          ctx.beginPath();
          ctx.arc(node.x, node.y, node.radius * 2.8, 0, Math.PI * 2);
          ctx.fillStyle = `rgba(59, 130, 246, ${currentAlpha * 0.25})`;
          ctx.fill();
        }

        // Connect lines
        for (let j = i + 1; j < nodes.length; j++) {
          const other = nodes[j];
          const dx = other.x - node.x;
          const dy = other.y - node.y;
          const dist = Math.sqrt(dx * dx + dy * dy);

          if (dist < maxDistance) {
            const lineAlpha = (1 - dist / maxDistance) * 0.18;
            ctx.beginPath();
            ctx.moveTo(node.x, node.y);
            ctx.lineTo(other.x, other.y);
            ctx.strokeStyle = (node.isVertex && other.isVertex)
              ? `rgba(96, 165, 250, ${lineAlpha * 1.6})`
              : `rgba(148, 163, 184, ${lineAlpha})`;
            ctx.lineWidth = (node.isVertex && other.isVertex) ? 1.2 : 0.8;
            ctx.stroke();
          }
        }
      }

      // 5. Render Simplex Trajectory Path
      ctx.beginPath();
      ctx.moveTo(keyVertices[0].x, keyVertices[0].y);
      for (let k = 1; k < keyVertices.length; k++) {
        ctx.lineTo(keyVertices[k].x, keyVertices[k].y);
      }
      ctx.strokeStyle = 'rgba(56, 189, 248, 0.32)';
      ctx.lineWidth = 1.8;
      ctx.setLineDash([4, 4]);
      ctx.stroke();
      ctx.setLineDash([]);

      // Draw Key Vertices
      keyVertices.forEach((v, idx) => {
        const isOpt = idx === keyVertices.length - 1;
        ctx.beginPath();
        ctx.arc(v.x, v.y, isOpt ? 5 : 3.5, 0, Math.PI * 2);
        ctx.fillStyle = isOpt ? '#10b981' : '#38bdf8';
        ctx.fill();

        ctx.font = '10px "JetBrains Mono", monospace';
        ctx.fillStyle = isOpt ? 'rgba(16, 185, 129, 0.9)' : 'rgba(148, 163, 184, 0.7)';
        ctx.fillText(v.label, v.x + 8, v.y - 6);
      });

      // Move Simplex Tracer Particle
      pathProgress = (pathProgress + pathSpeed) % (keyVertices.length - 1);
      const segmentIndex = Math.floor(pathProgress);
      const segmentFrac = pathProgress - segmentIndex;
      const p1 = keyVertices[segmentIndex];
      const p2 = keyVertices[segmentIndex + 1];

      const tracerX = p1.x + (p2.x - p1.x) * segmentFrac;
      const tracerY = p1.y + (p2.y - p1.y) * segmentFrac;

      ctx.beginPath();
      ctx.arc(tracerX, tracerY, 4, 0, Math.PI * 2);
      ctx.fillStyle = '#38bdf8';
      ctx.fill();

      ctx.beginPath();
      ctx.arc(tracerX, tracerY, 10, 0, Math.PI * 2);
      ctx.fillStyle = 'rgba(56, 189, 248, 0.35)';
      ctx.fill();

      // Optimum Pulse Ring
      if (pathProgress > keyVertices.length - 1.2) {
        optimumPulseRadius = (optimumPulseRadius + 0.4) % 24;
        const opt = keyVertices[keyVertices.length - 1];
        ctx.beginPath();
        ctx.arc(opt.x, opt.y, optimumPulseRadius, 0, Math.PI * 2);
        ctx.strokeStyle = `rgba(16, 185, 129, ${Math.max(0, 1 - optimumPulseRadius / 24)})`;
        ctx.lineWidth = 1.5;
        ctx.stroke();
      }

      animationFrameId = requestAnimationFrame(render);
    };

    render();

    return () => {
      cancelAnimationFrame(animationFrameId);
      window.removeEventListener('resize', resizeCanvas);
      document.removeEventListener('visibilitychange', handleVisibilityChange);
    };
  }, []);

  return (
    <canvas
      ref={canvasRef}
      className="scientific-network-canvas"
      aria-hidden="true"
    />
  );
}
