/*****************************************************************************************************
* NewMain.cpp
* 
* Function to keep bare neceties of the program running. For testing and to be eventually built out
/*****************************************************************************************************/


#include <Arduino.h>
#include "WiFi.h"
#include <PIDController.h>
#include <stdint.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <TaskScheduler.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <YAAJ_ModbusMaster.h>



//flow sensor commands
int Reset = (0x06);
int LOW_ADDRESS = 0;
int HIGH_ADDRESS = 1;

int TCA_ADDR = 0x70;

void initFlowSensors(){
    int check = 1;

    //Wire.begin();
    ///Wire.beginTransmission(TCA_ADDR);
    //Wire.write(1 << LOW_ADDRESS);
    //Wire.endTransmission();
    
    while (check != 1) {
        Wire.beginTransmission(0x3F);
        Wire.write(0x06);
        check = Wire.endTransmission();
        if (check != 0) {
            Serial.printf("Low flow did not reset");
        }
    }

    /*
    int check = 1;

    Wire.begin();
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << HIGH_ADDRESS);
    Wire.endTransmission();

    while (check != 1) {
        Wire.beginTransmission(0x00);
        Wire.write(Reset);
        check = Wire.endTransmission();
        if (check != 0) {
            Serial.printf("High flow did not reset");
        }
    }
    */
}

/*
void initPump(){

}

void initStepperMotor(){ 

}

void initAllComponets(){

}
*/

void setup() {
    Serial.begin(115200);
    //controller.begin(ModbusSerial, 9600, SERIAL_8N1, MODBUS_RX, MODBUS_TX, PUMP_ADDRESS, MODBUS_ENABLE, MODBUS_TIMEOUT);

}

void loop() {
    //void initFlowSensors();
}