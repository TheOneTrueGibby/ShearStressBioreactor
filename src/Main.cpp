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
#include <TaskScheduler.h>

//all file includes
#include "WebHosting.hpp"
#include "FlowSensor.hpp"
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "StepperMotor.hpp"
#include "Routine.hpp"
#include "MicrosdCard.hpp"

bool varPush = false;

//Start Running
void setup() {
  //Start Serial Communication
  Serial.begin(115200);

  //Set up web server
  initWebSetup(); //Function in WebHosting.hpp
  
  //Setup modbus for pump communication
  pumpSetup(); //Function in Pump.hpp
  setPump(0); //Function in Pump.hpp

  //Set up microSD card
  setupMicroSDcard(); //Function in MicrosdCard.hpp

  //Setup TaskScheduler
  scheduler.init();

  //Begin wire communication
  Wire.begin();
  
  //Set up Flow Sensor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp

  //Set up Stepper Motor
  //stepperSetup(stepper); //Function in StepperMotor.hpp

  //Testing routine and adds dummy test to test file
  setRoutine("test", 0, 0, 0, 1);
}

void loop() {
  //delay();
  //yield();

  //Refresh websocket
  ws.cleanupClients();

  //Run the task scheduler
  scheduler.execute();
}