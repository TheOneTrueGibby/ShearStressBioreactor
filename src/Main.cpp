/*****************************************************************************************************
* NewMain.cpp
* 
* Where all commands/routines are executed
/*****************************************************************************************************/

//All library includes
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <YAAJ_ModbusMaster.h>
#include <WiFiManager.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

//all file includes
#include "BioreactorVaribiles.hpp"
#include "Pump.hpp"
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"
#include "ESP_FlexyStepper.cpp"

//Setup Wifi Manager and Server
WiFiManager wifiManager;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//Set up Pump controller
HardwareSerial ModbusSerial(1);
const int PUMP_ADDRESS = 0xEF; // Modbus address of pump controller
const int MODBUS_TIMEOUT = 500; // timeout in ms for Modbus command responses
// extern YAAJ_ModbusMaster controller;
// static Pump* pump;
// pump = Pump(controller);

//Set up Flow Sensor
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

//Pins for Stepper Motor
int HIGH_MOTOR_DIRPIN = 27;
int HIGH_MOTOR_STEPPIN = 26;
int HIGH_MOTOR_ENAPIN = 25;

//Set up Stepper Motor varibiles
ESP_FlexyStepper stepper;
const float STEPS_PER_REV = 200;
const float DISTANCE_PER_REV = 1;
const float MAX_SPEED = 1000;
const float ACCELERATION = 1000;
const float MOVE_DISTANCE = 5; 

//Declare Functions
void initSPIFFS();
void initWebServer();
void initWebServer();
void initWebSocket();
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

//Start Running
void setup() {
  //Start Serial Communication
  Serial.begin(115200);

  //WIFI
  //first parameter is name of access point, second is the password (if used)
  initSPIFFS();
  wifiManager.autoConnect("ESPBio");
  initWebSocket();
  initWebServer();

  //begin communication
  Wire.begin();
  
  //Set up Flow Sensor
  uint16_t reset = flowSensor.init();
  if (reset != 0) {
      Serial.print("Error initializing the flow sensor: ");
      Serial.println(reset);
      return;
  }
  Serial.println("Flow sensor initialized.");

  delay(100);

  //Setup Stepper Motor Paramaters
  stepper.connectToPins(HIGH_MOTOR_STEPPIN, HIGH_MOTOR_DIRPIN);

  stepper.setStepsPerMillimeter(STEPS_PER_REV / DISTANCE_PER_REV);
  stepper.setSpeedInStepsPerSecond(MAX_SPEED);
  stepper.setAccelerationInStepsPerSecondPerSecond(ACCELERATION);

  //stepper.setCurrentPositionInMillimeters(0.0);
  stepper.startAsService(1);
}

void loop() {
  ws.cleanupClients();

  //Collect Flow Sensor Data
  int ret = flowSensor.readSample();
  // if (SLF3X.isAirInLineDetected()) {
  //   Serial.print(" [Air in Line Detected]");
  // }
  String flowData = "";
  if (ret == 0) {
    //Print flow to terminal
    Serial.print("Flow: ");
    Serial.print(flowSensor.getFlow(), 2);
    Serial.print(" ml/min");

    //Put flow data to webserver
    flowData += "Flow: ";
    flowData += String(flowSensor.getFlow(), 2) + " ml/min";

    //Print temp to terminal
    Serial.print(" | Temp: ");
    Serial.print(flowSensor.getTemp(), 1);
    Serial.print(" deg C\n");

    //Put temp data to webserver
    flowData += " | Temp: ";
    flowData += String(flowSensor.getTemp(), 1) + " deg C";
  } else {
    Serial.print("Error in flowsensor.readSample(): ");
    Serial.println(ret);
    flowData = "Error in flowsensor.readSample(): " + String(ret);
  }
  ws.textAll(flowData);
  delay(100);

  //Move stepper motor
  stepper.moveRelativeInMillimeters(MOVE_DISTANCE);
  while (!stepper.motionComplete()) {
      //Do Nothing
  }

  stepper.moveRelativeInMillimeters(-MOVE_DISTANCE);
  while (!stepper.motionComplete()) {
      //Do Nothing
  }
  Serial.print("Moved Stepper Motor\n");
}

//set up to host website
//https://m1cr0lab-esp32.github.io/remote-control-with-websocket/web-server-setup/
void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
  }
}

void onRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html", false);
}

void initWebServer() {
  server.on("/", onRootRequest);
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

  switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
  }
}