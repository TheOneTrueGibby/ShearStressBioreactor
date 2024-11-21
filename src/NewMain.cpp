/*****************************************************************************************************
* NewMain.cpp
* 
* Where all commands/routines are executed
/*****************************************************************************************************/


#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <YAAJ_ModbusMaster.h>

#include "FlowSensor.hpp"
#include "StepperMotor.hpp"
#include "BioreactorVaribiles.hpp"
#include "Pump.hpp"

YAAJ_ModbusMaster controller;

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
    //test opening and closing each stepper motor
    openStepperMotor(1, lowMotorDirPin);
    closeStepperMotor(1, lowMotorDirPin);
    openStepperMotor(1, highMotorDirPin);
    closeStepperMotor(1, highMotorDirPin);

    //check motor state and set speed
    initPump(controller);
    checkPump(controller);
    //setPumpSpeed(100, controller);
}

void loop() {
    //Loop to read and print flow rate from flow sensor
    float flowRate = readFlowSensor();
    Serial.print("Flow Rate Unchanged: ");
    Serial.print(flowRate);
    Serial.print("\n");
    Serial.print("Flow Rate: ");
    Serial.print(flowRateCalc(flowRate));
    Serial.println(" ml/min\n");
    delay(1000);
}