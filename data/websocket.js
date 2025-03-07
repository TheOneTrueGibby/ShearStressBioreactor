/************************************************************************
Gibson Moseley - websocket.js

handles webscoket data handling for website side
helped with tutorial: https://m1cr0lab-esp32.github.io/remote-control-with-websocket/
*************************************************************************/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen  = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

//This allows string varibiles to be sent to the websever in the format of "[varibleHTMLName]; data you want to send"
//Example: "flowdata; Flow is not set" is broken into varName = flowdata, value = "Flow is not set"
function onMessage(event) {
    console.log('Received: ', event.data);

    let [variableName, value] = event.data.split(';');
    
    variableName = variableName.trim();
    value = value.trim();

    let element = document.getElementById(variableName);
    if (element) {
        element.innerText = value;
    }
    //document.getElementById('flowData').innerText = data.flowData;
    //document.getElementById('pumpStatus').innerText = data.pumpStatus;
}