/************************************************************************
Gibson Moseley - BioreactorVaribiles.hpp

This holds all equations and varbilies about bioreactor setup to calculate values correctly
*************************************************************************/

#ifndef BIOREACTORVARIABLES_HPP
#define BIOREACTORVARIABLES_HPP

//All bioreactor varibiles needed for calculation (change as needed)
static constexpr double CHANNEL_HEIGHT = 1; // mm
static constexpr double CHANNEL_WIDTH = 25; // mm
static constexpr double MU = (0.93/60); // mPa * min
static constexpr double RHO = 993; // kg / m^3

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
    float sh = (flowRate * 6 * MU) / (CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT);
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