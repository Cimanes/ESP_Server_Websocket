// ============================================================================
// Fetch JSON data from URL and fix the response format
// ============================================================================
function fetchAndFixJSON(url) {
  return fetch(url)
    .then(response => response.text())
    .then(data => {       // Fix the response format (add "[]" and remove trailing comma)
      return JSON.parse("[" + data.slice(0, -1) + "]");
    });
}

// ============================================================================
// Unit conversion function
// ============================================================================
function unitConvert(jsonArray) {
  const len = jsonArray.length;
  const convertedBME = [];

  for (let i = 0; i < len; i++) {
    const time = jsonArray[i].time                // seconds
          t = (jsonArray[i].t / 10).toFixed(1),   // Convert temperature to decimal places
          rh = (jsonArray[i].rh / 10).toFixed(1), // Convert humidity to decimal places
          p = (jsonArray[i].p / 10).toFixed(1);   // Convert pressure to decimal places

          convertedBME.push([time, t, rh, p]);
  }
  return convertedBME;
}

// ============================================================================
// Convert to CSV function
// ============================================================================
function convertToCSV(jsonArray) {
  if (!Array.isArray(jsonArray) || jsonArray.length === 0) {
    console.error("Invalid or empty JSON data");
    return null;
  }
  const data = unitConvert(jsonArray);        // Process the data with unitConvert
  const headers = Object.keys(jsonArray[0]);  // Extract headers from the first object
  const csvRows = [headers.join(',')];        // Add headers to the CSV rows
  
  // Loop through the processed data and add rows to the CSV
  data.forEach(row => { csvRows.push(row.join(',')); });
  return csvRows.join('\n');        // Join all rows with new lines
}

// ============================================================================
// Trigger CSV download
// ============================================================================
function downloadCSV(csvData, filename = 'download.csv') {
  const blob = new Blob([csvData], { type: 'text/csv' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);  // Append to body (required for some browsers)
  a.click();  // Trigger download
  document.body.removeChild(a);  // Clean up
  URL.revokeObjectURL(url);  // Free up memory
}

// ============================================================================
// Export data-file to formatted CSV (including unit conversion)
// ============================================================================
function retrieveCSV() {
  fetchAndFixJSON('/data-file')
    .then(jsonArray => convertToCSV(jsonArray))
    .then(csvData => downloadCSV(csvData))
    .catch(error => console.error("Error retrieving CSV:", error));
}