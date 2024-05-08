// Configuration: Retrieve color variables from defined in CSS:
const tColor = getComputedStyle(document.documentElement).getPropertyValue("--tColor"),
      rhColor = getComputedStyle(document.documentElement).getPropertyValue("--rhColor"),
      pColor = getComputedStyle(document.documentElement).getPropertyValue("--pColor"),
      timeColor = getComputedStyle(document.documentElement).getPropertyValue("--timeColor"),
      bgColor = getComputedStyle(document.documentElement).getPropertyValue("--bgColor"),
      gridColor = getComputedStyle(document.documentElement).getPropertyValue("--gridColor"),
      gridWidth = 0.5;
    
// ===============================================================================
// OPTION: HIGHCHARTS --> Create single chart with all values
// ===============================================================================
// Create global Chart
const highChart = new Highcharts.Chart({
  time:{ useUTC: false },
  chart:{ 
    backgroundColor: bgColor,
    height: (9 / 16 * 100) + '%',
    renderTo:'bmeHighchart', 
    borderRadius: 20,
  },
  series: [ 
    { name: 'Temp', color: tColor },  
    { name: 'RH', color: rhColor, yAxis: 1 } ,  
    { name: 'Press', color: pColor, yAxis: 2 }  
  ],
  title: { text: undefined },       // The containing card already includes the title.
  plotOptions: { line: { animation: false } },

  xAxis: {
    gridLineColor: gridColor,
    gridLineWidth: gridWidth,
    type: 'datetime',
    dateTimeLabelFormats: { second: '%H:%M:%S' }
  },
  yAxis: [    
    { title: { style: { color: tColor }, text: 'Temperature (ºC)' }, 
      labels: { style: { color: tColor } },
      lineColor: tColor,
      lineWidth: 1,
      gridLineColor: gridColor,
      gridLineWidth: gridWidth
    },
    { title: { text: 'Humidity (%)', style: {color: rhColor } },
      labels: { style: {color: rhColor } },
      lineColor: rhColor,
      lineWidth: 1,
      gridLineColor: gridColor,
      gridLineWidth: gridWidth
    }, 
    { title: { text: 'Pressure (mbar)', style: {color: pColor} }, 
      labels: { style: {color: pColor } },
      lineColor: pColor,
      lineWidth: 1,
      opposite: true,
      gridLineColor: gridColor,
      gridLineWidth: gridWidth
    }, 
  ],
  credits: { enabled: true }
});

// Plot received object on chart (Highcharts)
function highChartPlot(arr, time) {
  const x = time.getTime();
  for (let i = 0; i < arr.length; i++) {
    const y = Number(arr[i]);
    if(highChart.series[i].data.length > 100) {
      highChart.series[i].addPoint([x, y], true, true, true);
    } 
    else { highChart.series[i].addPoint([x, y], true, false, true); }
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
    { label: 'Temperature (ºC)',
      data: [],              // each element will be {x: ## , y: ## } or [x, y]
      borderColor: tColor,
      yAxisID: 'y0',
      // fill: false
    },
    { label: 'Rel. Humidity (%)',
      data: [],             // {x: ## , y: ## } or [x, y]
      borderColor: rhColor,
      yAxisID: 'y1',
      // fill: false
    },
    { label: 'Pressure (mbar)',
      data: [],              // {x: ## , y: ## } or [x, y]
      borderColor: pColor,
      yAxisID: 'y2',
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
      type: 'time',             // Requires Charts.js date "adapter"
      border: { color: '#111' },
      ticks: { maxTicksLimit: 16, color: '#111' }
    },
    y0: {
      title: { display: true, text: 'Temperature (ºC)', color: tColor},
      type: 'linear',
      // display: true,            // Not required (is default).
      position: 'left',
      min: minVal(0, tRound),      // Optional: round min/max values
      max: maxVal(0, tRound),      // Optional: round min/max values
      border: { color: tColor },
      ticks: { color: tColor, count: numTicks }
    },
    y1: {
      title: { display: true, text: 'Rel. Humidity (%)', color: rhColor},
      type: 'linear',
      position: 'left',
      min: minVal(1, rhRound),           // Optional: round min/max values
      max: maxVal(1, rhRound),           // Optional: round min/max values
      border: { color: rhColor },
      ticks: { color: rhColor, count: numTicks }
    },
    y2: {
      title: { display: true, text: 'Pressure (mbar)', color: pColor},
      type: 'linear',
      position: 'right',
      min: minVal(2, pRound),           // Optional: round min/max values
      max: maxVal(2, pRound),           // Optional: round min/max values
      border: { color: pColor },
      ticks: { color: pColor, count: numTicks }
    },
  }
}

const chartJS = new Chart(
  document.getElementById("bmeChartJS"), 
  { type: 'line', 
    data: dataChartJS, 
    options: options
  }
);

// Plot received object on chart (Charts.js)
function chartJSPlot(arr, time) {
  const x = time.getTime();
  for (let i = 0; i < arr.length; i++) {
    const y = arr[i];
    if (chartJS.data.datasets[i].data.length > 100) {
      chartJS.data.datasets[i].data.shift();
    }
    chartJS.data.datasets[i].data.push([x, y]);  
  }
}

// Function to get formatted current time and date
function updateTime() {
  const config = {
    timeZone: 'Europe/Madrid', 
    day: 'numeric', 
    month: '2-digit', 
    hour: 'numeric', 
    minute: 'numeric', 
    second: '2-digit'
  }
  const date = new Date();
  const timeString = date.toLocaleString('es-ES', config);
  document.getElementById('timeBME').textContent = timeString;
  document.getElementById('timeBMEtable').textContent = timeString;
  return date;
}

// Function to retrieve data from the JSON received and assign those values to HTML elements
function updateBME(objBME, date) {
  const t = (objBME.t / 10).toFixed(1),
        rh = (objBME.rh / 10).toFixed(1),
        p = (objBME.p / 10).toFixed(0),
        time = new Date(date);

  // Update data cards
  document.getElementById('tBME').textContent = t;
  document.getElementById('rhBME').textContent = rh;
  document.getElementById('pBME').textContent = p;

  // Update Table
  document.getElementById('tBMEtable').textContent = t;
  document.getElementById('rhBMEtable').textContent = rh;
  document.getElementById('pBMEtable').textContent = p;

  // Update chart (Highcharts)
  highChartPlot([t, rh, p], time)

  // Update chart (Charts.js):
  // 1) define the new points
  chartJSPlot([t, rh, p], time)
  // optional: define the new limits for Y Axis
  chartJS.options.scales.y0.min = minVal(0, tRound);
  chartJS.options.scales.y1.min = minVal(1, rhRound);
  chartJS.options.scales.y2.min = minVal(2, pRound);
  chartJS.options.scales.y0.max = maxVal(0, tRound);
  chartJS.options.scales.y1.max = maxVal(1, rhRound);
  chartJS.options.scales.y2.max = maxVal(2, pRound);
  // Update the chart
  chartJS.update();
}

// ===============================================================================
// Get current sensor readings when the page loads:
// ===============================================================================
// We send a "GET" request with URL= '/readings'
// And process response with "updateBME" to update values
window.addEventListener('load', getReadings);

function getReadings() {
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const objBME = JSON.parse(this.responseText);
      const time = updateTime();
      updateBME(objBME, time);
    }
  };
  xhr.open('GET', '/readings', true);
  xhr.send();
}

// ===============================================================================
// Handle data received periodically
// ===============================================================================
// Create an Event Source to listen for events.
// The condition checks if the browser supports Server-Sent Events (SSE) by testing the existence of window.EventSource
if (!!window.EventSource) {
  const source = new EventSource('/events');
  source.addEventListener('open', function(e) {
    console.log('Events Connected');
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log('Events Disconnected');
    }
  }, false);

  // Update charts when new readings are received
  source.addEventListener('new_readings', function(e) {
    const objBME = JSON.parse(e.data);
    const date = updateTime();
    updateBME(objBME, date);
  }, false);
}
