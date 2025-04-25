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
#include <ticker.h>

//all file includes
#include "WebHosting.hpp"
#include "FlowSensor.hpp"
#include "Pump.hpp"
#include "BioreactorVariables.hpp"
#include "StepperMotor.hpp"
#include "Routine.hpp"
#include "MicrosdCard.hpp"
#include "FeedBackControl.hpp"

Ticker rollingAverageUpdater;

bool varPush = false;

float rollingAverageFlow = 0.0;
float currentFlowRate = 0.0; // Variable to store the rolling average flow rate

void updateRollingAverage() {
  currentFlowRate = getRawFlow(flowSensor); // Function to get the current flow rate from the sensor
   Serial.print("Current Flow Rate: ");
   Serial.println(currentFlowRate); // Debug print to verify sensor reading

  rollingAverageFlow = calculateRollingAverage(currentFlowRate);
   Serial.print("Updated Rolling Average Flow: ");
   Serial.println(rollingAverageFlow); // Debug print to verify rolling average update
}

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

    //Set bioreactor vars
    setBioreactorSettings(); //Function in BioreactorVariables.hpp

  //Set up Stepper Motor
  //stepperSetup(stepper); //Function in StepperMotor.hpp

  //Testing routine and adds dummy test to test file
  setRoutine("test", 0, 0, 0, 1);

  rollingAverageUpdater.attach(0.1, updateRollingAverage); // Update rolling average every 0.1 seconds
}

void loop() {
  //delay();
  //yield();

  //Refresh websocket
  ws.cleanupClients();

  //Run the task scheduler
  scheduler.execute();
}