/************************************************************************
Gibson Moseley & Dillon West - BioreactorVaribiles.hpp
Some code used from last years team - Carson Sloan (utils.hpp/utils.cpp)

This holds all equations and varbilies about bioreactor setup to calculate values correctly
*************************************************************************/

#ifndef BIOREACTORVARIABLES_HPP
#define BIOREACTORVARIABLES_HPP

//Library includes
#include <Preferences.h>

//All function declerations
void pushCurrentVariables();
void setBioreactorSettings();
void saveBioreactorSettings(float height, float width, float mu, float rho);
float shearStressCalc(float flowRate);
float flowRateBasedOnShearStressCalc(float shearStress);
float reynoldsCalc(float flowRate);

//Prefrences object (used to save settings)
Preferences preferences;

//All bioreactor varibiles needed for calculation (change as needed)
float CHANNEL_HEIGHT = 1; // mm
float CHANNEL_WIDTH = 25; // mm
float MU = 0.0155; // mPa * min
float RHO = 993; // kg / m^3

//Push current bioreactor vars
void pushCurrentVariables() {
    preferences.begin("bioreactor", true);

    CHANNEL_HEIGHT = preferences.getFloat("height", 1);
    CHANNEL_WIDTH = preferences.getFloat("width", 25);
    MU = preferences.getFloat("mu", 0.0155);
    RHO = preferences.getFloat("rho", 993);

    preferences.end();

    String height = "channelHeight; Channel Height: " + String(CHANNEL_HEIGHT) + " mm";
    String width = "channelWidth; Channel Width: " + String(CHANNEL_WIDTH) + " mm";
    String mu = "MU; MU: " + String(MU, 5) + " mPa * min"; // Specify number of decimal places
    String rho = "RHO; RHO: " + String(RHO) + " kg / m^3";
 
    ws.textAll(height);
    ws.textAll(width);
    ws.textAll(mu);
    ws.textAll(rho);

    Serial.print("Pushing Current Vars\n");
    return;
}

//This starts the prefrences objects and vars and sets them as these base values
void setBioreactorSettings() {
    preferences.begin("bioreactor", true);

    CHANNEL_HEIGHT = preferences.getFloat("height", 1);
    CHANNEL_WIDTH  = preferences.getFloat("width", 25);
    MU             = preferences.getFloat("mu", 0.0155);
    RHO            = preferences.getFloat("rho", 993);

    preferences.end();

    pushCurrentVariables();
}

//This sets the current prefrences vars to new values (they will persist through power cycles)
void saveBioreactorSettings(float height, float width, float mu, float rho) {
    preferences.begin("bioreactor", false);

    //Only save if value is diffrent & above zero for all values
    if ((height != CHANNEL_HEIGHT) && (height > 0)) {
        preferences.putFloat("height", height);
        CHANNEL_HEIGHT = height; // Update the in-memory value
    }

    if ((width != CHANNEL_WIDTH) && (width > 0)) {
        preferences.putFloat("width", width);
        CHANNEL_WIDTH = width;
    }

    if ((mu != MU) && (mu > 0)) {
        preferences.putFloat("mu", mu);
        MU = mu;
    }

    if ((rho != RHO) && (rho > 0)) {
        preferences.putFloat("rho", rho);
        RHO = rho;
    }

    preferences.end();

    // CHANNEL_HEIGHT = height;
    // CHANNEL_WIDTH  = width;
    // MU             = mu;
    // RHO            = rho;

    //pushes new saved settings
    pushCurrentVariables();
}

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