/************************************************************************
Gibson Moseley  - Pump.hpp
All functions to setup and control the pump
Some code used from last years team - Carson Sloan (pump.hpp/pump.cpp)
*************************************************************************/

#ifndef PUMP_HPP
#define PUMP_HPP

#include <ModbusMaster.h>

//#include "WebHosting.hpp"

ModbusMaster node;

// Set up Pump controller
const int MODBUS_RX2 = 16;
const int MODBUS_TX2 = 17;
const int MODBUS_DE = 14;
const int MODBUS_RE = 14;
//const int MODBUS_ENABLE = 19; // automatically set to high when writing, low otherwise to receive
const int PUMP_ADDRESS = 0xEF; // Modbus address of pump controller

// Pump speeds in ml/min above which the precision of the pump decreases by a factor of 2
const int STEP_0 = 8;
const int STEP_1 = 16;
const int STEP_2 = 32;
const int STEP_3 = 64;
const int STEP_4 = 128;
const int STEP_5 = 256;

// Register values corresponding to the above speeds
const uint16_t STEP_0_CMD = 0x4100;
const uint16_t STEP_1_CMD = 0x4180;
const uint16_t STEP_2_CMD = 0x4200;
const uint16_t STEP_3_CMD = 0x4280;
const uint16_t STEP_4_CMD = 0x4300;
const uint16_t STEP_5_CMD = 0x4380;
const uint16_t STEP_6_CMD = 0x43C8;

// Number of register value increments to change pump speed by 1 ml/min
const int RATE_0 = 16;      // 1 / 0.0625 ml/min
const int RATE_1 = 8;       // 1 / 0.125
const int RATE_2 = 4;       // 1 / 0.25
const int RATE_3 = 2;       // 1 / 0.5
const int RATE_4 = 1;       // 1 / 1
const double RATE_5 = 0.5;  // 1 / 2

bool pumpOn;

void preTransmission()
{
  digitalWrite(MODBUS_RE, 1);
  digitalWrite(MODBUS_DE, 1);
}

void postTransmission()
{
  digitalWrite(MODBUS_RE, 0);
  digitalWrite(MODBUS_DE, 0);
}

//this sets all necessary paramaters in order to communicate with the pump
void pumpSetup() {
    // Setup RS485 communication
    pinMode(MODBUS_RE, OUTPUT);
    pinMode(MODBUS_DE, OUTPUT);
    digitalWrite(MODBUS_RE, 0);
    digitalWrite(MODBUS_DE, 0);

    // Initialize ModbusMaster with proper pins for TX, RX, and DE/RE
    Serial2.begin(9600, SERIAL_8N1, MODBUS_RX2, MODBUS_TX2);
    node.begin(PUMP_ADDRESS, Serial2);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
}

//This sets the pumpOn global varibile based on the pump status and returns a string with the data if pump is on or off
String checkPumpStatus(bool printSerial) {
    //delay(100);

    //string to store pump status and anothe rone to send to the website
    String pumpStatus = "";
    String pumpStatusWebsite = "";

    //Read coils that return pump status, if we can't read then sned that to website and print to terminal if set
    if (node.readCoils(0x1001, 1) == 0) {

        //Store the repsonse buffer (which is pump status)
        uint16_t state = node.getResponseBuffer(0);

        //Based on buffer print/store if pump is on or off
        if(state == 1) {
            pumpStatus += "Pump status: On";
            if(printSerial == 1) {
                Serial.printf("Pump status: On\n");
            }
        } 
        else if (state == 0) {
            pumpStatus += "Pump status: Off";
            if(printSerial == 1) {
                Serial.printf("Pump status: Off\n");
            }
        }

        //set pump varibile to current state
        pumpOn = state;
    }
    else {
        pumpStatus += "Pump status: Unknown";
        if (printSerial == 1) {
            Serial.println("Error: Unable to read pump state!");
        }
    }

    //Add the website varibile name so website displays pump data
    pumpStatusWebsite = "pumpStatus; " + pumpStatus;
    ws.textAll(pumpStatusWebsite);

    //return the pump status string
    return pumpStatus;
}

//This sets the pump to on (1) or off (0)
bool setPump(bool option) {
    //make sure the pumpOn varibile is correct
    checkPumpStatus(0);

    //check if the option is diffrent then current state
    if (pumpOn != option) {
        //set pump on to the option
        pumpOn = option;

        //Write the option to the correct regestry and if unable to set pumpOn back
        uint16_t result = node.writeSingleCoil(0x1001, pumpOn ? 0xFF : 0x00);
        if (result != 0) {
            // Serial.printf("Unable to switch pump state! Error code: %d\n", result);
            pumpOn = !option;
        }
    }

    //return current status
    return pumpOn;
}

//This function is used to write the speed to the right registers
bool setPumpSpeed(uint16_t high, uint16_t low, bool start/* = false*/) {
    node.setTransmitBuffer(0, low);
    node.setTransmitBuffer(1, high);
    int result = node.writeMultipleRegisters(0x3001, 0x02);

    if (start) {
        setPump(true);
    }

    if (result != 0) {
        // Serial.printf("Error (%d) setting flow rate!\n", result);
        return false;
    }
    else {
        return true;
    }
}

//This function sets the speed to the pump to match the requested flow rate in m/min
bool setPumpSpeed(int flow, bool force) {
    // The pump will ignore speed commands when running
    if (pumpOn) {
        if (force) {
            setPump(true);
        }
        else {
            // Serial.println("Error: Attempt to set pump speed while running.");
            return false;
        }
    }

    // Constrain values to within the pump range (although the pump controller does this as well for 0.1-420ish ml/min)
    if (flow < 8) {
        flow = 8;
    }
    else if (flow > 400) {
        flow = 400;
    }

    uint16_t low = 0;
    uint16_t high = 0;

    // TODO: currently only integer flow rates are possible, but the high bytes can be used to 
    // achieve decimal values, following the calculations done for flow rates over 256 ml/min

    // TESTING: Non-integer flow rates

    // Start with the known register value at the edge of the precision level, then add the needed steps
    int integerPart = flow;
    float fractionalPart = flow - integerPart;

    // Start with the known register value at the edge of the precision level, then add the needed steps
    if (flow <= STEP_1) {
        low = STEP_0_CMD + ((integerPart - STEP_0) * RATE_0);
    }
    else if (flow > STEP_1 && flow <= STEP_2) {
        low = STEP_1_CMD + ((integerPart - STEP_1) * RATE_1);
    }
    else if (flow > STEP_2 && flow <= STEP_3) {
        low = STEP_2_CMD + ((integerPart - STEP_2) * RATE_2);
    }
    else if (flow > STEP_3 && flow <= STEP_4) {
        low = STEP_3_CMD + ((integerPart - STEP_3) * RATE_3);
    }
    else if (flow > STEP_4 && flow <= STEP_5) {
        low = STEP_4_CMD + ((integerPart - STEP_4) * RATE_4);
    }
    else if (flow > STEP_5) {
        low = STEP_5_CMD + (int)((integerPart - STEP_5) * RATE_5);
    }

    // Encode the fractional part into the high byte
    if (fractionalPart > 0) {
        high = (uint16_t)(fractionalPart * 0x8000); // Scale fractional part to high byte
    }

    return setPumpSpeed(high, low, force);
}
//Returns current pump speed
int32_t getPumpSpeed() {
    static int32_t lastPumpSpeed = 0; // Store the last valid pump speed

    // Read 2 registers starting at 0x3005 (real-time speed high and low bytes)
    uint16_t result = node.readHoldingRegisters(0x3005, 2);
    if (result == 0) {
        // Combine high and low bytes into a 32-bit integer
        uint32_t rawValue = ((uint32_t)node.getResponseBuffer(0) << 16) | node.getResponseBuffer(1);

        // Interpret the raw value as an IEEE 754 float
        float* pumpSpeed = (float*)&rawValue;

        // Update the last valid pump speed
        lastPumpSpeed = (int32_t)(*pumpSpeed);

        // Print debug information
        //Serial.printf("Pump Speed (float): %.2f\n", *pumpSpeed);
        return lastPumpSpeed;
    } else {
        // Print error information
        // Serial.printf("Error reading pump speed! Error code: %d\n", result);
        return lastPumpSpeed; // Return the last valid pump speed instead of -1
    }
}

#endif