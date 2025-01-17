/*****************************************************************************************************
* NewMain.cpp
* 
* Where all commands/routines are executed
/*****************************************************************************************************/


#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <YAAJ_ModbusMaster.h>
//#include <Stepper.h>

//#include "FlowSensor.hpp"
//#include "StepperMotor.hpp"
#include "BioreactorVaribiles.hpp"
#include "Pump.hpp"
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"
#include "ESP_FlexyStepper.cpp"


int lowMotorDirPin = 13;
int lowMotorStepPin = 12;
int lowMotorEnaPin = 14;
int highMotorDirPin = 27;
int highMotorStepPin = 26;
int highMotorEnaPin = 25;
#define stepsPerRevolution 5

YAAJ_ModbusMaster controller;
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);
//Stepper lowStepper(stepsPerRevolution, 13, 12, 14, 15);
ESP_FlexyStepper stepper;

//Start Running
void setup() {
    //Start Serial Communication
    Serial.begin(115200);
    Wire.begin();
    
    // Initlize Flow Sensor
    // initFlowSensor();
    // Serial.println("Measurement started.\n");
    // delay(100);

    uint16_t reset = flowSensor.init();
    if (reset != 0) {
        Serial.print("Error initializing the flow sensor: ");
        Serial.println(reset);
        return;
    }
    Serial.println("Flow sensor initialized.");

    delay(100);

    //Initlize both stepper motors
    //initAllStepperMotors(lowMotorDirPin, lowMotorStepPin, lowMotorEnaPin, highMotorDirPin, highMotorStepPin, highMotorEnaPin);
    //initStepperMotor(highMotorDirPin, highMotorStepPin, highMotorEnaPin);
    //test opening and closing each stepper motor
    //openStepperMotor(23, lowMotorDirPin);
    //closeStepperMotor(23, lowMotorDirPin);

    stepper.connectToPins(highMotorStepPin, highMotorDirPin);


    //openStepperMotor(23, 27);
    //closeStepperMotor(23, 27);

    //check motor state and set speed
    //initPump(controller);
    //checkPump(controller);
    //setPumpSpeed(100, controller);
}

void loop() {
    int ret = flowSensor.readSample();
    if (SLF3X.isAirInLineDetected()) {
      Serial.print(" [Air in Line Detected]");
    }
    if (ret == 0) {
        Serial.print("Flow: ");
        Serial.print(flowSensor.getFlow(), 2);
        Serial.print(" ml/min");

        Serial.print(" | Temp: ");
        Serial.print(flowSensor.getTemp(), 1);
        Serial.print(" deg C\n");
    } else {
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
    }

    //Loop to read and print flow rate from flow sensor
    // float flowRate = readFlowSensor();
    // Serial.print("Flow Rate Unchanged: ");
    // Serial.print(flowRate);
    // Serial.print("\n");
    // Serial.print("Flow Rate: ");
    // Serial.print(flowRateCalc(flowRate));
    // Serial.println(" ml/min\n");
    delay(1000);
}