// Get current sensor readings when the page loads  
window.addEventListener('load', getReadings);

// Function to convert the data received into JSON and assign values to HTML elements
function parseData(str1, str2) {
  const objBME = JSON.parse(str2);
  document.getElementById("tBME").textContent = (objBME.t / 10).toFixed(1);
  document.getElementById("rhBME").textContent = (objBME.rh / 10).toFixed(1);
  document.getElementById("pBME").textContent = objBME.p / 10;  
  console.log(str1 + ": " + JSON.stringify(objBME));
}

// Function to get current readings on the web page when it loads for the first time 
function getReadings() {
  const xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      parseData("load", this.responseText);
    } 
  }; 
  xhr.open("GET", "/readings", true);
  xhr.send();
}
// Create an Event Source to listen for events
// The condition checks if the browser supports Server-Sent Events (SSE) by testing the existence of window.EventSource
if (!!window.EventSource) {
  const source = new EventSource('/events');
  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('new_readings', function(e) {
    parseData("new_reading", e.data);
  }, false);
}
