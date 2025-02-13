/*****************************************************************************************************
* NewMain.cpp
* 
* Where all commands/routines are executed
/*****************************************************************************************************/

//All library includes
#include <Arduino.h>
#include <SPI.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Update.h>
#include <ESP_FlexyStepper.h>
#include <Wire.h>

//all file includes
#include "WebHosting.hpp"
#include "ModbusClientRTU.h"
#include "FlowSensor.hpp"

// Set up Pump controller
const int MODBUS_RX = 16;
const int MODBUS_TX = 17;
const int MODBUS_ENABLE = 18; // automatically set to high when writing, low otherwise to receive
const int PUMP_ADDRESS = 0xEF; // Modbus address of pump controller
const int MODBUS_TIMEOUT = 500; // timeout in ms for Modbus command responses
/*******************************************************************************
 * Modbus client (AKA "master") is the ESP device reading and controlling the  *
 * peripheral pump. This communication is established using Modbus RTU; the    *
 * traditional serial-based Modbus communication strategy. Communicating over  *
 * 2-wire RS485 which will only allow one "side" to "talk" at any single       *
 * moment in time. This is commonly referred to as "half-duplex" because each  *
 * side must wait to talk until the other side is finished speaking.           *
 * See more information:    https://bit.ly/4jTW2X1                             *
 * Connection information:  https://github.com/eModbus/eModbus/discussions/388 *
 ******************************************************************************/
ModbusClientRTU RS485(MODBUS_ENABLE);   // for auto half-duplex

//Set up Flow Sensor
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

//Pins for Stepper Motor
int HIGH_MOTOR_DIRPIN = 27;
int HIGH_MOTOR_STEPPIN = 26;
int HIGH_MOTOR_ENAPIN = 25;

//Set up Stepper Motor varibiles
ESP_FlexyStepper stepper;
const float STEPS_PER_REV = 200;
const float DISTANCE_PER_REV = 1;
const float MAX_SPEED = 1000;
const float ACCELERATION = 1000;
const float MOVE_DISTANCE = 5; 

//Modbus
void handleData(ModbusMessage msg, uint32_t token) 
{
  Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", msg.getServerID(), msg.getFunctionCode(), token, msg.size());
  for (auto& byte : msg) {
    Serial.printf("%02X ", byte);
  }
  Serial.println("");
}

void handleError(Error error, uint32_t token) 
{
  // ModbusError wraps the error code and provides a readable error message for it
  ModbusError me(error);
  Serial.printf("Error response: %02X - %s\n", error, (const char *)me);
}

//Start Running
void setup() {
  //Start Serial Communication
  Serial.begin(115200);

  // Set up Serial2 connected to Modbus RTU
  RTUutils::prepareHardwareSerial(Serial2);
  Serial2.begin(19200, SERIAL_8N1);

  // Set up ModbusClientRTU client.
  // - provide onData and onError handler functions
  RS485.onDataHandler(&handleData);
  RS485.onErrorHandler(&handleError);

  // Start ModbusClientRTU background task
  RS485.begin(Serial2);

  //set up web server
  initWebSetup();

  //begin communication
  Wire.begin();

  // //Initialize Modbus communication for the pump
  // controller.begin(ModbusSerial, 9600, SERIAL_8N1, MODBUS_RX, MODBUS_TX, PUMP_ADDRESS, MODBUS_ENABLE, MODBUS_TIMEOUT);

  // //Set up Pump
  // if (pump.checkStatus()) {
  //   Serial.println("Pump initialized and currently running.");
  // } else {
  //   Serial.println("Pump initialized and currently stopped.");
  // }
  
  //Set up Flow Sensor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp

  delay(100);

  //Setup Stepper Motor Paramaters
  stepper.connectToPins(HIGH_MOTOR_STEPPIN, HIGH_MOTOR_DIRPIN);

  stepper.setStepsPerMillimeter(STEPS_PER_REV / DISTANCE_PER_REV);
  stepper.setSpeedInStepsPerSecond(MAX_SPEED);
  stepper.setAccelerationInStepsPerSecondPerSecond(ACCELERATION);

  //stepper.setCurrentPositionInMillimeters(0.0);
  stepper.startAsService(1);
}

void loop() {
  ws.cleanupClients();

  //Set pump speed example
  // if (!pump.checkStatus()) { // Ensure the pump is off before setting speed
  //   if (pump.setSpeed(100)) { // Set to 100 ml/min
  //     Serial.println("Pump speed set to 100 ml/min.");
  //   } else {
  //     Serial.println("Failed to set pump speed.");
  //   }
  // }

  // if(!pump.checkStatus()) {
  //   pump.togglePump();
  // }

  String flowData = readFlowSensor(flowSensor); //Function in FlowSensor.hpp
  ws.textAll(flowData); //Send data to be handled by webscoket
  delay(250);

  //Move stepper motor (works)
  // stepper.moveRelativeInMillimeters(MOVE_DISTANCE);
  // while (!stepper.motionComplete()) {
  //     //Do Nothing
  // }

  // stepper.moveRelativeInMillimeters(-MOVE_DISTANCE);
  // while (!stepper.motionComplete()) {
  //     //Do Nothing
  // }
  // Serial.print("Moved Stepper Motor\n");
}