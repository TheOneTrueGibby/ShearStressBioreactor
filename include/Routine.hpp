/************************************************************************
Gibson Moseley - Routine.hpp

Allows for the running of routines for the system
*************************************************************************/

#ifndef ROUTINE_HPP
#define ROUTINE_HPP

#include <chrono>
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "FlowSensor.hpp"

using namespace std;
using namespace std::chrono;

int timeRunningTotal;
int timeRunningRoutine;

//alows seeting for a basic routne that repeats x times in hr conversions, scaled in seconds and rounds.
//Example 3.6 seconds will be 4 seconds
void setRoutine(double timeRun, double timeBreak, double shearStress, int repetion) {
    //calculate flowrate needed to achive required shearStress
    int flowRate = flowRateBasedOnShearStressCalc(shearStress);

    //scale the time to run and break from hr to seconds
    auto timeRunHr =  std::chrono::duration<double, std::chrono::seconds::period>(timeRun * 3600);
    auto timeBreakHr = std::chrono::duration<double, std::chrono::seconds::period>(timeBreak * 3600);

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
        Serial.printf("Starting Routine Iternation: %d, ", i);
        setPump(1);

        //run the pump for specified amount of time and print how long at the end
        auto start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeRunHr) {
            readFlowSensor(flowSensor, 0);
            checkPumpStatus(0);
        }
        auto finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
        int time = finalTime.count();
        Serial.printf("Time ran was %d seconds, ", time);

        //for break turn pump off
        setPump(0);

        //dont run the pump for speicifed amount of time and print it
        start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeBreakHr) {
            readFlowSensor(flowSensor, 0);
            checkPumpStatus(0);
        }
        finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
        time = finalTime.count();
        Serial.printf("Time paused was %d seconds.\n", time);
    }

    //denote routine is over
    Serial.print("Routine Done\n"); 
}

#endif