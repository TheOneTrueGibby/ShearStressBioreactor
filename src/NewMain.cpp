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


void setup() {
    Serial.begin(115200);

}

void loop() {
    
}