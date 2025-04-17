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
    initFormSubmit();
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
}

//Function to handle form submission and send data to the ESP32
function initFormSubmit() {
    document.getElementById('submitButton').addEventListener('click', onSubmitRoutine);
    document.getElementById('valueSetButton').addEventListener('click', onSubmitSettings);
}

function onSubmitRoutine(event) {
    //Prevent form from reloading the page
    event.preventDefault();

    //Gather the values from the form
    var routineName = document.getElementById('routineNameValue').value.trim();
    var shearStress = document.getElementById('shearStressValue').value.trim();
    var runTime = document.getElementById('runTimeValue').value.trim();
    var breakTime = document.getElementById('breakTimeValue').value.trim();
    var repetitions = document.getElementById('repeationValue').value.trim();

    //Create a message string to send to the ESP32
    var message = routineName + ";" + shearStress + ";" + runTime + ";" + breakTime + ";" + repetitions;
    var messageSend = "routine;" + message;

    //Send the message to the WebSocket server
    websocket.send(messageSend);

    console.log('Sending data:', message);
}

function onSubmitSettings(event) {
    //Prevent form from reloading the page
    event.preventDefault();

    //Gather the values from the form
    var channelHeight = document.getElementById('channelHeightValue').value.trim();
    var channelHeight = document.getElementById('channelWidthValue').value.trim();
    var channelHeight = document.getElementById('MUValue').value.trim();
    var channelHeight = document.getElementById('RHOValue').value.trim();
    
    //Create a message string to send to the ESP32
    var message = channelHeightValue + ";" + channelWidthValue + ";" + MUValue + ";" + RHOValue;
    var messageSend = "settings;" + message;

    //Send the message to the WebSocket server
    websocket.send(messageSend);

    console.log('Sending data:', message);
}