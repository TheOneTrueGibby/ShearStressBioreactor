/************************************************************************
Gibson Moseley - FlowSensor.hpp

Holds all commands necessry to use the flow sensor, and declerations of the flow sensor
*************************************************************************/

#ifndef FLOWSENSOR_HPP
#define FLOWSENSOR_HPP

//inclusion of necessary files/functions
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"
#include "BioreactorVaribiles.hpp"
#include <deque>
#include <Ticker.h> // Ensure the Ticker library is included

//set up low flow sensor with appropriate varibiles
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

// Rolling average buffer for flow readings
std::deque<float> flowReadings;

const int rollingWindowSize = 25; // Size of the rolling window for averaging

// Declare the Ticker object
extern Ticker rollingAverageUpdater;

//float rollingAverageFlow; // Declare rolling average function

// Function to calculate rolling average
float calculateRollingAverage(float newReading) {
    // Debug print to verify new reading
    // Serial.print("New Reading for Rolling Average: ");
    // Serial.println(newReading);

    // Add the new reading to the buffer
    flowReadings.push_front(newReading);

    float sum = 0.0; // Variable to hold the sum of flow readings

    // Remove the oldest reading if the buffer exceeds the window size
    if (flowReadings.size() > rollingWindowSize) {
        flowReadings.pop_back();
    }

    // Calculate the average
    for (float reading : flowReadings) {
        sum += reading;
    }

    float average = sum / flowReadings.size();
    // Serial.print("Calculated Rolling Average: ");
    // Serial.println(average); // Debug print to verify calculation

    return average;
}

//sets up the flowsensor to be used
void flowSensorSetup(SensirionLF flowSensor) {
    //run the intilizing command and save result
    uint16_t reset = flowSensor.init();

    //if result is zero then error happened otherwise it was reset/setup
    if (reset != 0) {
        Serial.print("Error initializing the flow sensor: ");
        Serial.println(reset);
        return;
    } else {
        Serial.println("Flow sensor initialized.");
    }
}

//reads the provided flowsensor provided and prints out in serial if set to true
//String readFlowSensor(SensirionLF flowSensor, bool printTerminal) {
float readFlowSensor(SensirionLF flowSensor, bool printTerminal) {
    //delay(100);

    //read the flow sensor
    int ret = flowSensor.readSample();

    float flowData = 0.0; // Initialize flowData to 0.0

    //set up strings to hold readings
    // String flowData = "";
    // String flowShear = "";
    // String flowAll = "";

    //if we were able to read flowsensor then we can get temp and flow otherwise there is an error
    if (ret == 0) {

        //get both flow and temp, and calculate shear stress based on flow rate
        //float flowReading = flowSensor.getFlow();
        //float flowTemp = flowSensor.getTemp();
       // float flowShearStress = shearStressCalc(flowReading);

        flowData = flowSensor.getFlow();

        // Calculate rolling average of flow readings
        //float rollingAverageFlow = calculateRollingAverage(flowReading);

        //if set to true, prints readings in terminal
        if (printTerminal == 1) {
            //Print flow and temp  to terminal
            Serial.print("Flow: ");
            Serial.print(flowData, 2);
            Serial.print(" ml/min");

            //Print rolling average flow to terminal
            // Serial.print(" | Rolling Average Flow: ");
            // Serial.print(rollingAverageFlow, 2);
            // Serial.print(" ml/min");

            //Print temp to terminal
            // Serial.print(" | Temp: ");
            // Serial.print(flowTemp, 1);
            // Serial.print(" deg C\n");
        }

        //Put flow data & temp into string varibile
        // flowData += "Flow: ";
        // flowData += String(flowReading, 2) + " ml/min";
        // flowData += ", Rolling Average Flow: ";
        // flowData += String(rollingAverageFlow, 2) + " ml/min";
        // flowData += ", Temp: ";
        // flowData += String(flowTemp, 1) + " deg C";

        // //Put shear stress data into string varibile
        // flowShear += "Shear Stress: ";
        // flowShear += String(flowShearStress) + " Pa";
        // //Serial.print(flowShear);

        // //combine both into one String
        // flowAll += flowData + ", " + flowShear;

        flowData = flowSensor.getFlow();

    } else {
        //if unable to read set string varibile as error message
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
        Serial.print("\n");
       // flowData = "Error in flowsensor.readSample(): " + String(ret);
       //flowData = -1; // Set flowData to -1 to indicate an error
    }

    //send string varibiles to webserver/website through webscoket 
    // String flowDataWebsite = "flowData; " + flowData;
    // String flowShearWebsite = "shearStress; " + flowShear;
    // ws.textAll(flowDataWebsite);
    // ws.textAll(flowShearWebsite);

    //return all the data as a string
    //return flowAll;

    return flowData; // Return the flow reading for further processing
}

#endif