ESP8266 web server using websocket to handle messages with Javascript

The ESP8266 will control several digital and analog outputs. 
The user can operate buttons, toggle switches and sliders in the web page. 
Javascript functions will handle the user actions and send "WebSocket" messages to the server. 
The server will operate the outputs or change variables as requested by the messages received. 
The server will send feedback messages to the Javascript file. 
The Javascript file will update the content of the web page with the feedback received from the server.

This version is created using the official library Arduino_JSON. 