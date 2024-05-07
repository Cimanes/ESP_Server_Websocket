// Retrieve color variables from defined in CSS:
const tColor = getComputedStyle(document.documentElement).getPropertyValue("--tColor"),
      rhColor = getComputedStyle(document.documentElement).getPropertyValue("--rhColor"),
      pColor = getComputedStyle(document.documentElement).getPropertyValue("--pColor"),
      timeColor = getComputedStyle(document.documentElement).getPropertyValue("--timeColor"),
      bgColor = getComputedStyle(document.documentElement).getPropertyValue("--bgColor"),
      gridColor = '#666',
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
function highChartPlot(value, ser) {
  const x = (new Date()).getTime();
  const y = Number(value);
  if(highChart.series[ser].data.length > 100) {
    highChart.series[ser].addPoint([x, y], true, true, true);
  } 
  else { highChart.series[ser].addPoint([x, y], true, false, true); }
}

// ===============================================================================
// OPTION: CHARTS.JS --> Create single chart with all values / different timestamp
// ===============================================================================
// Note: Charts.js uses canvas. Canvas does not resize automatically 
// We need the following function to resize when window is resized.

const options = { 
  responsive: true,
  backgroundColor: bgColor,
  showLine: true, 
  legend: { display: false }, 
  scales: {
    x: { 
      type: 'time',
      maxTicksLimit: 10
    },
    y1: {
      type: 'linear',
      display: true,
      position: 'left',
      min: 5,
      max: 35,
      ticks: { stepSize: 5 }
    },
    y2: {
      type: 'linear',
      display: true,
      position: 'left',
      min: 10,
      max: 100,
      ticks: { stepSize: 15 }
    },
    y3: {
      type: 'linear',
      display: true,
      position: 'right',
      min: 950,
      max: 1040,
      ticks: { stepSize: 15 }
    },
  }
}

const data = {
  datasets: [
    { label: 'Temperature (ºC)',
      data: [],                   // {x: ## , y: ## }
      borderColor: tColor,
      yAxisID: 'y1',
      // fill: false
    },
    { label: 'Rel. Humidity (%)',
    data: [],                   // {x: ## , y: ## }
    borderColor: rhColor,
      yAxisID: 'y2',
      // fill: false
    },
    { label: 'Pressure (mbar)',
    data: [],                   // {x: ## , y: ## }
    borderColor: pColor,
      yAxisID: 'y3',
      // fill: false
    }
  ]
};

const chartJS = new Chart(
  document.getElementById("bmeChartJS"), 
  { type: 'line', 
    data: data, 
    options: options
  }
);

// Plot received object on chart (Charts.js)
function chartJSPlot(value, ser) {
  const xVal = (new Date()).getTime();
  const yVal = Number(value);
  if(chartJS.data.datasets[ser].data.length > 100) {
    chartJS.data.datasets[ser].data.shift();
  } 
  chartJS.data.datasets[ser].data.push([xVal, yVal]);
  console.log(xVal, yVal);
  console.log(chartJS.data.datasets[ser].data);
}

// Function to get formatted current time and date
function updateTime() {
  const time = new Date();
  const timeString = time.toLocaleString('es-ES', {timeZone: 'Europe/Madrid', day: 'numeric', month: "2-digit", hour: 'numeric', minute: 'numeric', second: '2-digit'});
  document.getElementById('timeBME').textContent = timeString;
  document.getElementById('timeBMEtable').textContent = timeString;
}

// Function to retrieve data from the JSON received and assign those values to HTML elements
function updateBME(str, objBME) {
  const t = (objBME.t / 10).toFixed(1),
        rh = (objBME.rh / 10).toFixed(1),
        p = (objBME.p / 10).toFixed(0);

  // Update data cards
  document.getElementById('tBME').textContent = t;
  document.getElementById('rhBME').textContent = rh;
  document.getElementById('pBME').textContent = p;

  // Update Table
  document.getElementById('tBMEtable').textContent = t;
  document.getElementById('rhBMEtable').textContent = rh;
  document.getElementById('pBMEtable').textContent = p;

  // Update chart (Highcharts)
  highChartPlot(t, 0);
  highChartPlot(rh, 1);
  highChartPlot(p, 2);

  // Update chart (Charts.js)
  chartJSPlot(t, 0);
  chartJSPlot(rh, 1);
  chartJSPlot(p, 2);
  chartJS.update();
}

// ===============================================================================
// Get current sensor readings when the page loads:
// ===============================================================================
// We send a "GET" request with URL= '/readings'
// And process response with "updateBME" to update values
window.addEventListener('load', getReadings);

function getReadings() {
  console.log(tColor, rhColor, pColor, timeColor, bgColor);


  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const objBME = JSON.parse(this.responseText);
      updateTime();
      updateBME('load', objBME);
    } 
  }; 
  xhr.open('GET', '/readings', true);
  xhr.send();
}

// ===============================================================================
// Handle data received periodically
// ===============================================================================
// Create an Event Source to listen for events
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

// ===============================================================================
// Update charts when new readings are received
// ===============================================================================
source.addEventListener('new_readings', function(e) {
  const objBME = JSON.parse(e.data);
  updateTime();
  updateBME('new_reading', objBME);
}, false);
}
