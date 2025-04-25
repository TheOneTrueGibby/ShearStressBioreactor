/************************************************************************
Gibson Moseley - websocket.js

handles webscoket data handling for website side
helped with tutorial: https://m1cr0lab-esp32.github.io/remote-control-with-websocket/
*************************************************************************/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

let csvLines = [];

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

    if (variableName === "csvdata") {
        if (value === "start") {
            csvLines = [];
        } else if (value === "done") {
            drawChart(csvLines);
        } else {
            csvLines.push(value.trim());
        }
        return;
    } 
    else {
        variableName = variableName.trim();
        value = value.trim();

        let element = document.getElementById(variableName);
        if (element) {
            element.innerText = value;
        }
    }
}

//Draw the routine graph based on data sent
function drawChart(csvData) {
    let labels = [];
    let flowData = [];

    // Skip header (first line)
    csvData.slice(1).forEach(line => {
        let parts = line.split(',');
        if (parts.length >= 3) {
            let time = parts[1].trim();  // Time column
            let flow = parseFloat(parts[2]); // Flow (ml/min)

            if (!isNaN(flow)) {
                labels.push(time);
                flowData.push(flow);
            }
        }
    });

    const ctx = document.getElementById('dataChart').getContext('2d');
    if (window.myChart) {
        window.myChart.destroy(); // Prevent multiple charts layering
    }

    window.myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                label: 'Flow Rate (ml/min)',
                data: flowData,
                borderColor: 'blue',
                borderWidth: 2,
                fill: false,
                tension: 0.1
            }]
        },
        options: {
            scales: {
                x: { title: { display: true, text: 'Time (hh:mm:ss)' } },
                y: { title: { display: true, text: 'Flow (ml/min)' } }
            }
        }
    });
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
   // var routine = String(routine);

    //Create a message string to send to the ESP32
    var message = "routine" + ";" + routineName + ";" + shearStress + ";" + runTime + ";" + breakTime + ";" + repetitions;
    //var messageSend = "routine;" + message;

    //Send the message to the WebSocket server
    websocket.send(message);

    console.log('Sending data:', message);
}

function onSubmitSettings(event) {
    //Prevent form from reloading the page
    event.preventDefault();

    //Gather the values from the form
    var channelHeight = document.getElementById('channelHeightValue').value.trim();
    var channelWidth = document.getElementById('channelWidthValue').value.trim();
    var MU = document.getElementById('MUValue').value.trim();
    var RHO = document.getElementById('RHOValue').value.trim();
    
    //Create a message string to send to the ESP32
    var message = "settings" + ";" + channelHeight + ";" + channelWidth + ";" + MU + ";" + RHO;
    //var messageSend = "settings;" + message;

    //Send the message to the WebSocket server
    websocket.send(message);

    console.log('Sending data:', message);
}