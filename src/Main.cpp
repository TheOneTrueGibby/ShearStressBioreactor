/*****************************************************************************************************
* NewMain.cpp
* 
* Where all commands/routines are executed
/*****************************************************************************************************/

//All library includes
#include <Arduino.h>
#include <SPI.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Update.h>
#include <ESP_FlexyStepper.h>
#include <Wire.h>

//all file includes
#include "WebHosting.hpp"
//#include "ModbusClientRTU.h"
#include "FlowSensor.hpp"
#include "Pump.hpp"

// Set up Pump controller
const int MODBUS_RX = 16;
const int MODBUS_TX = 17;
const int MODBUS_ENABLE = 18; // automatically set to high when writing, low otherwise to receive
const int PUMP_ADDRESS = 0xEF; // Modbus address of pump controller
const int MODBUS_TIMEOUT = 500; // timeout in ms for Modbus command responses

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
  node.begin(2, Serial);

  //set up web server
  initWebSetup();

  //begin communication
  Wire.begin();

  bool pumpState = checkStatus();
  Serial.printf("Pump is: %d", pumpState);
  
  //Set up Flow Sensor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp

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

  String flowData = readFlowSensor(flowSensor); //Function in FlowSensor.hpp
  ws.textAll(flowData); //Send data to be handled by webscoket
  delay(250);

  //Move stepper motor (works)
  // stepper.moveRelativeInMillimeters(MOVE_DISTANCE);
  // while (!stepper.motionComplete()) {
  //     //Do Nothing
  // }

  // stepper.moveRelativeInMillimeters(-MOVE_DISTANCE);
  // while (!stepper.motionComplete()) {
  //     //Do Nothing
  // }
  // Serial.print("Moved Stepper Motor\n");
}