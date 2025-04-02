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
#include "MicrosdCard.hpp"

//Start Running
void setup() {
  //Start Serial Communication
  Serial.begin(115200);

  //Set up web server
  initWebSetup(); //Function in WebHosting.hpp

  //Set up microSD card
  setupMicroSDcard(); //Function in MicrosdCard.hpp
  
  //Setup modbus for pump communication
  pumpSetup(); //Function in Pump.hpp

  //Begin wire communication
  Wire.begin();
  
  //Set up Flow Sensor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp

  //Set up Stepper Motor
  //stepperSetup(stepper); //Function in StepperMotor.hpp

  setRoutine("TestFinalFinal", 0.001, 0.001, 1, 1);
}

void loop() {
  //refresh data on the website 
  delay(200);
  ws.cleanupClients();
}