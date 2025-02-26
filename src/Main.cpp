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
#include <Wire.h>

//all file includes
#include "WebHosting.hpp"
#include "FlowSensor.hpp"
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "StepperMotor.hpp"

//Set up Flow Sensor
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

//Start Running
void setup() {
  //Start Serial Communication
  Serial.begin(115200);

  //Setup modbus for pump communication
  pumpSetup(); //Function in Pump.hpp
  
  //Set up web server
  initWebSetup();

  //Begin wire communication
  Wire.begin();
  
  //Set up Flow Sensor and Stepper Motor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp
  stepperSetup(stepper); //Function in StepperMotor.hpp

}

void loop() {
  ws.cleanupClients();

  checkStatus();
  delay(1000);

  // String flowData = readFlowSensor(flowSensor); //Function in FlowSensor.hpp
  // ws.textAll(flowData); //Send data to be handled by webscoket
  // delay(250);

  // stepper.moveRelativeInMillimeters(-MOVE_DISTANCE);
  // while (!stepper.motionComplete()) {
  //     //Do Nothing
  // }
  // Serial.print("Moved Stepper Motor\n");
}