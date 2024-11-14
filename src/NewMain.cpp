/*****************************************************************************************************
* NewMain.cpp
* 
* Function to keep bare neceties of the program running. For testing and to be eventually built out
/*****************************************************************************************************/


#include <Arduino.h>
#include <Wire.h>

//Other includes older code used
#include "WiFi.h"
#include <PIDController.h>
#include <stdint.h>
#include <HardwareSerial.h>
#include <TaskScheduler.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <YAAJ_ModbusMaster.h>



//Code for talking to flow sensor
static const float   SLF3X_SCALE_FACTOR_FLOW = 500.0;
static const float   SLF3X_SCALE_FACTOR_TEMP = 200.0;
static const uint8_t SENSOR_I2C_ADDRESS = 0x08;
static const uint8_t  SOFT_RESET_I2C_ADDRESS = 0x0;
static const uint8_t  CMD_SOFT_RESET = 0x06 ;
static const uint8_t  CMD_START_MESUREMENT = 0x3608;
static const uint8_t  CMD_READ_MESUREMENT = 0xEFC8;
static const uint8_t  SOFT_RESET_MAX_TRIES = 10;
static const uint8_t  CHIP_RESET_DELAY = 100;
static const uint16_t CHIP_RESET_RETRY_DELAY = 500;

int softResetFlowSensor() {
    int ret = 1;
    int failures = 0;
    
    while (ret != 0) {
        Wire.beginTransmission(SOFT_RESET_I2C_ADDRESS);
        Wire.write(CMD_SOFT_RESET);
        ret = Wire.endTransmission();

        if (ret != 0) {
            if (failures > CHIP_RESET_RETRY_DELAY) {
                return ret;
            }

            Serial.printf("Error while soft reseting flow sensor");
            failures++;
            delay(CHIP_RESET_DELAY);
        }
    }
}

void startMesuremntFlowSensor() {
    Wire.beginTransmission(SENSOR_I2C_ADDRESS);
    Wire.write(CMD_START_MESUREMENT);
    Wire.endTransmission();
}

int readFlowSensor() {
    Wire.beginTransmission(SENSOR_I2C_ADDRESS);
    Wire.write(CMD_READ_MESUREMENT);
    Wire.endTransmission();
    
    Wire.requestFrom(SENSOR_I2C_ADDRESS, 6);
    
    uint16_t flowData = (Wire.read() << 8); 
    flowData |= Wire.read();
    uint8_t crcFlow = Wire.read();
    
    float flowRate = (float)flowData * 1000;
    
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

    initFlowSensor();
    Serial.println("Measurement started.");
    delay(100);

}

void loop() {
    float flowRate = readFlowSensor();
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" ml/min");\
    delay(1000);
}