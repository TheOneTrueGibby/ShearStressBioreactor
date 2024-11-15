/*****************************************************************************************************
* NewMain.cpp
* 
* Function to keep bare neceties of the program running. For testing and to be eventually built out
/*****************************************************************************************************/


#include <Arduino.h>
#include <Wire.h>

//Other includes older code used
//#include "WiFi.h"
//#include <PIDController.h>
//#include <stdint.h>
//#include <HardwareSerial.h>
//#include <TaskScheduler.h>
#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WiFiServer.h>
//#include <YAAJ_ModbusMaster.h>



//Code for talking to flow sensor
static const float   SLF3X_SCALE_FACTOR_FLOW = 500.0;
static const float   SLF3X_SCALE_FACTOR_TEMP = 200.0;
static const uint8_t SENSOR_I2C_ADDRESS = 0x08;
static const uint8_t  SOFT_RESET_I2C_ADDRESS = 0x0;
static const uint8_t  CMD_SOFT_RESET = 0x06 ;
static const uint16_t  CMD_START_MESUREMENT = 0x3608;
static const uint16_t  CMD_STOP_MESUREMENT = 0x3FF9;
static const uint16_t  CMD_READ_MESUREMENT = 0xEFC8;
static const uint8_t  SOFT_RESET_MAX_TRIES = 10;
static const uint8_t  CHIP_RESET_DELAY = 100;
static const uint16_t CHIP_RESET_RETRY_DELAY = 500;

void softResetFlowSensor() {
    int ret = 1;
    int failures = 0;
    
    while (ret != 0) {
        Wire.beginTransmission(SOFT_RESET_I2C_ADDRESS);
        Wire.write(CMD_SOFT_RESET);
        ret = Wire.endTransmission();

        if (ret != 0) {
            if (failures > CHIP_RESET_RETRY_DELAY) {
                return;
            }

            Serial.printf("Error while soft reseting flow sensor\n");
            failures++;
            delay(CHIP_RESET_DELAY);
        } else {
            Serial.printf("Soft reset successful\n");
        }
    }
}

void startMesuremntFlowSensor() {
    int check = 1;

    Wire.beginTransmission(SENSOR_I2C_ADDRESS);
    Wire.write(0x36); //tell sensor its water
    Wire.write(0x08);
    Wire.write(CMD_START_MESUREMENT);
    check = Wire.endTransmission();
    if (check != 0) {
        Serial.printf("Error while starting measurement\n");
    }
}

void stopMesuremntFlowSensor() {
    Wire.beginTransmission(SENSOR_I2C_ADDRESS);
    Wire.write(CMD_STOP_MESUREMENT);
    
    int check = Wire.endTransmission();
    if (check != 0) {
        Serial.printf("Error while stoping measurement\n");
    }
}

/**
 * Reads the flow rate from the flow sensor.
 * 
 * This function sends a command to the flow sensor to read the current flow measurement,
 * retrieves the raw flow data, and converts it to a flow rate in ml/min.
 * 
 * @return The current flow rate in ml/min.
 */
int readFlowSensor() {
    int check = 1;

    //Wire.beginTransmission(SENSOR_I2C_ADDRESS);
    //Wire.write(CMD_READ_MESUREMENT);
    //check = Wire.endTransmission();
    //if (check != 0) {
        //Serial.printf("Error while reading sensor\n");
    //}
    
    int i = 9;
    Wire.requestFrom((uint8_t)SENSOR_I2C_ADDRESS, (uint8_t)i);
    //Wire.requestFrom(0x08, i);

    if (Wire.available() < i) {
        Serial.printf("Error while reading flow measurement\n");
    }
    
    uint16_t flowData = (Wire.read() << 8); 
    flowData |= Wire.read();
    byte crcFlow = Wire.read();

    stopMesuremntFlowSensor();
    
    int16_t flowRate = (int16_t) flowData;
    
    return flowRate;
}

void initFlowSensor() {
    Wire.begin();
    softResetFlowSensor();
    startMesuremntFlowSensor();
}


//Start Running
void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    initFlowSensor();
    Serial.println("Measurement started.\n");
    delay(100);
}

void loop() {
    float flowRate = readFlowSensor();
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" ml/min\n");
    delay(1000);
}