import React, { useState, useEffect } from 'react';
import IntroAnimation from './components/landing/IntroAnimation';
import LandingPage from './components/landing/LandingPage';
import Navbar from './components/Navbar';
import Sidebar from './components/Sidebar';
import WelcomeDashboardView from './components/WelcomeDashboardView';
import DataManagementView from './components/DataManagementView';
import HybridIntelligenceView from './components/HybridIntelligenceView';
import ModelBuilderView from './components/ModelBuilderView';
import OptimizationWorkspaceView from './components/OptimizationWorkspaceView';
import VisualAnalyticsView from './components/VisualAnalyticsView';
import SignatureWaveView from './components/SignatureWaveView';
import DatasetHistoryView from './components/DatasetHistoryView';
import ArchitectureView from './components/ArchitectureView';

import {
  checkHealth,
  fetchDemoDatasets,
  suggestModel,
  solveOptimizationProblem
} from './services/api';
import { parseCSVToRecords } from './utils/csvParser';
import './App.css';

const DEFAULT_MANUAL_MODEL = {
  id: "production_planning",
  name: "Refinery Multi-Product Production Planning",
  category: "Industrial / Petroleum & Chemical",
  description: "Maximize refinery product margins under crude, energy, labor, and water capacity limits.",
  objective_sense: "maximize",
  variables: [
    { name: "LPG Production (TPD)", type: "continuous", lower_bound: 0.0, upper_bound: 850.0, objective: 12500.0 },
    { name: "Naphtha Production (TPD)", type: "continuous", lower_bound: 0.0, upper_bound: 1200.0, objective: 8200.0 },
    { name: "Motor Spirit (MS/Petrol) Production (TPD)", type: "continuous", lower_bound: 0.0, upper_bound: 2500.0, objective: 14800.0 },
    { name: "High-Speed Diesel (HSD) Production (TPD)", type: "continuous", lower_bound: 0.0, upper_bound: 3800.0, objective: 11200.0 },
    { name: "Aviation Turbine Fuel (ATF) Production (TPD)", type: "continuous", lower_bound: 0.0, upper_bound: 950.0, objective: 13600.0 },
    { name: "Polypropylene Production (TPD)", type: "continuous", lower_bound: 0.0, upper_bound: 400.0, objective: 16500.0 }
  ],
  constraints: [
    { name: "Crude Feedstock Daily Capacity Limit (Tons)", sense: "<=", rhs: 8000.0, coefficients: [1.02, 1.01, 1.05, 1.03, 1.04, 1.08] },
    { name: "Total Energy Consumption Quota (MWh/day)", sense: "<=", rhs: 2507.5, coefficients: [0.35, 0.28, 0.42, 0.31, 0.38, 0.55] },
    { name: "Operational Labor & Supervision Limit (Hours/day)", sense: "<=", rhs: 977.5, coefficients: [0.12, 0.09, 0.15, 0.11, 0.14, 0.22] },
    { name: "Cooling & Process Water Allocation (m3/day)", sense: "<=", rhs: 6630.0, coefficients: [0.85, 0.72, 0.95, 0.80, 0.90, 1.40] }
  ]
};

export default function App() {
  const [viewMode, setViewMode] = useState('intro'); // 'intro' | 'landing' | 'workspace'
  const [activeTab, setActiveTab] = useState('dashboard');

  const [health, setHealth] = useState(null);
  const [datasets, setDatasets] = useState([]);
  const [selectedDataset, setSelectedDataset] = useState(null);

  const [dataAnalysis, setDataAnalysis] = useState(null);
  const [classification, setClassification] = useState(null);
  const [suggestedModel, setSuggestedModel] = useState(null);

  const [activeModel, setActiveModel] = useState(DEFAULT_MANUAL_MODEL);
  const [solverResult, setSolverResult] = useState(null);
  const [isSolving, setIsSolving] = useState(false);
  const [isAnalyzing, setIsAnalyzing] = useState(false);
  const [error, setError] = useState(null);

  const [options, setOptions] = useState({
    algorithm: "simplex",
    acceleration: "cpu_simd",
    enable_presolve: true,
    enable_gomory_cuts: true,
    max_iterations: 10000,
    feasibility_tolerance: 1e-7,
    optimality_tolerance: 1e-7,
    enable_validation: true
  });

  // Initial load & health polling
  useEffect(() => {
    async function init() {
      const h = await checkHealth();
      setHealth(h);

      try {
        const dsRes = await fetchDemoDatasets();
        if (dsRes.datasets && dsRes.datasets.length > 0) {
          setDatasets(dsRes.datasets);
          const firstDs = dsRes.datasets[0];
          setSelectedDataset(firstDs);
          if (firstDs.suggested_model) {
            setActiveModel(firstDs.suggested_model);
            setDataAnalysis(firstDs.analysis);
            setClassification(firstDs.classification);
            setSuggestedModel(firstDs.suggested_model);
            executeSolve(firstDs.suggested_model);
          }
        }
      } catch (err) {
        console.error("Failed to load demo datasets:", err);
        executeSolve(DEFAULT_MANUAL_MODEL);
      }
    }
    init();

    const interval = setInterval(async () => {
      const h = await checkHealth();
      setHealth(h);
    }, 5000);

    return () => clearInterval(interval);
  }, []);

  const handleAnalyzeDataset = async (datasetToAnalyze = selectedDataset) => {
    if (!datasetToAnalyze) return;
    setIsAnalyzing(true);
    setError(null);
    try {
      const payload = {
        dataset_name: datasetToAnalyze.name,
        records: datasetToAnalyze.records
      };
      const res = await suggestModel(payload);
      setDataAnalysis(res.analysis);
      setClassification(res.classification);
      setSuggestedModel(res.suggested_model);
      setActiveModel(res.suggested_model);
    } catch (err) {
      setError(err.message);
    } finally {
      setIsAnalyzing(false);
    }
  };

  const executeSolve = async (modelToSolve = activeModel, optionsOverride = null) => {
    if (!modelToSolve || !modelToSolve.variables || modelToSolve.variables.length === 0) {
      setError("Cannot solve empty model. Please ensure decision variables are configured.");
      return;
    }
    setIsSolving(true);
    setError(null);
    try {
      const opts = optionsOverride || options;
      const res = await solveOptimizationProblem(modelToSolve, opts);
      setSolverResult(res);
    } catch (err) {
      setError(err.message || "Solver error encountered during execution.");
    } finally {
      setIsSolving(false);
    }
  };

  const handleSelectScenario = (datasetId) => {
    const ds = datasets.find(d => d.id === datasetId);
    if (ds) {
      setSelectedDataset(ds);
      if (ds.suggested_model) {
        setActiveModel(ds.suggested_model);
        setDataAnalysis(ds.analysis);
        setClassification(ds.classification);
        setSuggestedModel(ds.suggested_model);
        executeSolve(ds.suggested_model);
      }
    }
  };

  const handleUploadCustomData = async (arg1, arg2) => {
    setIsAnalyzing(true);
    setError(null);
    try {
      let fileName = "Custom Uploaded Dataset";
      let rawCsv = "";

      if (typeof arg1 === 'string' && (arg1.includes('\n') || arg1.includes(',') || arg1.length > 80)) {
        rawCsv = arg1;
        fileName = (typeof arg2 === 'string' && arg2.trim()) ? arg2.trim() : "Custom Uploaded Dataset";
      } else if (typeof arg2 === 'string' && (arg2.includes('\n') || arg2.includes(',') || arg2.length > 80)) {
        rawCsv = arg2;
        fileName = (typeof arg1 === 'string' && arg1.trim()) ? arg1.trim() : "Custom Uploaded Dataset";
      } else {
        fileName = (typeof arg1 === 'string' && arg1.trim()) ? arg1.trim() : "Custom Uploaded Dataset";
        rawCsv = (typeof arg2 === 'string') ? arg2 : "";
      }

      // Parse CSV into structured record objects
      const parsedRecords = parseCSVToRecords(rawCsv);

      const payload = {
        dataset_name: fileName,
        csv_text: rawCsv,
        records: parsedRecords
      };

      const res = await suggestModel(payload);
      const finalRecords = (res.records && res.records.length > 0) ? res.records : parsedRecords;

      const rowCount = finalRecords.length || res.analysis?.row_count || 0;
      const colCount = Object.keys(finalRecords[0] || {}).length || res.analysis?.column_count || 0;

      const customDs = {
        id: `custom_${Date.now()}`,
        name: fileName,
        industry: res.classification?.category || "Custom Industry",
        category: res.classification?.category || "Custom Problem Domain",
        description: `Imported tabular dataset (${rowCount} rows × ${colCount} columns).`,
        icon: "Database",
        raw_csv: rawCsv,
        records: finalRecords,
        analysis: res.analysis,
        classification: res.classification,
        suggested_model: res.suggested_model,
        strategy: res.strategy
      };

      setDatasets(prev => [customDs, ...prev]);
      setSelectedDataset(customDs);
      setDataAnalysis(res.analysis);
      setClassification(res.classification);
      setSuggestedModel(res.suggested_model);
      setActiveModel(res.suggested_model);

      if (res.suggested_model?.variables?.length > 0) {
        executeSolve(res.suggested_model);
      }
    } catch (err) {
      setError(err.message || "Failed to parse and profile custom dataset.");
    } finally {
      setIsAnalyzing(false);
    }
  };

  return (
    <div className="sov-app-root">
      {viewMode === 'intro' && (
        <IntroAnimation
          onFinish={() => setViewMode('landing')}
          onComplete={() => setViewMode('landing')}
          onSkip={() => setViewMode('workspace')}
        />
      )}

      {viewMode === 'landing' && (
        <LandingPage
          onLaunchWorkspace={() => setViewMode('workspace')}
          health={health}
        />
      )}

      {viewMode === 'workspace' && (
        <>
          <Navbar
            health={health}
            currentModel={activeModel}
            selectedDatasetId={selectedDataset?.id}
            demos={datasets}
            onSelectDemo={handleSelectScenario}
            onSolve={() => executeSolve(activeModel)}
            isSolving={isSolving}
            onBackToLanding={() => setViewMode('landing')}
          />

          <div className="sov-main-layout">
            <Sidebar
              activeTab={activeTab}
              setActiveTab={(tab) => setActiveTab(tab)}
            />

            <main className="sov-content-area">
              {error && (
                <div className="alert-box error-alert mb-4">
                  <span><strong>Operational Notice:</strong> {error}</span>
                </div>
              )}

              {/* 1. Dashboard */}
              {activeTab === 'dashboard' && (
                <WelcomeDashboardView
                  health={health}
                  currentModel={activeModel}
                  solverResult={solverResult}
                  selectedDataset={selectedDataset}
                  onNavigate={(tab) => setActiveTab(tab)}
                  onQuickSolve={() => executeSolve(activeModel)}
                  isSolving={isSolving}
                />
              )}

              {/* 2. Data Management */}
              {activeTab === 'data' && (
                <DataManagementView
                  datasets={datasets}
                  selectedDataset={selectedDataset}
                  onSelectDataset={(ds) => {
                    setSelectedDataset(ds);
                    if (ds.suggested_model) {
                      setActiveModel(ds.suggested_model);
                      setDataAnalysis(ds.analysis);
                      setClassification(ds.classification);
                      setSuggestedModel(ds.suggested_model);
                    }
                  }}
                  onUploadCustomData={handleUploadCustomData}
                  onAnalyzeDataset={handleAnalyzeDataset}
                  dataAnalysis={dataAnalysis}
                  classification={classification}
                  isAnalyzing={isAnalyzing}
                  onProceedToHybrid={() => setActiveTab('hybrid')}
                />
              )}

              {/* 3. Data Intelligence */}
              {activeTab === 'hybrid' && (
                <HybridIntelligenceView
                  dataAnalysis={dataAnalysis}
                  classification={classification}
                  suggestedModel={suggestedModel || activeModel}
                  onProceedToBuilder={() => setActiveTab('builder')}
                  onProceedToOptimize={() => {
                    setActiveTab('optimizer');
                    executeSolve(activeModel);
                  }}
                  onBackToData={() => setActiveTab('data')}
                />
              )}

              {/* 4. Model Builder */}
              {activeTab === 'builder' && (
                <ModelBuilderView
                  model={activeModel}
                  setModel={(m) => setActiveModel(m)}
                  onSolve={() => executeSolve(activeModel)}
                  isSolving={isSolving}
                  result={solverResult}
                  onViewFullDashboard={() => setActiveTab('optimizer')}
                />
              )}

              {/* 5. Optimization Workspace */}
              {activeTab === 'optimizer' && (
                <OptimizationWorkspaceView
                  datasets={datasets}
                  selectedDatasetId={selectedDataset?.id}
                  onSelectDataset={handleSelectScenario}
                  dataAnalysis={dataAnalysis}
                  model={activeModel}
                  result={solverResult}
                  solverOptions={options}
                  setSolverOptions={setOptions}
                  onSolve={(opts) => executeSolve(activeModel, opts)}
                  isSolving={isSolving}
                  onNavigateToAnalytics={() => setActiveTab('analytics')}
                />
              )}

              {/* 6. Results & Analytics */}
              {activeTab === 'analytics' && (
                <VisualAnalyticsView
                  model={activeModel}
                  result={solverResult}
                  onNavigateToWorkspace={() => setActiveTab('optimizer')}
                />
              )}

              {/* 7. Computational Wave */}
              {activeTab === 'wave' && (
                <SignatureWaveView
                  result={solverResult}
                  isSolving={isSolving}
                  onRunOptimize={() => executeSolve(activeModel)}
                />
              )}

              {/* 8. Dataset History (SQLite) */}
              {activeTab === 'history' && (
                <DatasetHistoryView
                  onSelectDataset={(dsId) => {
                    handleSelectScenario(dsId);
                    setActiveTab('optimizer');
                  }}
                  onRunOptimize={() => executeSolve(activeModel)}
                />
              )}

              {/* 9. Solver / System Status */}
              {activeTab === 'architecture' && (
                <ArchitectureView />
              )}
            </main>
          </div>
        </>
      )}
    </div>
  );
}