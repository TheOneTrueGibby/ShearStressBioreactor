/************************************************************************
Gibson Moseley - BioreactorVaribiles.hpp
Some code used from last years team - Carson Sloan (utils.hpp/utils.cpp)

This holds all equations and varbilies about bioreactor setup to calculate values correctly
*************************************************************************/

#ifndef BIOREACTORVARIABLES_HPP
#define BIOREACTORVARIABLES_HPP

//All bioreactor varibiles needed for calculation (change as needed)
static constexpr double CHANNEL_HEIGHT = 1; // mm
static constexpr double CHANNEL_WIDTH = 25; // mm
static constexpr double MU = (0.93/60); // media viscosity, mPa * min
static constexpr double RHO = 993; // media density, kg / m^3

//Push current bioreactor vars
void pushCurrentVariables() {
    String height = "channelHeight; Channel Height: " + String(CHANNEL_HEIGHT) + " mm";
    String width = "channelWidth; Channel Width: " + String(CHANNEL_WIDTH) + " mm";
    String mu = "MU; MU: " + String(MU) + " mPa * min";
    String rho = "RHO; RHO: " + String(RHO) + " kg / m^3";
 
    ws.textAll(height);
    ws.textAll(width);
    ws.textAll(mu);
    ws.textAll(rho);
}

//Calculates flowrate based on sensor value given
// float flowRateCalc(float sensorValue) {
//     float fl = (((CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT * sensorValue) / (6 * MU * 1000)));
//     return fl;
// }

// //Calculates the shear stress in Pa that will result from the provided flow rate in ml/min.
// float shearStressCalc(float flowRate) {
//     float sh = (flowRate / 6e7) * (6 * MU) / (CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT);
//     return sh;
// }

//Calculates the shear stress in Pa that will result from the provided flow rate in ml/min.
float shearStressCalc(float flowRate) {
    float sh = (flowRate * 6 * MU * 1000) / (CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT);
    return sh;
}

//Calculates the flow rate ml/min needed to generate specified chear stress in mPa
float flowRateBasedOnShearStressCalc(float shearStress) {
    float fl = (shearStress *  CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT) / (6 * MU * 1000);
    return fl;
}

//Calculates the Reynolds number for the given flow rate.
float reynoldsCalc(float flowRate) {
    float r = (RHO * (flowRate / (CHANNEL_HEIGHT * CHANNEL_WIDTH)) * CHANNEL_HEIGHT) / MU;
    return r;
}

#endif