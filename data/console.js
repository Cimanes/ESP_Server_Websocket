// Initiate a WebSocket connection with the server when the web interface is fully loaded in the browser
// Receive requests from user on the web page (press buttons / toggle switches / slider bars...)
// Transfer those requests to the server via Websocket messages.
// Receive feedback from server in Websocket messages.
// Process that feedback and update data and style in the web page accordingly.

//===============================================================================
// OPTIONS
//===============================================================================
const logger = 1,                                   // 1 if console log is desired / 0 if not
      colorON = "#048",                             // Color for feedback text when "ACTIVE"
      colorOFF = "#888",                            // Color for feedback text when "INACTIVE"
      useButton = 1,                                // 1 if buttons are used / 0 if not
      arrButton = ["ON", "OFF", "AUTO", "MAN"],     // update with button commands
      useToggle = 1,                                // 1 if toggle switches are used / 0 if not
      arrToggle = ["12", "14"],                     // update with GPIO's used for toggles
      usePWM = 1,                                   // 1 if PWM analog outputs are used / 0 if not
      useAVAR = 1,                                  // 1 if analog control variables are used / 0 if not
      useBVAR = 1,                                  // 1 if boolean control variables are used / 0 if not
      decimals = 1,                                 // Define number of decimals for analog readings
      nSteps = 100,                                 // Number of steps for sliders
      aFactor = 10 ** decimals;                     // Factor for conversion of analog values (server uses int; factor = 10 -> one decimal place)
      
//===============================================================================
// Define and style the slide bars (set id, initial low value, add ticks and tick values)
//===============================================================================
const arrSliders = [
  { id: 13, val: [0, 100], hue: [180, 0], num: 5 },
  { id: 15, val: [200, 0], hue: [180, 0], num: 5 },
  { id: "tSET", val: [5, 35], hue: [180, 0], num: 6},
  { id: "rhSET", val: [0, 100], hue: [180, 0], num: 5 }
];

// Function to range "hsl" angle (hue color) as proportion between slider values and color values.
// Used to dynamically change slider color as a function of slider value
function angle(a, hueLow, hueHigh, valLow, valHigh) {
  return hueLow + (a - valLow) * (hueHigh - hueLow) / (valHigh - valLow);
}

for (const item of arrSliders) {
  const {id, val, hue, num} = item;              // destructuring each object.
 
  const slider = document.getElementById(id),
        step = (Math.max(...val) - Math.min(...val)) / nSteps;
  slider.setAttribute("min", Math.min(...val));
  slider.setAttribute("max", Math.max(...val));
  slider.setAttribute("step", step);
  slider.setAttribute("value", Math.min(...val));
  slider.style.background = "hsl("+ angle(val[0], hue[0], hue[1], val[0], val[1])+", 40%, 50%)";
  if(val[0] > val[1]) slider.style.transform = "rotateY(180deg)";

  for(let j = 0; j <= num; j++) {
    const tick = document.createElement("span");
    document.getElementById(id + "_axis").appendChild(tick);
    tick.className = "tick";
    tick.textContent = Math.round(10 *(val[0] + j * (val[1] - val[0]) / num))/10;
    tick.style.transform = "translateY(-10px)";
  }
}

//===============================================================================
// MANAGE WEBSOCKET
//===============================================================================
// Define the gateway as the current URL using template literal with variable (the webserver IP address)
// Initialize the WebSocket connection on that gateway:
const gateway = `ws://${window.location.hostname}/wsConsole`,
    websocket = new WebSocket(gateway);

// Add an event listener to run function when the page loads:
window.addEventListener("load",  onload());
function  onload(event) { initWebSocket(); }

// Assign callback functions used when the WebSocket connection is opened, closed or when a message is received: 
function initWebSocket() {
  if(logger) console.log("Trying to open WebSocket...");
  websocket.onopen    = onOpen;
  websocket.onmessage = onMessage;
  websocket.onclose   = onClose;
}

// When the websocket is opened:
// Send messages to update feedback from outputs and variables using websocket:
// (Server will reply message with state of ALL Buttons and Toggle Switches).
function onOpen(event) { 
  if(logger) console.log("WebSocket opened: Update all");
  websocket.send(`{"all": ""}`); 
}

// Re-initiate the Websocket if the connection gets lost.
function onClose(event) {
  if(logger) console.log("WebSocket closed");
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
    if (logger) console.log("button " + element.id);
    websocket.send(msg);
  }
}
// function toggle(element) is called by the HTML when one "D.O." element (toggle switch) is operated
// Send message (object: {"tog":"element.id"}) using websocket to the server to toggle D.O. channel number
if (useToggle) {
  function toggle(element) {
    const msg = `{"tog": ${element.id}}`;
    if (logger) console.log("toggle " + element.id);
    websocket.send(msg);
  }
}
// function bvar(element) is called by the HTML when one boolean variable is toggled by the user
// Send message (object: {"bvar":"XX"}) using websocket to the server to toggle the value of that variable.
if (useBVAR) {
  function bvar(element) {
    const msg = `{"bvar": "${element.id}"}`;
    if (logger) console.log("bvar " + element.id);
    websocket.send(msg);
  }
}
// function tune(element) is called by the HTML when one A.O. (PWM) is adjusted
// Send message (object: {"pwm":element.id, "value":##}) using websocket to the server to change value of that variable.

if (usePWM) {
  function tune(element, value) {
    const msg = `{"pwm": ${element.id}, "value":${value * aFactor}}`;
    if (logger) console.log(msg);
    websocket.send(msg);
  }
}
// function avar(element) is called by the HTML when one analog variable is set by the user
// Send message (object: {"avar":"x", "value":##}) using websocket to the server to change value of that variable.
if (useAVAR) {
  function avar(element, value) {
    const msg = `{"avar":"${element.id}", "value":${value * aFactor}}`;
    if (logger) console.log(msg);
    websocket.send(msg);
  }
}

//===============================================================================
// MANAGE MESSAGES FROM ESP SERVER (function "notifyClients" in main.cpp)
//===============================================================================
// Update feedback of affected element:
function onMessage(event) {
  if (logger) { console.log("feedback " + event.data); }
  //------------------------------------------------------
  // BUTTONS "element.id"
  //------------------------------------------------------
  // message received: "ON" / "OFF" / "AUTO" / "MAN"
  if (useButton && arrButton.includes(event.data)) {
    const element = (event.data === "ON" || event.data === "OFF") ? "state" : "mode";
    document.getElementById(element).textContent = event.data;
    document.getElementById(element).style.color = (event.data === "ON" || event.data === "AUTO") ? colorON : colorOFF;
  }

  //------------------------------------------------------
  // DIGITAL FEEDBACK "dfb" from TOGGLE SWITCHES
  //------------------------------------------------------
  // Update one toggle switch using JSON object with its current Output State:
  // { "dfb":"12", "state":"0" }
  else if ((useBVAR || useToggle) && "dfb" in JSON.parse(event.data)) {
    const jsonObj = JSON.parse(event.data);
    document.getElementById(jsonObj.dfb).checked = jsonObj.state == "1";
    document.getElementById(jsonObj.dfb+"_state").textContent = jsonObj.state == "1" ? "ON" : "OFF";
    document.getElementById(jsonObj.dfb+"_state").style.color = jsonObj.state == "1" ? colorON : colorOFF;
  }

  //------------------------------------------------------
  // ANALOG FEEDBACK "afb" from PWM's and Control Variables
  //------------------------------------------------------
  // Update analog feedback for PWM or Control variable (slider bar or text input) using JSON object with its current value:
  // { "afb":"TSET", "value":22 } or { "afb":5, "value":50 }
  else if ((useAVAR || usePWM) && "afb" in JSON.parse(event.data)) {
    const jsonObj = JSON.parse(event.data),
          value = jsonObj.value / aFactor;
    
    document.getElementById(jsonObj.afb).value = value;
    document.getElementById(jsonObj.afb+"_value").textContent = value;
    
    const checkId = (item) => item.id == jsonObj.afb;
    const {id, val, hue, num} = arrSliders.find(checkId);
    document.getElementById(id).style.background = "hsl("+ angle(value, hue[0], hue[1], val[0], val[1])+", 40%, 50%)";

    // for (const item of arrSliders) {
    //   const {id, val, hue, num} = item;              // destructuring each object.
    //   if (id == jsonObj.afb) {
    //     document.getElementById(id).style.background = "hsl("+ angle(value, hue[0], hue[1], val[0], val[1])+", 20%, 50%)";
    //     break;
    //   }
    // }
  } 
}