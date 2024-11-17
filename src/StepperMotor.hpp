#include <Arduino.h>
#include <Wire.h>

int lowMotorDirPin = 13;
int lowMotorStepPin = 12;
int lowMotorEnaPin = 14;

int highMotorDirPin = 27;
int highMotorStepPin = 26;
int highMotorEnaPin = 25;

void initStepperMotor(int dirPin, int stepPin, int enaPin) {
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(enaPin, OUTPUT);

    //set motor to on
    digitalWrite(enaPin, LOW);
}

void initAllStepperMotors(int lowDirPin, int lowStepPin, int lowEnaPin, int highDirPin, int highStepPin, int highEnaPin) {
    initStepperMotor(lowDirPin, lowStepPin, lowEnaPin);
    initStepperMotor(highDirPin, highStepPin, highEnaPin);
}

void openStepperMotor(int numTicks, int dirPin) {
    int i;
    for (i = 0; i < numTicks; i++) {
        digitalWrite(dirPin, LOW);
    }
}

void closeStepperMotor(int numTicks, int dirPin) {
    int i;
    for (i = 0; i < numTicks; i++) {
        digitalWrite(dirPin, HIGH);
    }
}