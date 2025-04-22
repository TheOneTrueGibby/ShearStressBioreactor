/************************************************************************
Gibson Moseley - BioreactorVaribiles.hpp
Some code used from last years team - Carson Sloan (utils.hpp/utils.cpp)

This holds all equations and varbilies about bioreactor setup to calculate values correctly
*************************************************************************/

#ifndef BIOREACTORVARIABLES_HPP
#define BIOREACTORVARIABLES_HPP

//Library includes
#include <Preferences.h>

Preferences preferences;

//All bioreactor varibiles needed for calculation (change as needed)
float CHANNEL_HEIGHT = 1; // mm
float CHANNEL_WIDTH = 25; // mm
float MU = (0.93/60); // mPa * min
float RHO = 993; // kg / m^3

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

    //Serial.print("Pushing Current Vars\n");
    return;
}

void setBioreactorSettings() {
    preferences.begin("bioreactor", true);

    CHANNEL_HEIGHT = preferences.getFloat("height", 1);
    CHANNEL_WIDTH  = preferences.getFloat("width", 25);
    MU             = preferences.getFloat("mu", 0.93 / 60);
    RHO            = preferences.getFloat("rho", 993);

    preferences.end();

    pushCurrentVariables();
}

void saveBioreactorSettings(float height, float width, float mu, float rho) {
    preferences.begin("bioreactor", false);

    preferences.putFloat("height", height);
    preferences.putFloat("width", width);
    preferences.putFloat("mu", mu);
    preferences.putFloat("rho", rho);

    preferences.end();

    pushCurrentVariables();
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