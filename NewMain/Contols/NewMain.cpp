/*****************************************************************************************************
* NewMain.cpp
* 
* Function to keep bare neceties of the program running. For testing and to be eventually built out
/*****************************************************************************************************/


#include <Arduino.h>
#include <TaskScheduler.h>
#include <Wire.h>
#include <stdint.h>
#include "utils.hpp"

// Pin Definitions
#define FLOW_SENSOR_1_PIN 12
#define FLOW_SENSOR_2_PIN 13
#define PUMP_PIN 14
#define STEPPER_STEP_PIN 15
#define STEPPER_DIR_PIN 16

// Used for sending and receiving Modbus commands
HardwareSerial ModbusSerial(1);

// Receive and transmit pins for the MAX485
const int MODBUS_RX = 16;
const int MODBUS_TX = 17;
const int MODBUS_ENABLE = 18; // automatically set to high when writing, low otherwise to receive

const int PUMP_ADDRESS = 0xEF; // Modbus address of pump controller
const int MODBUS_TIMEOUT = 500; // timeout in ms for Modbus command responses


//flow sensor commands
int SensorAddress = (0x08);
int Reset = (0x06);
int StartWaterMeasurement = (0x3608);
int StartIPAMeasurement = (0x3616);
int StopMeasurement = (0x3FF9);
int LOW_ADDRESS = 0;
int HIGH_ADDRESS = 1;

void initFlowSensors(){
    int check = 1;
    Wire.begin();
    while (check != 1) {
        Wire.beginTransmission(LOW_ADDRESS);
        Wire.write(Reset);
        check = Wire.endTransmission();
        if (check != 0) {
            Serial.printf("Low flow did not reset");
        }
    }

    int check = 1;
    Wire.begin();
    while (check != 1) {
        Wire.beginTransmission(HIGH_ADDRESS);
        Wire.write(Reset);
        check = Wire.endTransmission();
        if (check != 0) {
            Serial.printf("High flow did not reset");
        }
    }
}

void initPump(){

}

void initStepperMotor(){ 

}

void initAllComponets(){

}

int main() {
    void initFlowSensors();
}