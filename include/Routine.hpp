/************************************************************************
Gibson Moseley - Routine.hpp
*************************************************************************/

#include <chrono>
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"

int timeRunningTotal;
int timeRunningRoutine;

void setRoutine(int timeRun, int timeBreak, double shearStress, int repetion) {
    int flowRate = flowRateBasedOnShearStressCalc(shearStress);

    for(int i = 0; i < repetion; i++) {
        checkPumpStatus();
        if (pumpOn == 0) {
            setPumpSpeed(flowRate, false);
            setPump(1);
        }
    }
}