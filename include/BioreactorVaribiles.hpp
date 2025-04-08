/************************************************************************
Gibson Moseley - BioreactorVaribiles.hpp

This holds all equations and varbilies about bioreactor setup to calculate values correctly
*************************************************************************/

#ifndef BIOREACTORVARIBILES_HPP
#define BIOREACTORVARIBILES_HPP

//All bioreactor varibiles needed for calculation (change as needed)
static constexpr double CHANNEL_HEIGHT = 0.0005; // m
static constexpr double CHANNEL_WIDTH = 0.025; // m
static constexpr double MU = 0.0155; // mPa * min
static constexpr double RHO = 993; // kg / m^3

//Calculates flowrate based on sensor value given
float flowRateCalc(float sesnorValue) {
    float fl = (((CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT * sesnorValue) / (6 * MU)) * 6e7)/32;
    return fl;
}

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

//Calculates the flow rate ml/min needed to generate specified chear stress in Pa
float flowRateBasedOnShearStressCalc(float shearStress) {
    float fl = (shearStress *  CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT) / (6 * MU);
    return fl;
}

//Calculates the Reynolds number for the given flow rate.
float reynoldsCalc(float flowRate) {
    float r = (RHO * (flowRate / (CHANNEL_HEIGHT * CHANNEL_WIDTH)) * CHANNEL_HEIGHT) / MU;
    return r;
}

#endif