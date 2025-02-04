// ============================================================================
// Fetch JSON data from URL and fix the response format
// ============================================================================
/** Fetches data from the given URL and fixes the JSON format.
 * The function sends a GET request to the specified URL, retrieves the response as text,
 * and then parses it into a JSON array after fixing the format by adding square brackets
 * and removing the trailing comma.
 * @param {string} url - The URL to fetch data from.
 * @returns {Promise<Object[]>} A promise that resolves to an array of JSON objects.
 */
function fetchAndFixJSON(url) {
  return fetch(url)                 // { method: "GET" } is default
    .then(response => response.text())
    .then(data => {       // Fix the response format (add "[]" and remove trailing comma)
      return JSON.parse("[" + data.slice(0, -1) + "]");
    });
}

// ============================================================================
//  Button "delete-data" --> trigger function fileDELETE(url)
// ============================================================================
/**
 * Delete a file from the server at the specified URL.
 * Prompt the user for confirmation.
 * Send a "DELETE" request with the given URL --> ESP will delete the file
 * When confirmation is received, send feedback message to user.
 * @param {string} url - The URL of the file to be deleted.
 * @returns {void}
 */
function fileDELETE(url) {
  if (confirm("Confirm to delete data / Cancel to abort")) {  // Popup confirm/cancel
    fetch(url, { method: "DELETE" })
      .then(response => {
        if (response.ok) { alert("Data deleted"); }     // Feedback deleted OK
        else { throw new Error(response.statusText); }  // Throw error for non-OK response
      })
      .catch(error => console.error("Error deleting data:", error));
  }
}

// ============================================================================
// BME data - Unit conversion function
// ============================================================================
/**
 * Convert an array of sensor data objects to an array of arrays with converted values.
 * @param {Array<Object>} jsonArray - An array of objects containing sensor data.
 * @param {number} jsonArray[].time - The timestamp in seconds.
 * @param {number} jsonArray[].t - The temperature value multiplied by 10.
 * @param {number} jsonArray[].rh - The humidity value multiplied by 10.
 * @param {number} jsonArray[].p - The pressure value multiplied by 10.
 * @returns {Array<Array<number|string>>} An array of arrays, each containing the converted time, temperature, humidity, and pressure values.
 */
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
/**
 * Convert an array of JSON objects to a CSV string.
 * @param {Object[]} jsonArray - The array of JSON objects to convert.
 * @returns {string|null} The CSV string representation of the JSON array, or null if the input is invalid or empty.
 */
function convertToCSV(jsonArray) {
  if (!Array.isArray(jsonArray) || jsonArray.length === 0) {
    console.error("Invalid or empty JSON data");
    return null;
  }
  const data = unitConvert(jsonArray, 1);     // Process the data array with unitConvert (1 decimal)
  const headers = Object.keys(jsonArray[0]);  // Extract headers from the first object
  const csvRows = [headers.join(",")];        // Add headers to the CSV rows
  
  // Loop through the processed data and add rows to the CSV
  data.forEach(row => { csvRows.push(row.join(",")); });
  return csvRows.join("\n");        // Join all rows with new lines
}

// ============================================================================
// Trigger CSV download
// ============================================================================
/**
 * Downloads a file with the specified data, filename, and MIME type.
 * @param {string} data - The data to be included in the file.
 * @param {string} [filename="download.csv"] - The name of the file to be downloaded.
 * @param {string} [mimeType="text/csv"] - The MIME type of the file.
 */
function downloadFile(data, filename = "download.csv", mimeType = "text/csv") {
  const blob = new Blob([data], { type: mimeType });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = filename;
  a.click();                                              // Trigger download
  setTimeout(() => { URL.revokeObjectURL(url); }, 100);   // Free up memory
}

// ============================================================================
// Export data-file to formatted CSV (including unit conversion)
// ============================================================================
function downloadCSV(numDecimals = 1) {
  fetchAndFixJSON("/data-file")
    .then(jsonArray => convertToCSV(jsonArray, numDecimals))
    .then(csvData => downloadFile(csvData,"BMEdata.csv"))
    .catch(error => console.error("Error retrieving CSV:", error));
}