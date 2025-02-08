// Configuration: Retrieve color variables from defined in CSS:
const tColor = getComputedStyle(document.documentElement).getPropertyValue("--tColor"),
      rhColor = getComputedStyle(document.documentElement).getPropertyValue("--rhColor"),
      pColor = getComputedStyle(document.documentElement).getPropertyValue("--pColor"),
      timeColor = getComputedStyle(document.documentElement).getPropertyValue("--timeColor"),
      bgColor = getComputedStyle(document.documentElement).getPropertyValue("--bgColor"),
      gridColor = getComputedStyle(document.documentElement).getPropertyValue("--gridColor"),
      gridWidth = 0.5;

 var   numPoints = 60;   // Max number of points displayed in charts (limit memory usage)

// ===============================================================================
// Update number of points displayed in charts
// ===============================================================================
document.addEventListener('DOMContentLoaded', () => {
    const form = document.getElementById('numPointsForm');
    form.addEventListener('submit', (event) => {
        event.preventDefault(); // Prevent the default form submission behavior
        numPoints = document.getElementById('nPoints').value;
        console.log(`Number of Points set to: ${numPoints}`);
        // You can now use the numPoints variable as needed
    });
});

// ===============================================================================
// Update the displayed / hidden charts based on the selected checkboxes
// ===============================================================================
document.addEventListener('DOMContentLoaded', () => {
  const items = [
    { checkbox: document.getElementById('checkCards'), element: document.getElementById('BMECards') },
    { checkbox: document.getElementById('checkTable'), element: document.getElementById('BMETable') },
    { checkbox: document.getElementById('checkHighcharts'), element: document.getElementById('highchartsCard') },
    { checkbox: document.getElementById('checkChartsJS'), element: document.getElementById('chartsJSCard') },
    { checkbox: document.getElementById('checkPlotly'), element: document.getElementById('plotlyCard') }
  ];

  items.forEach(item => {
    item.element.classList.toggle('hidden', !item.checkbox.checked);
    item.checkbox.addEventListener('change', () => {
        item.element.classList.toggle('hidden', !item.checkbox.checked);
    });
  });
});

// ===============================================================================
// OPTION: HIGHCHARTS --> Create single chart with all values
// ===============================================================================
// Create global Chart
const highChart = new Highcharts.Chart({
  time:{ useUTC: false },
  chart:{ 
    backgroundColor: bgColor,
    height: (9 / 16 * 100) + "%",
    renderTo:"bmeHighchart",        // "div" element in html file where the chart is displayed
    borderRadius: 20
  },
  series: [
    { name: "Temperature (ºC)", color: tColor },  
    { name: "Rel. Humidity (%)", color: rhColor, yAxis: 1 } ,  
    { name: "Pressure (mbar)", color: pColor, yAxis: 2 }  
  ],
  title: { text: undefined },       // The containing card already includes the title.
  plotOptions: { line: { animation: false } },

  xAxis: {
    gridLineColor: gridColor,
    gridLineWidth: gridWidth,
    type: "datetime",
    dateTimeLabelFormats: { second: "%H:%M:%S" }
  },
  yAxis: [    
    { title: { style: { color: tColor }, text: "Temperature (ºC)" }, 
      labels: { style: { color: tColor } },
      lineColor: tColor,
      lineWidth: 1,
      gridLineColor: gridColor,
      gridLineWidth: gridWidth
    },
    { //title: { text: "Humidity (%)", style: {color: rhColor } },
      labels: { style: {color: rhColor } },
      lineColor: rhColor,
      lineWidth: 1,
      gridLineColor: gridColor,
      gridLineWidth: gridWidth
    }, 
    { //title: { text: "Pressure (mbar)", style: {color: pColor} }, 
      labels: { style: {color: pColor } },
      lineColor: pColor,
      lineWidth: 1,
      opposite: true,             // display on the right hand side
      gridLineColor: gridColor,
      gridLineWidth: gridWidth
    }, 
  ],
  credits: { enabled: true }
});

// Plot received object on chart (Highcharts)
function highChartPlot(arr) {
  const x = arr[0];
  for (let i = 1; i < arr.length; i++) {
    const y = Number(arr[i]);
    if (highChart.series[i-1].data.length > numPoints) {
      highChart.series[i-1].addPoint([x, y], true, true, true);
    } 
    else { highChart.series[i-1].addPoint([x, y], true, false, true); }
  }
}

// ===============================================================================
// OPTION: CHARTS.JS --> Create single chart with all values / different timestamp
// ===============================================================================
// Define rounders for each of the lines (t, p, rh) and round factor: 
const tRound = 3, 
      rhRound = 5,
      pRound = 1, 
      roundFactor = 0.5,
			numTicks = 7;

// Define series (data sets)
const dataChartJS = {
  datasets: [
    { label: "Temperature (ºC)",
      data: [],              // each element will be {x: ## , y: ## } or [x, y]
      borderColor: tColor,
      yAxisID: "y0",
      // fill: false
    },
    { label: "Rel. Humidity (%)",
      data: [],             // {x: ## , y: ## } or [x, y]
      borderColor: rhColor,
      yAxisID: "y1",
      // fill: false
    },
    { label: "Pressure (mbar)",
      data: [],              // {x: ## , y: ## } or [x, y]
      borderColor: pColor,
      yAxisID: "y2",
      // fill: false
    }
  ]
};

// Functions to find rounded min and max values for a series:
function minVal(ser, round) {
  if (dataChartJS.datasets[ser].data.length == 0) return 0;
  const yArr = dataChartJS.datasets[ser].data.map((item) => item[1]);          // keep only the "y" values
  return round * Math.floor( (Math.min(...yArr) - round * roundFactor ) / round);
}
function maxVal(ser, round) {
  if (dataChartJS.datasets[ser].data.length == 0) return 1;
  const yArr = dataChartJS.datasets[ser].data.map((item) => item[1]);  // keep only the "y" values
  return round * Math.ceil( (Math.max(...yArr) + round * roundFactor ) / round);
}

// Define chart configuration
const options = { 
  // responsive: true,          // Not required (is default). Needed to allow resizing of the canvas
  // showLine: true,            // Not required (is default).
  legend: { display: false },   // Card title defined in HTML instead.
  backgroundColor: bgColor,
  scales: {
    x: { 
      type: "time",             // Requires Charts.js date "adapter"
      border: { color: "#111" },
      ticks: { maxTicksLimit: 16, color: "#111" }
    },
    y0: { // title: { display: true, text: "Temperature (ºC)", color: tColor},
      min: minVal(0, tRound),      // Optional: round min/max values
      max: maxVal(0, tRound),      // Optional: round min/max values
      border: { color: tColor },
      ticks: { color: tColor, count: numTicks }
    },
    y1: { // title: { display: true, text: "Rel. Humidity (%)", color: rhColor},
      min: minVal(1, rhRound),           // Optional: round min/max values
      max: maxVal(1, rhRound),           // Optional: round min/max values
      border: { color: rhColor },
      ticks: { color: rhColor, count: numTicks }
    },
    y2: { // title: { display: true, text: "Pressure (mbar)", color: pColor},
      position: "right",
      min: minVal(2, pRound),           // Optional: round min/max values
      max: maxVal(2, pRound),           // Optional: round min/max values
      border: { color: pColor },
      ticks: { color: pColor, count: numTicks }
    },
  }
}
// Create the chart with the configuration defined previously
const chartJS = new Chart(
  document.getElementById("bmeChartJS"), 
  { type: "line", data: dataChartJS, options: options}
);

// Plot received object on chart (Charts.js)
function chartJSPlot(arr) {
  const x = arr[0];
  for (let i = 1; i < arr.length; i++) {
    const y = arr[i];
    if (chartJS.data.datasets[i-1].data.length > numPoints) {
      chartJS.data.datasets[i-1].data.shift();
    }
    chartJS.data.datasets[i-1].data.push([x, y]);  
  }
}

// ===============================================================================
// OPTION: PLOTLY --> Create single chart with all values / different timestamp
// ===============================================================================
const plotly = document.getElementById("bmePlotly");
const x0 = 80 / (plotly.offsetWidth + 50);
const dataPlotly = [
  { x: [], y: [],
    name: "Temperature (ºC)",
    line: {color: tColor}
  },
  { x: [], y: [],
    name: "Rel. Humidity (%)",
    yaxis: "y2",
    line: {color: rhColor}
  },
  { x: [], y: [],
    name: "Pressure (mbar)",
    yaxis: "y3",
    line: {color: pColor}
  }
];

const layout = { 
	margin: { t: 20, b: 20, l: 20, r: 80}, 
	paper_bgcolor: bgColor,
	plot_bgcolor: bgColor,
  xaxis: {type: "date", domain: [x0, 1],
    showline: true, linecolor: "#222"
  },
	yaxis: {
		tickfont: {color: tColor, size: 10},
		anchor: "x",
    showline: true,
    linecolor: tColor
  },
  yaxis2: {
    tickfont: {color: rhColor, size: 10},
    tickmode: "sync",
		overlaying: "y",
    position: 0.01,
		anchor: "free",
    showline: true, 
    linecolor: rhColor
  },
	yaxis3: {
    tickfont: {color: pColor, size: 10},
    tickmode: "sync",
    overlaying: "y",
    anchor: "x",
    side: "right",
    showline:true, linecolor: pColor
  },
	legend: {
		"orientation": "h",
		y: 1.1, x: 0.5,
		xanchor: "center"
	}
};
const config = {
	responsive: true,
	// scrollZoom: true,
	showLink: true,
  plotlyServerURL: "https://chart-studio.plotly.com"
}

// Create the Plotly chart
Plotly.newPlot("bmePlotly", dataPlotly, layout, config);

// Plot received object on chart (Plotly)
function plotlyPlot(arr) {
  // const xVal = arr[0];
  for (let i = 1; i < arr.length; i++) {
    Plotly.extendTraces("bmePlotly",
    { x: [[arr[0]]], y: [[arr[i]]] },      // Point to be added
    [i-1] );                            // Series to get the point
  }
}

// ===============================================================================
// Function to apply format to time values to be displayed:
// ===============================================================================
/**
 * Formats a given time into a localized string based on Spanish (Spain) locale.
 * @param {Date|string|number} time - The time to format (Date object, string representing a date, or timestamp).
 * @returns {string} - The formatted time string in "es-ES" locale.
 */
function formatTime(time) {
  const config = {
    // timeZone: "Europe/Madrid", 
    day: "numeric", month: "2-digit", year: "2-digit", 
    hour: "numeric", minute: "numeric", second: "2-digit"
  }
  return new Date(time).toLocaleString("es-ES", config);
}

// ===============================================================================
// Function to update values (arr = [time, t, rh, p])
// ===============================================================================
/**
 * Update the BME data on the webpage.
 * Update both the data cards and the table with the provided BME sensor data.
 * @param {Array} arr - An array containing BME sensor data.
 * @param {number} arr[0] - The timestamp of the data.
 * @param {number} arr[1] - The temperature value.
 * @param {number} arr[2] - The relative humidity value.
 * @param {number} arr[3] - The pressure value.
 */
function updateBME(arr) {
  // Update data cards
  document.getElementById("timeBME").textContent = formatTime(arr[0]);
  document.getElementById("tBME").textContent = arr[1];
  document.getElementById("rhBME").textContent = arr[2];
  document.getElementById("pBME").textContent = arr[3];

  // Update Table
  document.getElementById("timeBMEtable").textContent = formatTime(arr[0]);
  document.getElementById("tBMEtable").textContent = arr[1];
  document.getElementById("rhBMEtable").textContent = arr[2];
  document.getElementById("pBMEtable").textContent = arr[3];
}

// ===============================================================================
// Function to resize the chart by Charts.js
// ===============================================================================
/**
 * Adjust min/max values of the y-axes for the ChartJS instance.
 * Update the scales of the ChartJS instance by setting the minimum
 * and maximum values for each y-axis (y0, y1, y2) based on the provided rounding
 * functions (tRound, rhRound, pRound).
 * @function resizeChartJS
 */
function resizeChartJS() {
  chartJS.options.scales.y0.min = minVal(0, tRound);
  chartJS.options.scales.y1.min = minVal(1, rhRound);
  chartJS.options.scales.y2.min = minVal(2, pRound);
  chartJS.options.scales.y0.max = maxVal(0, tRound);
  chartJS.options.scales.y1.max = maxVal(1, rhRound);
  chartJS.options.scales.y2.max = maxVal(2, pRound);
}

// ===============================================================================
// Function to add a new point (arr = [time, t, rh, p]) on the charts
// ===============================================================================
function plotBME(arr) {
  highChartPlot(arr);   // Update chart (Highcharts)
  plotlyPlot(arr);      // Update chart (Plotly)
  chartJSPlot(arr);     // Update chart (Charts.js):
  resizeChartJS();      // Update y scales
  chartJS.update();     // Update the chart
}

// ============================================================================
// Process BME data-file received and limit numPoints
// ============================================================================
/**
 * Processes BME sensor data from a JSON array and extracts the last `numPoints` data points.
 * @param {Array<Object>} jsonArray - Array of objects containing BME sensor data.
 * @param {number} numPoints - Number of data points to extract from the end of the array.
 * @returns {Array<Array<number|string>>} - Array of processed data points, each containing:
 *   [time (ms), temperature (°C), relative humidity (%), pressure (hPa)].
 */
function processBMEData(jsonArray, numPoints) {
  const len = jsonArray.length,
        i0 = Math.max(0, len - numPoints),      // Extract the last `numPoints` data points
        processedData = [];
  
  for (let i = i0; i < len; i++) {
    const time = jsonArray[i].time * 1000,      // Convert epoch (seconds) to milliseconds
          t = (jsonArray[i].t / 10).toFixed(1),
          rh = (jsonArray[i].rh / 10).toFixed(1),
          p = (jsonArray[i].p / 10).toFixed(1);
    processedData.push([time, t, rh, p]);
  }
  return processedData;
}

// ============================================================================
// Update the charts with the processed data
// ============================================================================
/**
 * Updates the chart with the given processed data.
 * @param {Array} processedData - An array of arrays, where each inner array contains:
 *                                [time, temperature, relative humidity, pressure].
 *                                - time {number}: The timestamp of the data point.
 *                                - t {number}: The temperature value.
 *                                - rh {number}: The relative humidity value.
 *                                - p {number}: The pressure value.
 */
function updateChart(processedData) {
  processedData.forEach(row => {
    const [time, t, rh, p] = row;
    plotBME([time, t, rh, p]); // Plot the BME data
    if (processedData.indexOf(row) === processedData.length - 1) {
      updateBME([time, t, rh, p]); // Update BME if it"s the last data point
    }
  });
  resizeChartJS(); // Rescale Y axis
  chartJS.update(); // Update the chart
}

// ============================================================================
// Complete process: retrieve file, process the data and update the charts
// ============================================================================
function plotBMEfile() {
  fetchAndFixJSON("/data-file")         // Fetch the BME data-file
    .then(jsonArray => processBMEData(jsonArray, numPoints)) // Process the data (apply conversions and filter the number of points)
    .then(processedData => updateChart(processedData)) // Update the chart with the processed data
    .catch(error => console.error("Error retrieving BME data:", error)); // Handle any errors
}

// ===============================================================================
//  Function to update data and charts when a new reading is received
// ===============================================================================
/**
 * Refreshes BME sensor data by parsing the reading, extracting relevant values,
 * and updating and plotting the data.
 * @param {string} reading - The JSON string containing BME sensor data.
 * @property {number} time - The timestamp of the reading in seconds.
 * @property {number} t - The temperature reading in tenths of a degree.
 * @property {number} rh - The relative humidity reading in tenths of a percent.
 * @property {number} p - The pressure reading in tenths of a hPa.
 */
function BMErefresh(reading) {
  const objBME = JSON.parse(reading);
  const time = objBME.time * 1000,
        t = (objBME.t / 10).toFixed(1),
        rh = (objBME.rh / 10).toFixed(1),
        p = (objBME.p / 10).toFixed(1);
  updateBME([time, t, rh, p]);
  plotBME([time, t, rh, p]);
}

// ============================================================================
//  Button "Refresh" --> trigger function refresh()
// ============================================================================
// We send a "GET" request with URL= "/refresh" --> get single point
// And process response with "refresh" + "plot" to update values and charts
/**
 * Sends an asynchronous GET request to the "/refresh" endpoint to update data.
 * Displays an alert message "Data updated" when the request is successfully completed.
 */
function refresh() {
  fetch("/refresh")
    .then(response => {
      if (response.ok) { alert("Data updated"); }     // Feedback update OK
      else { throw new Error(response.statusText); }  // Throw error for non-OK response
    })
    .catch(error => console.error("Error:", error));
}

// ============================================================================
// Handle data received via events
// ============================================================================
// run function plotBMEfile() when the page is loaded
window.addEventListener("load", plotBMEfile);

// Create an Event Source to listen for events.
// The condition checks if the browser supports Server-Sent Events (SSE) by testing the existence of window.EventSource
if (!!window.EventSource) {
  const source = new EventSource("/eventsBME");

  source.addEventListener("open", function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener("error", function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.error("Events Disconnected: ", e);
    }
  }, false);

  // Update charts when new periodic readings are received (timerBME)
  source.addEventListener("newBMEreading", (e) => BMErefresh(e.data), false );
}
