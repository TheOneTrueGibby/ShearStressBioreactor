/************************************************************************
Gibson Moseley & Dillon West - FeedBackConrol.hpp

*************************************************************************/

#include <deque>
#include <Ticker.h> //Ensure the Ticker library is included

//Rolling average buffer for flow readings
std::deque<float> flowReadings;

//Create a Ticker object to update the rolling average
Ticker rollingAverageUpdater;

float desiredFlowRate = 100.0; //Setpoint in ml/min
float rollingAverageFlow = 0.0; //Define the rolling average flow rate variable
float currentFlowRate = 0.0; //Variable to hold the current flow rate

const int rollingWindowSize = 25; //Size of the rolling window for aver

//Ensure the rollingAverageFlow variable is updated globally
void updateRollingAverage() {
    float currentFlowRate = readFlowSensor(flowSensor, 0);
    // Serial.print("Current Flow Rate: ");
    // Serial.println(currentFlowRate); // Debug print to verify sensor reading

    rollingAverageFlow = calculateRollingAverage(currentFlowRate);
    // Serial.print("Updated Rolling Average Flow: ");
    // Serial.println(rollingAverageFlow); // Debug print to verify rolling average update
}

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