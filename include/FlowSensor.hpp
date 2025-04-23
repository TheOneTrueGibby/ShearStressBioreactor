/************************************************************************
Gibson Moseley - FlowSensor.hpp

Holds all commands necessry to use the flow sensor, and declerations of the flow sensor
*************************************************************************/

#ifndef FLOWSENSOR_HPP
#define FLOWSENSOR_HPP

//inclusion of necessary files/functions
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"
#include "BioreactorVariables.hpp"
#include "FeedBackControl.hpp"

void flowSensorSetup(SensirionLF flowSensor);
float readFlowSensor(SensirionLF flowSensor, bool printTerminal);
String getFlowsensorData();

//set up low flow sensor with appropriate varibiles
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

float tempStore;

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
float readFlowSensor(SensirionLF flowSensor, bool printTerminal) {
    //read the flow sensor
    int ret = flowSensor.readSample();
    float flowReading;

    //if we were able to read flowsensor then we can get temp and flow otherwose there is an error
    if (ret == 0) {

        //get both flow and temp, and calculate shear stress based on flow rate
        flowReading = flowSensor.getFlow();

        float flowTempReading = flowSensor.getTemp();
        tempStore = flowTempReading;

        //if set to true, prints readings in terminal
        if (printTerminal == 1) {
            // //Print flow and temp  to terminal
            Serial.print("Flow: ");
            Serial.print(rollingAverageFlow, 2);
            Serial.print(" ml/min");

            // Print temp to terminal
            Serial.print(" | Temp: ");
            Serial.print(flowTempReading, 1);
            Serial.print(" deg C\n");
        }

    } else {
        //if unable to read set string varibile as error message
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
        Serial.print("\n");
    }

    //return flow data as a float value
    return flowReading;
}

String getFlowsensorData () {
    //Put flow data & temp into string varibile using rolling average from the feed back control
    String flowData = String(rollingAverageFlow);  //This is pulled from FeedBackControl.hpp
    String flowTemp = String(tempStore);

    //Put shear stress data into string varibile
    String flowShear = String(shearStressCalc(rollingAverageFlow));

    String flowAll = flowData + ", " + flowTemp + ", " + flowShear;

    String flowAllWeb = " flowData; Flow: " + flowData + " ml/min, " + "Temp: " + flowTemp + " deg C";
    String flowShearWebsite = "shearStress; Shear Stress: " + flowShear + " Pa";

    ws.textAll(flowAllWeb);
    ws.textAll(flowShearWebsite);

    return flowAll;
}

#endif