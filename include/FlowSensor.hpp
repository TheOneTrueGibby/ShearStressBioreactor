/************************************************************************
Gibson Moseley - FlowSensor.hpp

Holds all commands necessry to use the flow sensor, and declerations of the flow sensor
*************************************************************************/

#ifndef FLOWSENSOR_HPP
#define FLOWSENSOR_HPP

#include <deque>
#include <Ticker.h> //Ensure the Ticker library is included

//inclusion of necessary files/functions
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"
#include "BioreactorVariables.hpp"
#include "FeedBackControl.hpp"
#include "Pump.hpp"

void flowSensorSetup(SensirionLF flowSensor);
float readFlowSensor(SensirionLF flowSensor, bool printTerminal);
String getFlowsensorData();
float calculateRollingAverage(float newReading);
void updateRollingAverage();
float calculatePID(float setpoint, float measuredValue);
int smoothPumpSpeed(int currentSpeed, int targetSpeed, int maxChange);
void controlPumpSpeed(float setpoint);

//set up low flow sensor with appropriate varibiles
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

//Ticker decleration
Ticker rollingAverageUpdater;

//Rolling average buffer for flow readings
std::deque<float> flowReadings;

float tempStore;

bool varPush = false;

float rollingAverageFlow = 0.0;
float currentFlowRate = 0.0; // Variable to store the rolling average flow rate
const int rollingWindowSize = 20; //Size of the rolling window for average flow rate
bool flowInRange = false; //Flag to indicate if flow is within range

//PID parameters
float kp = 0.1; //Proportional gain
float ki = 0.1; //Integral gain
float kd = 0.01; //Derivative gain

//Initial PID parameters of Kp = 0.1, and Ki = 0.1 gave good first control scheme

//PID state variables
float previousError = 0.0;
float integral = 0.0;

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

float calculateRollingAverage(float newReading) {
    //Debug print to verify new reading
    //Serial.print("New Reading for Rolling Average: ");
    //Serial.println(newReading);

    //Add the new reading to the buffer
    flowReadings.push_front(newReading);

    float sum = 0.0; //Variable to hold the sum of flow readings

    //Remove the oldest reading if the buffer exceeds the window size
    if (flowReadings.size() > rollingWindowSize) {
        flowReadings.pop_back();
    }

    //Calculate the average
    for (float reading : flowReadings) {
        sum += reading;
    }
    float average = sum / flowReadings.size();
    //Serial.print("Calculated Rolling Average: ");
    //Serial.println(average); // Debug print to verify calculation
    return average;
}

//Ensure the rollingAverageFlow variable is updated globally
void updateRollingAverage() {
    float newFlowReading = readFlowSensor(flowSensor, false);

    rollingAverageFlow = calculateRollingAverage(newFlowReading);
    //Serial.print("Updated Rolling Average Flow: ");
    //Serial.println(rollingAverageFlow); //Debug print to verify rolling average update
}

// Function to calculate PID output
float calculatePID(float setpoint, float measuredValue) {
    float error = setpoint - measuredValue;
    integral += error*1; //Integral term (adjust the time step as needed)
    float derivative = error - previousError;
    previousError = error;

    //PID formula
    //return (kp * error) + (ki * integral) + (kd * derivative);
    return (kp * error) + (ki * integral);
}

// Function to smoothly ramp the pump speed
int smoothPumpSpeed(int currentSpeed, int targetSpeed, int maxChange) {
    if (abs(targetSpeed - currentSpeed) > maxChange) {
        if (targetSpeed > currentSpeed) {
            targetSpeed =  currentSpeed + maxChange;
        } 
        else  {
            targetSpeed = currentSpeed - maxChange;
        }
    }
    else {
        currentSpeed = targetSpeed; //If within range, set to target speed 
    }
    return targetSpeed;
}

//Updated function to control pump speed using PID with smooth ramping
void controlPumpSpeed(float setpoint) {
    float difference = setpoint*0.05;
    float currentFlowRate = rollingAverageFlow;
    //Check if the flow rate is within the acceptable range
    if (difference > abs(setpoint - currentFlowRate) && currentFlowRate > 0) {
        flowInRange = true; // Set the flag to indicate flow is in range
        //Serial.println("Flow is in range.");
    } else {
        flowInRange = false; // Set the flag to indicate flow is not in range
        //Serial.println("Flow is not in range.");
    }

    if (!flowInRange) {

        float pidOutput = calculatePID(setpoint, currentFlowRate);

        //Convert PID output to a valid pump speed
        int targetSpeed = constrain((int)pidOutput, 8, 400); // Ensure speed is within pump range

        //Get the current pump speed
        int currentSpeed = getPumpSpeed();

        //Smoothly ramp the pump speed
        int newSpeed = smoothPumpSpeed(currentSpeed, targetSpeed, 25); // Limit speed change to 25 units per loop

        //Set the pump speed
        setPumpSpeed(newSpeed, true);

        //Serial.print("Current Flow: ");
        //Serial.print(currentFlowRate);
        //Serial.print(" | Target Speed: ");
        //Serial.print(targetSpeed);
        //Serial.print(" | Current Speed: ");
        //Serial.print(currentSpeed);
        //Serial.print(" | New Speed: ");
        //Serial.println(newSpeed);
        //delay(1000); // Delay time for flow to adjust
    }
}    

#endif