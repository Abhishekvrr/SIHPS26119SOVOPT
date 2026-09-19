import React, { useState, useMemo } from 'react';
import { UploadCloud, FileText, Database, ArrowRight, CheckCircle2 } from 'lucide-react';
import DataTablePreview from './DataTablePreview';
import { parseCSVToRecords } from '../utils/csvParser';

export default function DatasetUploadView({
  datasets,
  selectedDataset,
  onSelectDataset,
  onUploadCustomData,
  onProceedToAnalysis,
  isAnalyzing
}) {
  const [csvInput, setCsvInput] = useState('');
  const [customName, setCustomName] = useState('Custom Company Data');

  const handleFileUpload = (e) => {
    const file = e.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (event) => {
      const text = event.target?.result;
      if (typeof text === 'string') {
        onUploadCustomData(file.name.replace(/\.[^/.]+$/, ""), text);
      }
    };
    reader.readAsText(file);
  };

  const handleApplyCustomCsv = () => {
    if (!csvInput.trim()) return;
    onUploadCustomData(customName, csvInput);
  };

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
    <div className="view-container">
      {/* Header */}
      <div className="panel-card flex-between">
        <div>
          <div className="badge-row">
            <span className="category-tag">Step 1 of 7</span>
            <span className="sense-tag">Dataset Ingestion</span>
          </div>
          <h2 className="problem-main-title">Select or Ingest Company Dataset</h2>
          <p className="problem-desc">
            Provide operational business data in CSV or tabular format. SOVOPT's intelligence layer will automatically profile columns, assess data quality, and construct mathematical model suggestions.
          </p>
        </div>

        <button
          className="solve-btn"
          onClick={onProceedToAnalysis}
          disabled={!selectedDataset || isAnalyzing}
        >
          <span>{isAnalyzing ? 'Analyzing Data...' : 'Analyze & Profile Data'}</span>
          <ArrowRight size={15} />
        </button>
      </div>

      {/* Dataset Selection Cards */}
      <div className="panel-card mt-6">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <Database size={17} className="text-blue" />
            <h3>Pre-loaded Multi-Industry Datasets (SIH Reference & Enterprise)</h3>
          </div>
          <span className="panel-badge">{datasets.length} Ready-to-Use Datasets</span>
        </div>

        <div className="datasets-grid">
          {datasets.map((d) => {
            const isSelected = selectedDataset?.id === d.id;
            return (
              <div
                key={d.id}
                className={`dataset-card ${isSelected ? 'selected' : ''}`}
                onClick={() => onSelectDataset(d)}
              >
                <div className="dataset-card-top">
                  <span className="category-tag">{d.industry}</span>
                  {isSelected && <CheckCircle2 size={16} className="text-emerald" />}
                </div>
                <h4 className="dataset-title">{d.name}</h4>
                <p className="dataset-desc">{d.description}</p>
                <div className="dataset-meta-row">
                  <span>{d.records?.length || 0} Operational Rows</span>
                  <span>{Object.keys(d.records?.[0] || {}).length} Attributes</span>
                </div>
              </div>
            );
          })}
        </div>
      </div>

      {/* Custom CSV Upload & Paste */}
      <div className="panel-card mt-6">
        <div className="panel-header">
          <div className="panel-title-wrap">
            <UploadCloud size={17} className="text-purple" />
            <h3>Upload Custom Company Dataset (.CSV)</h3>
          </div>
        </div>

        <div className="upload-section-grid">
          <div className="file-drop-area">
            <input
              type="file"
              accept=".csv,.txt"
              id="csv-file-input"
              className="file-input-hidden"
              onChange={handleFileUpload}
            />
            <label htmlFor="csv-file-input" className="file-drop-label">
              <UploadCloud size={32} className="text-blue" />
              <span className="file-drop-text">Click to choose CSV file or drag & drop</span>
              <span className="file-drop-sub">Supports comma-separated values (.csv) with column headers</span>
            </label>
          </div>

          <div className="paste-csv-area">
            <div className="paste-header">
              <label>Or Paste Raw CSV Data:</label>
              <input
                type="text"
                placeholder="Dataset Name"
                value={customName}
                onChange={(e) => setCustomName(e.target.value)}
                className="custom-name-input"
              />
            </div>
            <textarea
              className="csv-textarea font-mono"
              rows={4}
              placeholder="Stream,Octane Rating,Sulfur Pct,Unit Margin ($/BBL),Max Supply (kBPD)&#10;Arabian Light,92.0,1.77,14.50,60.0&#10;Bonny Light,95.0,0.14,18.20,45.0"
              value={csvInput}
              onChange={(e) => setCsvInput(e.target.value)}
            />
            <button
              className="apply-csv-btn"
              onClick={handleApplyCustomCsv}
              disabled={!csvInput.trim()}
            >
              <FileText size={14} />
              <span>Ingest Raw CSV</span>
            </button>
          </div>
        </div>
      </div>

      {/* Dataset Preview Table with Enterprise Grid */}
      {selectedDataset && displayRecords.length > 0 && (
        <div className="panel-card mt-6">
          <div className="panel-header">
            <div className="panel-title-wrap">
              <FileText size={17} className="text-emerald" />
              <h3>Dataset Ingestion Preview: {selectedDataset.name}</h3>
            </div>
            <span className="panel-badge">
              {displayRecords.length} Operational Records Loaded
            </span>
          </div>

          <div className="mt-4">
            <DataTablePreview records={displayRecords} title={selectedDataset.name} />
          </div>
        </div>
      )}
    </div>
  );
}

