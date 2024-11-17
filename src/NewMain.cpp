/*****************************************************************************************************
* NewMain.cpp
* 
* Function to keep bare neceties of the program running. For testing and to be eventually built out
/*****************************************************************************************************/


#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>

#include "FlowSensor.hpp"
#include "StepperMotor.hpp"
#include "BioreactorVaribiles.hpp"

//Other includes older code used
//#include "WiFi.h"
//#include <PIDController.h>
//#include <stdint.h>
//#include <HardwareSerial.h>
//#include <TaskScheduler.h>
//#include <WiFiClient.h>
//#include <WiFiServer.h>
//#include <YAAJ_ModbusMaster.h>

//Start Running
void setup() {
    //Start Serial Communication
    Serial.begin(115200);
    Wire.begin();
    
    //Initlize Flow Sensor
    initFlowSensor();
    Serial.println("Measurement started.\n");
    delay(100);

    //Initlize both stepper motors
    initAllStepperMotors(lowMotorDirPin, lowMotorStepPin, lowMotorEnaPin, highMotorDirPin, highMotorStepPin, highMotorEnaPin);
}

void loop() {
    //Loop to read and print flow rate from flow sensor
    float flowRate = readFlowSensor();
    Serial.print("Flow Rate: ");
    Serial.print(flowRateCalc(flowRate));
    Serial.println(" ml/min\n");
    delay(1000);
}