/*****************************************************************************************************
* NewMain.cpp
* 
* Function to keep bare neceties of the program running. For testing and to be eventually built out
/*****************************************************************************************************/


#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "FlowSensor.hpp"

//Other includes older code used
//#include "WiFi.h"
//#include <PIDController.h>
//#include <stdint.h>
//#include <HardwareSerial.h>
//#include <TaskScheduler.h>
//#include <WiFiClient.h>
//#include <WiFiServer.h>
//#include <YAAJ_ModbusMaster.h>

int lowMotorDirPin = 13;
int lowMotorStepPin = 12;
int lowMotorEnaPin = 14;

int highMotorDirPin = 27;
int highMotorStepPin = 26;
int highMotorEnaPin = 25;

void initStepperMotor (int dir, int step, int ena) {

}

//Start Running
void setup() {
    //Start Serial Communication
    Serial.begin(115200);
    Wire.begin();
    
    //Initlize Flow Sensor
    initFlowSensor();
    Serial.println("Measurement started.\n");
    delay(100);
}

void loop() {
    //Loop to read and print flow rate from flow sensor
    float flowRate = readFlowSensor();
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" ml/min\n");
    delay(1000);
}