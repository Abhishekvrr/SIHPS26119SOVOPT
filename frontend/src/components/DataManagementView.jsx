import React, { useState, useMemo } from 'react';
import { Database, Sparkles, UploadCloud, CheckCircle2, Layers, DollarSign, Gauge, ArrowRight, Table, Search } from 'lucide-react';
import DatasetUploadView from './DatasetUploadView';
import DataProfileView from './DataProfileView';
import DataTablePreview from './DataTablePreview';
import { parseCSVToRecords } from '../utils/csvParser';

export default function DataManagementView({
  datasets,
  selectedDataset,
  onSelectDataset,
  onUploadCustomData,
  onAnalyzeDataset,
  dataAnalysis,
  classification,
  isAnalyzing,
  onProceedToHybrid
}) {
  const [activeSubTab, setActiveSubTab] = useState('upload'); // 'upload' | 'profile' | 'raw_table'

  const displayRecords = useMemo(() => {
    if (selectedDataset?.records && selectedDataset.records.length > 0) {
      return selectedDataset.records;
    }
    if (selectedDataset?.raw_csv) {
      return parseCSVToRecords(selectedDataset.raw_csv);
    }
    return [];
  }, [selectedDataset]);

  return (
    <div className="data-management-root">
      {/* Header Banner */}
      <div className="panel-card flex-between">
        <div>
          <div className="badge-row">
            <span className="category-tag">Data Management & Profiling</span>
            <span className="sense-tag">Enterprise Ingestion</span>
          </div>
          <h2 className="problem-main-title">Company Data Assets & Ingestion Hub</h2>
          <p className="problem-desc">
            Upload organizational CSV spreadsheets, select pre-loaded industrial scenarios, and view automated 0–100% data telemetry profiles.
          </p>
        </div>

        <div className="sub-tab-pill-group">
          <button
            className={`sub-tab-pill ${activeSubTab === 'upload' ? 'active' : ''}`}
            onClick={() => setActiveSubTab('upload')}
          >
            <UploadCloud size={14} />
            <span>Ingestion & Presets</span>
          </button>
          <button
            className={`sub-tab-pill ${activeSubTab === 'profile' ? 'active' : ''}`}
            onClick={() => setActiveSubTab('profile')}
            disabled={!dataAnalysis}
          >
            <Sparkles size={14} />
            <span>Data Profile & Quality {dataAnalysis ? '(Ready)' : ''}</span>
          </button>
          <button
            className={`sub-tab-pill ${activeSubTab === 'raw_table' ? 'active' : ''}`}
            onClick={() => setActiveSubTab('raw_table')}
            disabled={displayRecords.length === 0}
          >
            <Table size={14} />
            <span>Operational Data Grid ({displayRecords.length})</span>
          </button>
        </div>
      </div>

      {/* Sub-Tab 1: Upload & Presets */}
      {activeSubTab === 'upload' && (
        <div className="mt-6">
          <DatasetUploadView
            datasets={datasets}
            selectedDataset={selectedDataset}
            onSelectDataset={(ds) => {
              onSelectDataset(ds);
              onAnalyzeDataset(ds);
            }}
            onUploadCustomData={onUploadCustomData}
            onProceedToAnalysis={() => {
              onAnalyzeDataset(selectedDataset);
              setActiveSubTab('profile');
            }}
            isAnalyzing={isAnalyzing}
          />
        </div>
      )}

      {/* Sub-Tab 2: Preserved Data Profile & Quality View (Step 2/7) */}
      {activeSubTab === 'profile' && dataAnalysis && (
        <div className="mt-6">
          <DataProfileView
            analysis={dataAnalysis}
            classification={classification}
            onProceedToSuggestions={onProceedToHybrid}
            onBackToUpload={() => setActiveSubTab('upload')}
          />
        </div>
      )}

      {/* Sub-Tab 3: Raw Tabular Data Grid */}
      {activeSubTab === 'raw_table' && displayRecords.length > 0 && (
        <div className="panel-card mt-6">
          <div className="panel-header flex-between">
            <div className="panel-title-wrap">
              <Table size={17} className="text-cyan" />
              <div>
                <h3 style={{ fontSize: '1.05rem', fontWeight: 700 }}>{selectedDataset?.name || "Dataset"} — Raw Records</h3>
                <span style={{ fontSize: '0.8rem', color: '#94a3b8' }}>
                  {displayRecords.length} operational rows loaded into memory
                </span>
              </div>
            </div>

            <button className="action-btn-primary" onClick={() => setActiveSubTab('profile')}>
              <span>View Quality Telemetry</span>
              <ArrowRight size={14} />
            </button>
          </div>

          <div className="mt-4">
            <DataTablePreview records={displayRecords} title={selectedDataset?.name || "Dataset"} />
          </div>
        </div>
      )}
    </div>
  );
}


