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

//set up low flow sensor with appropriate varibiles
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

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
String readFlowSensor(SensirionLF flowSensor, bool printTerminal) {
    //delay(100);

    //read the flow sensor
    int ret = flowSensor.readSample();

    //set up strings to hold readings
    String flowData = "";
    String flowTemp = "";
    String flowShear = "";
    String flowAll = "";
    String flowAllWeb = "";

    //if we were able to read flowsensor then we can get temp and flow otherwose there is an error
    if (ret == 0) {

        //get both flow and temp, and calculate shear stress based on flow rate
        float flowReading = flowSensor.getFlow();
        float flowTempReading = flowSensor.getTemp();
        float flowShearStress = shearStressCalc(flowReading);

        //Update rolling average for feedback control loop
        updateRollingAverage(flowReading);

        //if set to true, prints readings in terminal
        if (printTerminal == 1) {
            //Print flow and temp  to terminal
            Serial.print("Flow: ");
            Serial.print(flowReading, 2);
            Serial.print(" ml/min");

            //Print temp to terminal
            Serial.print(" | Temp: ");
            Serial.print(flowTempReading, 1);
            Serial.print(" deg C\n");
        }

        //Put flow data & temp into string varibile using rolling average from the feed back control
        flowData = String(rollingAverageFlow);  //This is pulled from FeedBackControl.hpp
        flowTemp = String(flowTempReading, 1);

        //Put shear stress data into string varibile
        flowShear += String(flowShearStress);
        //Serial.print(flowShear);

        flowAll = flowData + ", " + flowTemp + ", " + flowShear;

        //combine both into one String
        flowAllWeb += "Flow: " + flowData + " ml/min, " + "Temp: " + flowTemp + " deg C";
    } else {
        //if unable to read set string varibile as error message
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
        Serial.print("\n");
        flowData = "Error in flowsensor.readSample(): " + String(ret);
    }

    //send string varibiles to webserver/website through webscoket 
    String flowDataWebsite = "flowData; " + flowAllWeb;
    String flowShearWebsite = "shearStress; Shear Stress: " + flowShear + " Pa";
    ws.textAll(flowDataWebsite);
    ws.textAll(flowShearWebsite);

    //return all the data as a string
    return flowAll;
}

#endif