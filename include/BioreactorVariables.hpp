/************************************************************************
Gibson Moseley - BioreactorVaribiles.hpp
Some code used from last years team - Carson Sloan (utils.hpp/utils.cpp)

This holds all equations and varbilies about bioreactor setup to calculate values correctly
*************************************************************************/

#ifndef BIOREACTORVARIABLES_HPP
#define BIOREACTORVARIABLES_HPP

//Library includes
#include <Preferences.h>

//Function Declerations
void pushCurrentVariables();
void setBioreactorSettings();
void saveBioreactorSettings(float height, float width, float mu, float rho);
float shearStressCalc(float flowRate);
float flowRateBasedOnShearStressCalc(float shearStress);
float reynoldsCalc(float flowRate);

Preferences preferences;

extern float rollingAverageFlow;

//All bioreactor varibiles needed for calculation (change as needed)
float CHANNEL_HEIGHT = 1; // mm
float CHANNEL_WIDTH = 25; // mm
float MU = 0.0155; // mPa * min
float RHO = 993; // kg / m^3

//Push current bioreactor vars
void pushCurrentVariables() {
    //Read-only access to preferences
    preferences.begin("bioreactor", true);

    //Retrieve all saved prefrences
    CHANNEL_HEIGHT = preferences.getFloat("height", 1);
    CHANNEL_WIDTH = preferences.getFloat("width", 25);
    MU = preferences.getFloat("mu", 0.0155);
    RHO = preferences.getFloat("rho", 993);

    //Close preferences
    preferences.end();

    //Put all saved prefrences into strings
    String height = "channelHeight; Channel Height: " + String(CHANNEL_HEIGHT) + " mm";
    String width = "channelWidth; Channel Width: " + String(CHANNEL_WIDTH) + " mm";
    String mu = "MU; MU: " + String(MU, 5) + " mPa * min"; // Specify number of decimal places
    String rho = "RHO; RHO: " + String(RHO) + " kg / m^3";
 
    //Send all the strings to the website
    ws.textAll(height);
    ws.textAll(width);
    ws.textAll(mu);
    ws.textAll(rho);

    //Denotion that function was ran
    Serial.print("Pushing Current Vars\n");
    return;
}

//Load default bioreactor settings and update website
void setBioreactorSettings() {
    //Read-only access to preferences
    preferences.begin("bioreactor", true);

    //Set all prefrences
    CHANNEL_HEIGHT = preferences.getFloat("height", 1);
    CHANNEL_WIDTH = preferences.getFloat("width", 25);
    MU = preferences.getFloat("mu", 0.0155);
    RHO = preferences.getFloat("rho", 993);

    //Close preferences
    preferences.end();

    //Update Website
    pushCurrentVariables();
}

//Save new bioreactor settings if they are different and bigger than zero
void saveBioreactorSettings(float height, float width, float mu, float rho) {
    //Read and write access to preferences
    preferences.begin("bioreactor", false);

    //Only save if value is diffrent and greater than zero
    if ((height != CHANNEL_HEIGHT) && (height > 0)) {
        //Save new height & update memory value
        preferences.putFloat("height", height);
        CHANNEL_HEIGHT = height;
    }

    if ((width != CHANNEL_WIDTH) && (width > 0)) {
        //Save new width & update memory value
        preferences.putFloat("width", width);
        CHANNEL_WIDTH = width;
    }

    if ((mu != MU) && (mu > 0)) {
        //Save new mu & update memory value
        preferences.putFloat("mu", mu);
        MU = mu;
    }

    if ((rho != RHO) && (rho > 0)) {
        //Save new rho & update memory value
        preferences.putFloat("rho", rho);
        RHO = rho;
    }

    //Close preferences
    preferences.end();

    //Debug
    // CHANNEL_HEIGHT = height;
    // CHANNEL_WIDTH  = width;
    // MU             = mu;
    // RHO            = rho;

    //Update Website
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