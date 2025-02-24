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

  // Setup RS485 communication
  pinMode(MODBUS_RE, OUTPUT);
  pinMode(MODBUS_DE, OUTPUT);
  digitalWrite(MODBUS_RE, 0);
  digitalWrite(MODBUS_DE, 0);

  // Initialize ModbusMaster with proper pins for TX, RX, and DE/RE
  Serial2.begin(9600, SERIAL_8N1, MODBUS_RX2, MODBUS_TX2);
  node.begin(PUMP_ADDRESS, Serial2);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  
  //set up web server
  initWebSetup();

  //begin communication
  Wire.begin();

  pumpOn = checkStatus();
  Serial.printf("Pump is: %d\n", pumpOn);
  
  //Set up Flow Sensor and Stepper Motor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp
  delay(100);
  stepperSetup(stepper); //Function in StepperMotor.hpp

}

void loop() {
  ws.cleanupClients();

  // String flowData = readFlowSensor(flowSensor); //Function in FlowSensor.hpp
  // ws.textAll(flowData); //Send data to be handled by webscoket
  // delay(250);

  // stepper.moveRelativeInMillimeters(-MOVE_DISTANCE);
  // while (!stepper.motionComplete()) {
  //     //Do Nothing
  // }
  // Serial.print("Moved Stepper Motor\n");
}