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

float lastFlow = 0.0;
extern float rollingAverageFlow; // Declare rolling average flow variable as external

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

float getRawFlow(SensirionLF) {
    //read the flow sensor
    int ret = flowSensor.readSample();

    //if we were able to read flowsensor then we can get temp and flow otherwose there is an error
    if (ret == 0) {
        //get both flow and temp, and calculate shear stress based on flow rate
        float flow = flowSensor.getFlow();
        lastFlow = flow; // Store the last flow reading
        return lastFlow; // Return the flow reading
    } else {
        //if unable to read set string varibile as error message
        //Serial.print("Error in flowsensor.readSample(): ");
        //Serial.println(ret);
        return lastFlow; // Return the last valid flow reading
    }
}

//reads the provided flowsensor provided and prints out in serial if set to true
String readFlowSensor(SensirionLF flowSensor, bool printTerminal) {
    //delay(100);

    //read the flow sensor
    int ret = flowSensor.readSample();

    //set up strings to hold readings
    String flowData = "";
    String flowShear = "";
    String flowAll = "";

    //if we were able to read flowsensor then we can get temp and flow otherwose there is an error
    if (ret == 0) {

        //get both flow and temp, and calculate shear stress based on flow rate
        //float flowReading = flowSensor.getFlow();
        float flowTemp = flowSensor.getTemp();
        float flowShearStress = shearStressCalc(rollingAverageFlow);

        //if set to true, prints readings in terminal
        if (printTerminal == 1) {
            // //Print flow and temp  to terminal
            Serial.print("Flow: ");
            Serial.print(rollingAverageFlow, 2);
            Serial.print(" ml/min");

            // Print temp to terminal
            Serial.print(" | Temp: ");
            Serial.print(flowTemp, 1);
            Serial.print(" deg C\n");
        }

        //Put flow data & temp into string varibile using rolling average from the feed back control
        flowData += "Flow: ";
        flowData += String(rollingAverageFlow) + " ml/min";  //This is pulled from FeedBackControl.hpp
        flowData += ", Temp: ";
        flowData += String(flowTemp, 1) + " deg C";

        //Put shear stress data into string varibile
        flowShear += "Shear Stress: ";
        flowShear += String(flowShearStress) + " mPa";
        //Serial.print(flowShear);

        //combine both into one String
        flowAll += flowData + ", " + flowShear;
    } else {
        //if unable to read set string varibile as error message
        // Serial.print("Error in flowsensor.readSample(): ");
        // Serial.println(ret);
        // Serial.print("\n");
        //flowData = "Error in flowsensor.readSample(): " + String(ret);
        flowAll += flowData + ", " + flowShear; // Send last known flow data info
    }

    //send string varibiles to webserver/website through webscoket 
    String flowDataWebsite = "flowData; " + flowData;
    String flowShearWebsite = "shearStress; " + flowShear;
    ws.textAll(flowDataWebsite);
    ws.textAll(flowShearWebsite);

    //return all the data as a string
    return flowAll;
}

#endif