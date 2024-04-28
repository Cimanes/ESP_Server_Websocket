// Function to get formatted current time and date
function getTime() {
  const time = new Date();
  const timeString = time.toLocaleString('es-ES', {timeZone: 'Europe/Madrid', day: 'numeric', month: "2-digit", hour: 'numeric', minute: 'numeric', second: '2-digit'});
  document.getElementById('timeBME').textContent = timeString;
  document.getElementById('timeBMEtable').textContent = timeString;
}

// Function to convert the data received into JSON and assign values to HTML elements
function parseData(str1, str2) {
  const objBME = JSON.parse(str2);
  document.getElementById('tBME').textContent = (objBME.t / 10).toFixed(1);
  document.getElementById('rhBME').textContent = (objBME.rh / 10).toFixed(1);
  document.getElementById('pBME').textContent = objBME.p / 10;
  document.getElementById('tBMEtable').textContent = (objBME.t / 10).toFixed(1);
  document.getElementById('rhBMEtable').textContent = (objBME.rh / 10).toFixed(1);
  document.getElementById('pBMEtable').textContent = objBME.p / 10;  
  console.log(str1 + ': ' + JSON.stringify(objBME));
}

// ===============================================================================
// Get current sensor readings when the page loads:
// ===============================================================================
// We send a "GET" request with URL= '/readings'
// And process response with "parseData" to update values
window.addEventListener('load', getReadings);

function getReadings() {
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      parseData('load', this.responseText);
      getTime();
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
    parseData('new_reading', e.data);
    getTime();
  }, false);
}
