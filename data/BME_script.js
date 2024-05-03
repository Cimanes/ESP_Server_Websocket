// Function to get formatted current time and date
function getTime() {
  const time = new Date();
  const timeString = time.toLocaleString('es-ES', {timeZone: 'Europe/Madrid', day: 'numeric', month: "2-digit", hour: 'numeric', minute: 'numeric', second: '2-digit'});
  document.getElementById('timeBME').textContent = timeString;
  document.getElementById('timeBMEtable').textContent = timeString;
}

// Function to retrieve data from the JSON received and assign those values to HTML elements
function updateBME(str, objBME) {
  document.getElementById('tBME').textContent = (objBME.t / 10).toFixed(1);
  document.getElementById('rhBME').textContent = (objBME.rh / 10).toFixed(1);
  document.getElementById('pBME').textContent = objBME.p / 10;
  document.getElementById('tBMEtable').textContent = (objBME.t / 10).toFixed(1);
  document.getElementById('rhBMEtable').textContent = (objBME.rh / 10).toFixed(1);
  document.getElementById('pBMEtable').textContent = objBME.p / 10;  
  console.log(str + ': ' + JSON.stringify(objBME));
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
      updateBME('load', objBME);
      getTime();
      // OPTION for separate charts
      // plot((objBME.t / 10).toFixed(1), tChart);
      // plot((objBME.rh / 10).toFixed(1), rhChart);
      // plot((objBME.p / 10).toFixed(0), pChart);

      // OPTION for single charts with all data
      bmePlot((objBME.t / 10).toFixed(1), 0);
      bmePlot((objBME.rh / 10).toFixed(1), 1);
      bmePlot((objBME.p / 10).toFixed(0), 2);
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

source.addEventListener('new_readings', function(e) {
  const objBME = JSON.parse(e.data);
  updateBME('new_reading', objBME);
  getTime();

  // OPTION for separate charts;
  // plot((objBME.t / 10).toFixed(1), tChart);
  // plot((objBME.rh / 10).toFixed(1), rhChart);
  // plot((objBME.p / 10).toFixed(0), pChart);
  
  // OPTION for single charts with all data
  bmePlot((objBME.t / 10).toFixed(1), 0);
  bmePlot((objBME.rh / 10).toFixed(1), 1);
  bmePlot((objBME.p / 10).toFixed(0), 2);
}, false);
}

// ===============================================================================
// OPTION: Create separate charts
// ===============================================================================
// Create Temperature Chart
// const tChart = new Highcharts.Chart({
//   time:{ useUTC: false },
//   chart:{ 
//     renderTo:'tChart', 
//     borderRadius: 20,
//   },
//   series: [ { name: 'Temp' } ],
//   title: { text: undefined },
//   plotOptions: {
//     line: { 
//       animation: false
//       // dataLabels: { enabled: true }
//     },
//     series: { color: '#922' }
//   },
//   xAxis: {
//     type: 'datetime',
//     dateTimeLabelFormats: { second: '%H:%M:%S' }
//   },
//   yAxis: { title: { text: 'Temperature (ºC)' } },
//   credits: { enabled: true }
// });
  
// // Create Humidity Chart
// const rhChart = new Highcharts.Chart({
//   chart:{ renderTo:'rhChart' },
//   series: [{ name: 'RH' }],
//   title: { text: undefined },    
//   plotOptions: {
//     line: { 
//       animation: false,
//       dataLabels: { enabled: true }
//     },
//     series: { color: '#173' }
//   },
//   xAxis: {
//     type: 'datetime',
//     dateTimeLabelFormats: { second: '%H:%M:%S' }
//   },
//   yAxis: { title: { text: 'Humidity (%)' } },
//   credits: { enabled: false }
// });

// // Create Pressure Chart
// const pChart = new Highcharts.Chart({
//   chart:{ renderTo:'pChart' },
//   series: [{ name: 'Press' }],
//   title: { text: undefined },    
//   plotOptions: {
//     line: { 
//       animation: false,
//       dataLabels: { enabled: true }
//     },
//     series: { color: '#169' }
//   },
//   xAxis: {
//     type: 'datetime',
//     dateTimeLabelFormats: { second: '%H:%M:%S' }
//   },
//   yAxis: { title: { text: 'Pressure (mbar)' } },
//   credits: { enabled: false }
// });

// //Plot value in chart:
// function plot(value, chart) {
//   const x = (new Date()).getTime()
//   const y = Number(value);
//   if(chart.series[0].data.length > 40) {
//     chart.series[0].addPoint([x, y], true, true, true);
//   } 
//   else { chart.series[0].addPoint([x, y], true, false, true); }
// }



// ===============================================================================
// OPTION: Create single chart with all values
// ===============================================================================
// Create global Chart
const bmeChart = new Highcharts.Chart({
  time:{ useUTC: false },
  chart:{ 
    height: (9 / 16 * 100) + '%',
    renderTo:'bmeChart', 
    borderRadius: 20,
  },
  series: [ 
    { name: 'Temp', color: '#922' },  
    { name: 'RH', color: '#173', yAxis: 1 } ,  
    { name: 'Press', color: '#169', yAxis: 2 }  
  ],
  title: { text: undefined },
  plotOptions: { line: { animation: false } },
  xAxis: {
    type: 'datetime',
    dateTimeLabelFormats: { second: '%H:%M:%S' }
  },
  yAxis: [
    { title: { style: { color: '#922' }, text: 'Temperature (ºC)' }, 
      labels: { style: { color: '#922' } }
    },
    { title: { text: 'Humidity (%)', style: {color: '#173' } },
      labels: { style: {color: '#173' } }
    }, 
    { title: { text: 'Pressure (mbar)', style: {color: '#169'} }, 
      labels: { style: {color: '#169' } },
      opposite: true
    }, 
  ],
  // credits: { enabled: true }
});

function bmePlot(value, ser) {
  const x = (new Date()).getTime()
  const y = Number(value);
  if(bmeChart.series[ser].data.length > 40) {
    bmeChart.series[ser].addPoint([x, y], true, true, true);
  } 
  else { bmeChart.series[ser].addPoint([x, y], true, false, true); }
}