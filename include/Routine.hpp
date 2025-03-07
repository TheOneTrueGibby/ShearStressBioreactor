/************************************************************************
Gibson Moseley - Routine.hpp
*************************************************************************/

//#include <iostream>

#include <chrono>
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "FlowSensor.hpp"

using namespace std;
using namespace std::chrono;

int timeRunningTotal;
int timeRunningRoutine;

//alows seeting for a basic routne that repeats x times in hr conversions
void setRoutine(double timeRun, double timeBreak, double shearStress, int repetion) {
    int flowRate = flowRateBasedOnShearStressCalc(shearStress);
    auto timeRunHr =  std::chrono::duration<double, std::chrono::seconds::period>(timeRun * 3600);
    auto timeBreakHr = std::chrono::duration<double, std::chrono::seconds::period>(timeBreak * 3600);

    for(int i = 1; i < repetion + 1; i++) {
        checkPumpStatus(0);

        if (pumpOn == 1 & i == 0) {
            setPump(0);
            setPumpSpeed(flowRate, 1);
        }
        else if (pumpOn == 0 & i == 0) {
            setPumpSpeed(flowRate, 1);
        }

        Serial.printf("Starting Routine Iternation: %d\n", i);
        setPump(1);

        auto start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeRunHr) {
            readFlowSensor(flowSensor, 0);
            checkPumpStatus(0);
        }
        auto finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
        int time = finalTime.count();
        Serial.printf("Time ran was %d seconds\n", time);

        setPump(0);
        start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeBreakHr) {
            readFlowSensor(flowSensor, 0);
            checkPumpStatus(0);
        }
        finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
        time = finalTime.count();
        Serial.printf("Time paused was %d seconds\n", time);
    }
}