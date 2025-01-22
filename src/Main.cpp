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
#include <WiFi.h>
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

//Pins for Stepper Motor
int HIGH_MOTOR_DIRPIN = 27;
int HIGH_MOTOR_STEPPIN = 26;
int HIGH_MOTOR_ENAPIN = 25;

//Set up Pump controller
YAAJ_ModbusMaster controller;

//Set up Flow Sensor
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

//Set up Stepper Motor varibiles
ESP_FlexyStepper stepper;
const float STEPS_PER_REV = 200;
const float DISTANCE_PER_REV = 1;
const float MAX_SPEED = 1000;
const float ACCELERATION = 500;
const float MOVE_DISTANCE = 5; 

//Declare Functions
void initSPIFFS();
void initWebServer();

//Start Running
void setup() {

    //Start Serial Communication
    Serial.begin(115200);

    //WIFI
    //first parameter is name of access point, second is the password (if used)
    initSPIFFS();
    wifiManager.autoConnect("ESPBio");
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
    //Collect Flow Sensor Data
    int ret = flowSensor.readSample();
    if (SLF3X.isAirInLineDetected()) {
      Serial.print(" [Air in Line Detected]");
    }
    if (ret == 0) {
        Serial.print("Flow: ");
        Serial.print(flowSensor.getFlow(), 2);
        Serial.print(" ml/min");

        Serial.print(" | Temp: ");
        Serial.print(flowSensor.getTemp(), 1);
        Serial.print(" deg C\n");
    } else {
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
    }
    delay(1000);

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