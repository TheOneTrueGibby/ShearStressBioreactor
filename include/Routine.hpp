/************************************************************************
Gibson Moseley - Routine.hpp
*************************************************************************/

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
    auto timeRunHr =  std::chrono::duration<double, std::chrono::hours::period>(timeRun);
    auto timeBreakHr = std::chrono::duration<double, std::chrono::hours::period>(timeBreak);

    for(int i = 0; i < repetion; i++) {
        checkPumpStatus();
        if(pumpOn == 1) {
            setPump(0);
        }

        Serial.printf("Starting Routine Iternation: %d\n", i);
        setPumpSpeed(flowRate, false);
        setPump(1);

        Serial.printf("Running for %d hours\n", timeRun);
        auto start = high_resolution_clock::now();
        while (duration_cast<hours>(high_resolution_clock::now() - start) < timeRunHr) {
            //readFlowSensor(flowSensor, 0);
            start = high_resolution_clock::now();
            delay(250);
        }

        Serial.printf("Breaking for %d hours\n", timeBreak);
        setPump(0);
        start = high_resolution_clock::now();
        while (duration_cast<hours>(high_resolution_clock::now() - start) < timeRunHr) {
            //readFlowSensor(flowSensor, 0);
            start = high_resolution_clock::now();
            delay(250);
        }
    }
}