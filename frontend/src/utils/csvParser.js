/**
 * SOVOPT CSV Ingestion Parser
 * Converts raw CSV string into an array of typed row objects.
 * Handles quoted fields with embedded commas, auto-detects numbers, and cleans whitespace.
 */

export function parseCSVToRecords(csvText) {
  if (!csvText || typeof csvText !== 'string') return [];
  const lines = csvText.trim().split(/\r?\n/).filter(line => line.trim().length > 0);
  if (lines.length === 0) return [];

  // Robust line tokenizer handling quoted strings and commas
  const splitRow = (rowStr) => {
    const values = [];
    let current = '';
    let inQuotes = false;
    for (let i = 0; i < rowStr.length; i++) {
      const char = rowStr[i];
      if (char === '"' || char === "'") {
        inQuotes = !inQuotes;
      } else if ((char === ',' || char === '\t') && !inQuotes) {
        values.push(current.trim().replace(/^["']|["']$/g, ''));
        current = '';
      } else {
        current += char;
      }
    }
    values.push(current.trim().replace(/^["']|["']$/g, ''));
    return values;
  };

  const headers = splitRow(lines[0]);
  if (headers.length === 0) return [];

  const records = [];

  for (let i = 1; i < lines.length; i++) {
    const rowValues = splitRow(lines[i]);
    if (rowValues.length === 0 || (rowValues.length === 1 && rowValues[0] === '')) continue;
    const rowObj = {};
    headers.forEach((header, colIdx) => {
      const key = header || `Column_${colIdx + 1}`;
      const rawVal = rowValues[colIdx] !== undefined ? rowValues[colIdx] : '';
      if (rawVal === '' || rawVal.toLowerCase() === 'null' || rawVal.toLowerCase() === 'nan') {
        rowObj[key] = null;
      } else if (rawVal.toLowerCase() === 'true') {
        rowObj[key] = true;
      } else if (rawVal.toLowerCase() === 'false') {
        rowObj[key] = false;
      } else {
        const numVal = Number(rawVal);
        rowObj[key] = (!isNaN(numVal) && rawVal.trim() !== '') ? numVal : rawVal;
      }
    });
    records.push(rowObj);
  }

  return records;
}

