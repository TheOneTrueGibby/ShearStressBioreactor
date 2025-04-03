/************************************************************************
Gibson Moseley - Routine.hpp

Allows for the running of routines for the system
*************************************************************************/

#ifndef ROUTINE_HPP
#define ROUTINE_HPP

//library for timing
#include <chrono>

//all necessary files for includes
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "FlowSensor.hpp"
#include "MicrosdCard.hpp"

//naming conventions
using namespace std;
using namespace std::chrono;

const int secondCheck = 1000;

//this converts the amount of time running in seconds to a String with Hr:Min:Sec
String convertTimeToString(int timeSeconds) {
    int timeLeft = timeSeconds;

    int totalHr = timeSeconds / 3600;
    int totalMin = (timeSeconds % 3600) / 60;
    int totalSec = timeSeconds % 60;

    String clockConversion = "";

    clockConversion = "Routine Time: " + String(totalHr) + ":" + String(totalMin) + ":" + String(totalSec);

    String clockWebsite = "runningTime; " + clockConversion;
    ws.textAll(clockWebsite);
    //Serial.print(clockConversion);
    //Serial.print("\n");

    return clockConversion;
    
}

//alows seeting for a basic routne that repeats x times in hr conversions, scaled in seconds and rounds.
//Example 3.6 seconds will be 4 seconds
void setRoutine(String routineName, double timeRun, double timeBreak, double shearStress, int repetion) {
    //calculate flowrate needed to achive required shearStress
    int flowRate = flowRateBasedOnShearStressCalc(shearStress);

    //scale the time to run and break from hr to seconds
    auto timeRunHr =  std::chrono::duration<double, std::chrono::seconds::period>(timeRun * 3600);
    auto timeBreakHr = std::chrono::duration<double, std::chrono::seconds::period>(timeBreak * 3600);

    //Print name of routinr that is being run
    Serial.printf("Running Routine: %s\n", routineName);

    //runn the routine as may times as speicifed
    for(int i = 1; i < repetion + 1; i++) {
        checkPumpStatus(0);

        //make sure flow rate is set and pump is off
        if (pumpOn == 1 & i == 0) {
            setPump(0);
            setPumpSpeed(flowRate, 1);
        }
        else if (pumpOn == 0 & i == 0) {
            setPumpSpeed(flowRate, 1);
        }

        //print what repition we are on and start pump
        //Serial.printf("Starting Routine Iteration: %d\n", i);
        setPump(1);

        //clock to calculate total time of the routine run and when samples were read
        auto startTotal = high_resolution_clock::now();

        //run the pump for specified amount of run time
        auto start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeRunHr) {
            delay(secondCheck);
            auto currentTotal = duration_cast<seconds>(high_resolution_clock::now() - startTotal);
            int timeInt = currentTotal.count();
            String time = convertTimeToString(timeInt);
            String flow = readFlowSensor(flowSensor, 0);
            String pump = checkPumpStatus(0);

            writeBioreactorInfo(routineName, time, flow, pump);
        }
        //debug code to see how long it was run
        //auto finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
        //int time = finalTime.count();
        //Serial.printf("Time ran was %d seconds, ", time);

        //for break turn pump off
        setPump(0);

        //dont run the pump for the speicifed amount of break time
        start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeBreakHr) {
            delay(secondCheck);
            auto currentTotal = duration_cast<seconds>(high_resolution_clock::now() - startTotal);
            int timeInt = currentTotal.count();
            String time = convertTimeToString(timeInt);
            String flow = readFlowSensor(flowSensor, 0);
            String pump = checkPumpStatus(0);

            writeBioreactorInfo(routineName, time, flow, pump);
        }
        //debug code to see how long it was not run
        //finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
        //time = finalTime.count();
        //Serial.printf("Time paused was %d seconds.\n", time);
    }

    //denote routine is over
    Serial.printf("Routine %s Done\n", routineName);
}

#endif