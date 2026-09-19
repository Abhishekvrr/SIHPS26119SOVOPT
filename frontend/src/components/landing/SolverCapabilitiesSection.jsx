import React from 'react';
import { ShieldCheck, Check, Circle, Zap, Layers } from 'lucide-react';

export default function SolverCapabilitiesSection() {
  return (
    <section id="capabilities" className="landing-section">
      <div className="section-header-wrap text-center">
        <span className="section-eyebrow">TECHNICAL GROUND TRUTH</span>
        <h2 className="section-main-title">Verified Solver Capabilities</h2>
        <p className="section-subtitle max-w-2xl mx-auto">
          Honest, transparent categorization of what is genuinely implemented today, what forms our core foundation, and what is planned on our roadmap.
        </p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mt-8 max-w-5xl mx-auto">
        {/* IMPLEMENTED */}
        <div className="p-5 rounded-xl bg-emerald-950/20 border border-emerald-800/50">
          <div className="flex items-center justify-between mb-4 pb-2 border-b border-emerald-800/40">
            <h3 className="font-bold text-base text-emerald-400">IMPLEMENTED</h3>
            <span className="px-2.5 py-0.5 text-xs font-bold rounded bg-emerald-500/20 text-emerald-300 border border-emerald-500/40">
              Active &amp; Tested
            </span>
          </div>
          <ul className="space-y-2.5 text-xs text-slate-300">
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> Linear Programming (LP)</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> LP model construction</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> Objective handling (Max/Min)</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> Constraint handling (&lt;=, &gt;=, ==)</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> Simplex iterations (Two-Phase)</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> Feasibility / solution status</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> Native C++20 execution (MSVC)</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> CMake build automation</li>
            <li className="flex items-center gap-2"><Check size={14} className="text-emerald-400 flex-shrink-0" /> Solver result integration</li>
          </ul>
        </div>

        {/* FOUNDATION */}
        <div className="p-5 rounded-xl bg-blue-950/20 border border-blue-800/50">
          <div className="flex items-center justify-between mb-4 pb-2 border-b border-blue-800/40">
            <h3 className="font-bold text-base text-cyan">FOUNDATION</h3>
            <span className="px-2.5 py-0.5 text-xs font-bold rounded bg-blue-500/20 text-cyan border border-blue-500/40">
              Core Data Layer
            </span>
          </div>
          <ul className="space-y-2.5 text-xs text-slate-300">
            <li className="flex items-center gap-2"><span className="w-2 h-2 rounded-full bg-cyan"></span> Sparse matrix structures</li>
            <li className="flex items-center gap-2"><span className="w-2 h-2 rounded-full bg-cyan"></span> Numerical computation layer</li>
            <li className="flex items-center gap-2"><span className="w-2 h-2 rounded-full bg-cyan"></span> Modular solver architecture</li>
            <li className="flex items-center gap-2"><span className="w-2 h-2 rounded-full bg-cyan"></span> Direct stdio JSON protocol bridge</li>
            <li className="flex items-center gap-2"><span className="w-2 h-2 rounded-full bg-cyan"></span> SQLite dataset persistence</li>
          </ul>
        </div>

        {/* ROADMAP */}
        <div className="p-5 rounded-xl bg-slate-900/80 border border-slate-800">
          <div className="flex items-center justify-between mb-4 pb-2 border-b border-slate-800">
            <h3 className="font-bold text-base text-slate-400">ROADMAP</h3>
            <span className="px-2.5 py-0.5 text-xs font-bold rounded bg-slate-800 text-slate-400 border border-slate-700">
              Future Work
            </span>
          </div>
          <ul className="space-y-2.5 text-xs text-slate-400">
            <li className="flex items-center gap-2"><Circle size={10} className="text-slate-500 flex-shrink-0" /> Mixed-Integer Linear Programming (MILP)</li>
            <li className="flex items-center gap-2"><Circle size={10} className="text-slate-500 flex-shrink-0" /> Quadratic Programming (QP)</li>
            <li className="flex items-center gap-2"><Circle size={10} className="text-slate-500 flex-shrink-0" /> Advanced presolve &amp; dual simplex</li>
            <li className="flex items-center gap-2"><Circle size={10} className="text-slate-500 flex-shrink-0" /> Branch-and-bound / Branch-and-cut</li>
            <li className="flex items-center gap-2"><Circle size={10} className="text-slate-500 flex-shrink-0" /> GPU acceleration &amp; SIMD vectorization</li>
          </ul>
        </div>
      </div>
    </section>
  );
}
