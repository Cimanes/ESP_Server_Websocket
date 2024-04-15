// Initiate a WebSocket connection with the server when the web interface is fully loaded in the browser
// Receive requests from user on the web page (press buttons / toggle switches / slider bars...)
// Transfer those requests to the server via Websocket messages.
// Receive feedback from server in Websocket messages.
// Process that feedback and update data and style in the web page accordingly.

//===============================================================================
// OPTIONS
//===============================================================================
const logger = 1,                                   // 1 if console log is desired / 0 if not
      colorON = '#048',                             // Color for feedback text when "ACTIVE"
      colorOFF = '#888',                            // Color for feedback text when "INACTIVE"
      useButton = 1,                                // 1 if buttons are used / 0 if not
      arrButton = ['ON', 'OFF', 'AUTO', 'MAN'],     // update with button commands
      useToggle = 1,                                // 1 if toggle switches are used / 0 if not
      arrToggle = ['12', '14'],                     // update with GPIO's used for toggles
      usePWM = 1,                                   // 1 if PWM analog outputs are used / 0 if not
      useAVAR = 1,                                  // 1 if analog control variables are used / 0 if not
      useBVAR = 1,                                  // 1 if boolean control variables are used / 0 if not
      decimals = 1,                                 // Define number of decimals for analog readings
      aFactor = 10 ** decimals;                     // Factor for conversion of analog values (server uses int; factor = 10 -> one decimal place)

//===============================================================================
// MANAGE WEBSOCKET
//===============================================================================
// Define the gateway as the current URL using template literal with variable (the webserver IP address)
// Initialize the WebSocket connection on that gateway:
const gateway = `ws://${window.location.hostname}/ws`,
      websocket = new WebSocket(gateway);

// Add an event listener to run function when the page loads:
window.addEventListener('load',  onload());
function  onload(event) {  initWebSocket();  }

// Assign callback functions used when the WebSocket connection is opened, closed or when a message is received: 
function initWebSocket() {
  if(logger) console.log('Trying to open WebSocket...');
  websocket.onopen    = onOpen;
  websocket.onmessage = onMessage;
  websocket.onclose   = onClose;
}

// When the websocket is opened:
// Send messages to update feedback from outputs and variables using websocket:
// (Server will reply message with state of ALL Buttons and Toggle Switches).
function onOpen(event) { 
  if(logger) console.log('WebSocket opened: Update all');
  websocket.send(`{"all": ""}`); 
}

// Re-initiate the Websocket if the connection gets lost.
function onClose(event) {
  if(logger) console.log('WebSocket closed');
  setTimeout(initWebSocket, 2000);
} 

//===============================================================================
// HANDLE USER ACTIONS IN HTML
//===============================================================================
// function press(element) is called by the HTML when one button is pressed
// Send message (object: {"but":"element.id"}) using websocket to the server to toggle D.O. channel number
if (useButton) {
  function press(element) {
    const msg = `{"but": "${element.id}"}`;
    if (logger) console.log('button ' + element.id);
    websocket.send(msg);
  }
}
// function toggle(element) is called by the HTML when one "D.O." element (toggle switch) is operated
// Send message (object: {"tog":"element.id"}) using websocket to the server to toggle D.O. channel number
if (useToggle) {
  function toggle(element) {
    const msg = `{"tog": ${element.id}}`;
    if (logger) console.log('toggle ' + element.id);
    websocket.send(msg);
  }
}
// function bvar(element) is called by the HTML when one boolean variable is toggled by the user
// Send message (object: {"var":"XX"}) using websocket to the server to toggle the value of that variable.
if (useBVAR) {
  function bvar(element) {
    const msg = `{"bvar": "${element.id}"}`;
    if (logger) console.log('bvar ' + element.id);
    websocket.send(msg);
  }
}
// function tune(element) is called by the HTML when one A.O. (PWM) is adjusted
if (usePWM) {
  function tune(element, value) {
    // const tuneValue = document.getElementById(element.id).value;
    const msg = `{"pwm": ${element.id}, "value":${value * aFactor}}`;
    if (logger) console.log('tune ' + element.id + ' - ' + value);
    websocket.send(msg);
  }
}
// function avar(element) is called by the HTML when one analog variable is set by the user
// Send message (object: {"avar":"x", "value":"xx"}) using websocket to the server to change value of that variable.
if (useAVAR) {
  function avar(element, value) {
    const msg = `{"avar": "${element.id}", "value":${value * aFactor}}`;
    if (logger) console.log('avar ' + element.id + ' - ' + value);
    websocket.send(msg);
  }
}

//===============================================================================
// MANAGE MESSAGES FROM ESP SERVER (function "notifyClients" in main.cpp)
//===============================================================================
// Update feedback of affected element:
function onMessage(event) {
  if (logger) { console.log('feedback ' + event.data); }
  let element = '';  
  //------------------------------------------------------
  // BUTTONS "element.id"
  //------------------------------------------------------
  // message received: "ON" / "OFF" / "AUTO" / "MAN"
  if (useButton && arrButton.includes(event.data)) {
    if (event.data == 'ON' || event.data == 'OFF')  element = 'state';
    else element = 'mode';
    document.getElementById(element).textContent = event.data;
    if (event.data == 'ON' || event.data == 'AUTO')  document.getElementById(element).style.color = colorON;
    else document.getElementById(element).style.color = colorOFF;
  }

  //------------------------------------------------------
  // DIGITAL FEEDBACK "dfb" from TOGGLE SWITCHES
  //------------------------------------------------------
  // Update one toggle switch using JSON object with its current Output State:
  // { "dfb":"12", "state":"0" }
  else if ((useBVAR || useToggle) && 'dfb' in JSON.parse(event.data)) {
    const jsonObj = JSON.parse(event.data);
    element = jsonObj.dfb;
    if (jsonObj.state == "1") {
      document.getElementById(element).checked = true;
      document.getElementById(element+"_state").textContent = "ON";
      document.getElementById(element+"_state").style.color = colorON;    
    }
    else {
      document.getElementById(element).checked = false;
      document.getElementById(element+"_state").textContent = "OFF";
      document.getElementById(element+"_state").style.color = colorOFF;
    }
  }

  //------------------------------------------------------
  // ANALOG FEEDBACK "afb" from PWM's and Control Variables
  //------------------------------------------------------
  // Update analog feedback for PWM or Control variable (slider bar or text input) using JSON object with its current value:
  // { "afb":"TSET", "value":"22" } or { "afb":"5", "value":"50" }
  else if ((useAVAR || usePWM) && 'afb' in JSON.parse(event.data)) {
    const jsonObj = JSON.parse(event.data);
    element = jsonObj.afb;
    document.getElementById(element).value = jsonObj.value / aFactor;
    document.getElementById(element+"_value").textContent = jsonObj.value / aFactor;
  } 
}

//===============================================================================
// Define and style the slide bars (add ticks and tick values)
//===============================================================================
const arrSliders = [
  { "id":"5", "range": [0, 100], "num": 5},
  { "id":"15", "range": [5, 35], "num": 6},
  { "id":"tSET", "range": [0, 40], "num": 8},
  { "id":"rhSET", "range": [0, 100], "num": 5}   
]

for (i = 0; i < arrSliders.length; i++) {
  const min = (arrSliders[i].range[0]), 
        max = (arrSliders[i].range[1]),
        step = (max - min) / 100; 
        numTicks = (arrSliders[i].num);
  const slider = document.getElementById(arrSliders[i].id);
  slider.setAttribute("min", min);
  slider.setAttribute("max", max);
  slider.setAttribute("step", step);
  slider.setAttribute("value", min);

  const axis = document.getElementById(arrSliders[i].id + "_axis");
  for(j = 0; j <= numTicks; j++) {
    const tick = document.createElement("span");
    axis.appendChild(tick);
    tick.classList.add("tick");
    tick.textContent = min + j * (max - min) / numTicks;
    tick.style.transform = "translateY(-10px)";
  }
}
