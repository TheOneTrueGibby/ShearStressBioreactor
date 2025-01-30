/*****************************************************************************************************
* NewMain.cpp
* 
* Where all commands/routines are executed
/*****************************************************************************************************/

//All library includes
#include <Arduino.h>
#include <Wire.h>
#include <YAAJ_ModbusMaster.h>

//all file includes
#include "BioreactorVaribiles.hpp"
#include "Pump.hpp"
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"
#include "ESP_FlexyStepper.cpp"
#include "WebHosting.hpp"

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