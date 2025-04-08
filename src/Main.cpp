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
#include <Ticker.h>

//all file includes
#include "WebHosting.hpp"
#include "FlowSensor.hpp"
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "StepperMotor.hpp"
#include "Routine.hpp"
#include "MicrosdCard.hpp"

// Create a Ticker object to update the rolling average
Ticker rollingAverageUpdater;

float desiredFlowRate = 100.0; // Setpoint in ml/min
float rollingAverageFlow = 0.0; // Define the rolling average flow rate variable
float currentFlowRate = 0.0; // Variable to hold the current flow rate

// Ensure the rollingAverageFlow variable is updated globally
void updateRollingAverage() {
    float currentFlowRate = readFlowSensor(flowSensor, 0);
    // Serial.print("Current Flow Rate: ");
    // Serial.println(currentFlowRate); // Debug print to verify sensor reading

    rollingAverageFlow = calculateRollingAverage(currentFlowRate);
    // Serial.print("Updated Rolling Average Flow: ");
    // Serial.println(rollingAverageFlow); // Debug print to verify rolling average update
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

  //Begin wire communication
  Wire.begin();
  
  //Set up Flow Sensor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp

  //Set up Stepper Motor
  //stepperSetup(stepper); //Function in StepperMotor.hpp

  //setRoutine("Test", 0.001, 0.001, 1, 1);

  // Start the Ticker to update the rolling average every 100 milliseconds
  rollingAverageUpdater.attach(0.1, updateRollingAverage);
}

void loop() {

  millis();
  float time = millis() / 1000.0; // Get the elapsed time in seconds
  // Refresh data on the website every second

  controlPumpSpeed(desiredFlowRate); // Function in Pump.hpp

  Serial.print(time);
  Serial.print(", ");
  Serial.println(rollingAverageFlow); // Print the elapsed time and rolling average flow rate

  delay(500); // Adjust delay as needed
  ws.cleanupClients();
}