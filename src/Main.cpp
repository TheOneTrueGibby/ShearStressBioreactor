/*****************************************************************************************************
Main.cpp
Where we call all functions and run all code
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
#include "Routine.hpp"

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

  //setRoutine(1, 1, 1, 1);
}

void loop() {
  ws.cleanupClients();
  readFlowSensor(flowSensor, 1); //Function in FlowSensor.hpp
  //checkPumpStatus();
  //ws.textAll(flowData); //Send data to be handled by webscoket
  int result = digitalRead(22);
  Serial.println(result);
  //delay(100);
}