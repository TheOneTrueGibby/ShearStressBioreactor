/************************************************************************
Gibson Moseley - FlowSensor.hpp

Holds all commands necessry to use the flow sensor, and declerations of 
the flow sensor and used those readings to control the pump
*************************************************************************/

#ifndef FLOWSENSOR_HPP
#define FLOWSENSOR_HPP

//Library Includes
#include <deque>
#include <Ticker.h>

//Inclusion of necessary files/functions
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"
#include "BioreactorVariables.hpp"
#include "Pump.hpp"

//Function Declerations
void flowSensorSetup(SensirionLF flowSensor);
float readFlowSensor(SensirionLF flowSensor, bool printTerminal);
String getFlowsensorData();
float calculateRollingAverage(float newReading);
void updateRollingAverage();
float calculatePID(float setpoint, float measuredValue);
int smoothPumpSpeed(int currentSpeed, int targetSpeed, int maxChange);
void controlPumpSpeed(float setpoint);

//Set up low flow sensor with appropriate varibiles
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

//Rolling average buffer for flow readings
std::deque<float> flowReadings;

//Stores last temp reading
float tempStore;

//Flag for pushing var changes
bool varPush = false;


float rollingAverageFlow = 0.0; //Global rolling average of flow rate
float newFlowReading = 0.0; //Temp holder for each new reading
const int rollingWindowSize = 50; //Size of the rolling window for average flow rate
bool flowInRange = false; //Flag to indicate if flow is within range

//PID parameters
float kp = 0.1; //Proportional gain
float ki = 0.1; //Integral gain
float kd = 0.01; //Derivative gain

//Initial PID parameters of Kp = 0.1, and Ki = 0.1 gave good first control scheme

//PID state variables
float previousError = 0.0;
float integral = 0.0;

//Sets up the flowsensor to be used
void flowSensorSetup(SensirionLF flowSensor) {
    //Run the intilizing command and save result
    uint16_t reset = flowSensor.init();

    //If result is zero then error happened otherwise it was reset/setup
    if (reset != 0) {
        Serial.print("Error initializing the flow sensor: ");
        Serial.println(reset);
        return;
    } else {
        Serial.println("Flow sensor initialized.");
    }
}

//Reads the provided flowsensor provided and prints out in serial if set to true
float readFlowSensor(SensirionLF flowSensor, bool printTerminal) {
    //Read the flow sensor
    int ret = flowSensor.readSample();
    float flowReading = 0.0;
    float lastFlowReading;

    //If we were able to read flowsensor then we can get temp and flow otherwose there is an error
    if (ret == 0) {

        //Get both flow and temp, and calculate shear stress based on flow rate
        flowReading = flowSensor.getFlow();
        lastFlowReading = flowReading; //Store the latest flow rate reading

        float flowTempReading = flowSensor.getTemp();
        tempStore = flowTempReading;

        //If set to true, prints readings in terminal
        if (printTerminal == 1) {
            //Print flow and temp  to terminal
            Serial.print("Flow: ");
            Serial.print(rollingAverageFlow, 2);
            Serial.print(" ml/min");

            //Print temp to terminal
            Serial.print(" | Temp: ");
            Serial.print(flowTempReading, 1);
            Serial.print(" deg C\n");
        }

    } else {
        //If unable to read, set string variable as error message
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
        Serial.print("\n");
        flowReading = lastFlowReading; //If an error occurs, return the last known flow reading to not mess up the rolling average

    }
    //Return flow data as a float value
    return flowReading;
}

//Returns a string of formatted flow data and pushes to website
String getFlowsensorData () {
    //Put flow data & temp into string varibile using rolling average
    String flowData = String(rollingAverageFlow);
    String flowTemp = String(tempStore);

    //Put shear stress data into string varibile
    String flowShear = String(shearStressCalc(rollingAverageFlow));

    //Combine them all for MicroSD card use from return
    String flowAll = flowData + ", " + flowTemp + ", " + flowShear;

    //Make strngs to push to website
    String flowAllWeb = " flowData; Flow: " + flowData + " mL/min, " + "Temp: " + flowTemp + " deg C";
    String flowShearWebsite = "shearStress; Shear Stress: " + flowShear + " mPa";

    //Push strings to website
    ws.textAll(flowAllWeb);
    ws.textAll(flowShearWebsite);

    //Return all data as a string
    return flowAll;
}

//Calculate the rolling average from a stream of flow readings
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

    //Return the average
    return average;
}

//Ensure the rollingAverageFlow variable is updated
void updateRollingAverage() {
    //Get flow data
    newFlowReading = readFlowSensor(flowSensor, false);
    Serial.printf("Current flow: %f\n", newFlowReading);

    //Update rollingAverageFlow
    rollingAverageFlow = calculateRollingAverage(newFlowReading);
    Serial.print("Updated Rolling Average Flow: ");
    Serial.println(rollingAverageFlow); //Debug print to verify rolling average update
}

//Function to calculate PID output
float calculatePID(float setpoint, float measuredValue) {
    float error = setpoint - measuredValue;
    integral += error*1; //Integral term (adjust the time step as needed)
    float derivative = error - previousError;
    previousError = error;

    //PID formula
    //return (kp * error) + (ki * integral) + (kd * derivative);
    return (kp * error) + (ki * integral);
}

//Function to smoothly ramp the pump speed
int smoothPumpSpeed(int currentSpeed, int targetSpeed, int maxChange) {
    //Increases and decreases pump speed gradually based on the diffrence between current and target speed
    if (abs(targetSpeed - currentSpeed) > maxChange) {
        if (targetSpeed > currentSpeed) {
            targetSpeed =  currentSpeed + maxChange;
        } 
        else  {
            targetSpeed = currentSpeed - maxChange;
        }
    }
    else {
        //If within range, set to target speed 
        currentSpeed = targetSpeed;
    }

    //Return the target speed
    return targetSpeed;
}

//Updated function to control pump speed using PID with smooth ramping
void controlPumpSpeed(float setpoint) {
    float difference = setpoint*0.05;
    //float currentFlowRate = rollingAverageFlow;

    //Check if the flow rate is within the acceptable range
    if (difference > abs(setpoint - rollingAverageFlow) && rollingAverageFlow > 0) {
        flowInRange = true; //Set the flag to indicate flow is in range
        //Serial.println("Flow is in range.");
    } else {
        flowInRange = false; //Set the flag to indicate flow is not in range
        //Serial.println("Flow is not in range.");
    }

    if (!flowInRange) {

        float pidOutput = calculatePID(setpoint, rollingAverageFlow);

        //Convert PID output to a valid pump speed
        int targetSpeed = constrain((int)pidOutput, 8, 400); //Ensure speed is within pump range

        //Get the current pump speed
        int currentSpeed = getPumpSpeed();

        //Smoothly ramp the pump speed
        int newSpeed = smoothPumpSpeed(currentSpeed, targetSpeed, 25); //Limit speed change to 25 units per loop

        //Set the pump speed
        setPumpSpeed(newSpeed, true);

        //Debug
        //Serial.print("Current Flow: ");
        //Serial.print(rollingAverageFlow);
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