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

  //Setup modbus for pump communication
  //pumpSetup(); //Function in Pump.hpp
  
  //Set up web server
  initWebSetup();
  setupMicroSDcard();

  //Begin wire communication
  Wire.begin();
  
  //Set up Flow Sensor, Stepper Motor, & MicroSD
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp
  // stepperSetup(stepper); //Function in StepperMotor.hpp

  //setRoutine("Test", 0.01, 0.001, 0.3, 1);
}

void loop() {
  delay(200);
  //ws.cleanupClients();
}