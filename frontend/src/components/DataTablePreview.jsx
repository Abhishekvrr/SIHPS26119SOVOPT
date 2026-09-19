import React, { useState, useMemo } from 'react';
import { Search, ChevronLeft, ChevronRight, Database, Download, Filter, Layers } from 'lucide-react';

export default function DataTablePreview({
  records = [],
  title = "Dataset Ingestion Preview",
  maxPreviewRows = 1000
}) {
  const [searchQuery, setSearchQuery] = useState("");
  const [currentPage, setCurrentPage] = useState(1);
  const [pageSize, setPageSize] = useState(10);

  // Extract all columns
  const columns = useMemo(() => {
    if (!records || records.length === 0) return [];
    return Object.keys(records[0]);
  }, [records]);

  // Filtered records by search query
  const filteredRecords = useMemo(() => {
    if (!searchQuery.trim()) return records;
    const q = searchQuery.toLowerCase().trim();
    return records.filter((row) =>
      Object.values(row).some((val) =>
        String(val).toLowerCase().includes(q)
      )
    );
  }, [records, searchQuery]);

  // Paginated records
  const totalPages = Math.max(1, Math.ceil(filteredRecords.length / pageSize));
  const displayedRecords = useMemo(() => {
    const start = (currentPage - 1) * pageSize;
    return filteredRecords.slice(start, start + pageSize);
  }, [filteredRecords, currentPage, pageSize]);

  if (!records || records.length === 0) {
    return (
      <div className="empty-table-box">
        <Database size={24} className="text-slate-500 mb-2" />
        <p className="text-slate-400 text-sm">No dataset records loaded to display.</p>
      </div>
    );
  }

  return (
    <div className="data-table-container">
      {/* Table Control Toolbar */}
      <div className="table-toolbar flex-between">
        <div className="table-toolbar-left">
          <div className="table-search-box">
            <Search size={14} className="search-icon text-slate-400" />
            <input
              type="text"
              placeholder="Search in records..."
              value={searchQuery}
              onChange={(e) => {
                setSearchQuery(e.target.value);
                setCurrentPage(1);
              }}
              className="table-search-input"
            />
          </div>

          <div className="table-meta-pills">
            <span className="meta-pill text-cyan">
              <Layers size={12} />
              <span>{records.length.toLocaleString()} Rows &times; {columns.length} Columns</span>
            </span>
            {searchQuery && (
              <span className="meta-pill text-amber">
                Filtered: {filteredRecords.length} Rows
              </span>
            )}
          </div>
        </div>

        {/* Pagination & Page Size Control */}
        <div className="table-toolbar-right">
          <div className="page-size-selector">
            <span className="text-xs text-slate-400">Rows:</span>
            <select
              value={pageSize}
              onChange={(e) => {
                setPageSize(Number(e.target.value));
                setCurrentPage(1);
              }}
              className="page-size-select"
            >
              <option value={10}>10</option>
              <option value={25}>25</option>
              <option value={50}>50</option>
              <option value={100}>100</option>
            </select>
          </div>

          <div className="table-pagination-nav">
            <button
              className="page-nav-btn"
              disabled={currentPage <= 1}
              onClick={() => setCurrentPage((p) => Math.max(1, p - 1))}
            >
              <ChevronLeft size={14} />
            </button>
            <span className="page-indicator">
              Page {currentPage} of {totalPages}
            </span>
            <button
              className="page-nav-btn"
              disabled={currentPage >= totalPages}
              onClick={() => setCurrentPage((p) => Math.min(totalPages, p + 1))}
            >
              <ChevronRight size={14} />
            </button>
          </div>
        </div>
      </div>

      {/* Structured Scrollable Data Table */}
      <div className="table-scroll-viewport">
        <table className="sov-enterprise-table">
          <thead>
            <tr>
              <th className="row-num-th">#</th>
              {columns.map((col, idx) => (
                <th key={idx} className="data-th" title={col}>
                  <div className="th-content font-mono">
                    <span>{col}</span>
                  </div>
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {displayedRecords.map((row, rIdx) => {
              const globalIdx = (currentPage - 1) * pageSize + rIdx + 1;
              return (
                <tr key={rIdx} className="data-row">
                  <td className="row-num-td font-mono text-slate-400">{globalIdx}</td>
                  {columns.map((col, cIdx) => {
                    const val = row[col];
                    const isNum = typeof val === 'number';
                    const displayStr = val !== null && val !== undefined ? (isNum ? val.toLocaleString() : String(val)) : "-";

                    return (
                      <td
                        key={cIdx}
                        className={`data-td ${isNum ? 'td-numeric font-mono' : 'td-text'}`}
                        title={String(val ?? '')}
                      >
                        <span className="td-ellipsis">{displayStr}</span>
                      </td>
                    );
                  })}
                </tr>
              );
            })}
          </tbody>
        </table>
      </div>

      {/* Table Footer Status */}
      <div className="table-footer-status flex-between">
        <span className="text-xs text-slate-400">
          Showing rows {((currentPage - 1) * pageSize) + 1}–{Math.min(currentPage * pageSize, filteredRecords.length)} of {filteredRecords.length.toLocaleString()} total rows
        </span>
        <span className="text-xs text-emerald font-semibold">
          ✓ High-Throughput Memory Ingestion Active
        </span>
      </div>
    </div>
  );
}

