#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>

int lowMotorDirPin = 13;
int lowMotorStepPin = 12;
int lowMotorEnaPin = 14;

int highMotorDirPin = 27;
int highMotorStepPin = 26;
int highMotorEnaPin = 25;

// void initStepperMotor(int dirPin, int stepPin, int enaPin) {
//     //set motor to on
//     digitalWrite(enaPin, LOW);
    
//     pinMode(stepPin, OUTPUT);
//     pinMode(dirPin, OUTPUT);
//     pinMode(enaPin, OUTPUT);
// }
void initStepperMotor(int dirPin, int stepPin, int enaPin) {
    
}

void initAllStepperMotors(int lowDirPin, int lowStepPin, int lowEnaPin, int highDirPin, int highStepPin, int highEnaPin) {
    //initStepperMotor(lowDirPin, lowStepPin, lowEnaPin);
    initStepperMotor(highDirPin, highStepPin, highEnaPin);
    Serial.print("Stepper Motors Intilized.\n");
}

void openStepperMotor(int numTicks, int dirPin) {
    int i;
    for (i = 0; i < numTicks; i++) {
        digitalWrite(dirPin, LOW);
    }
    Serial.print("Stepper Motor Opened.\n");
}

void closeStepperMotor(int numTicks, int dirPin) {
    int i;
    for (i = 0; i < numTicks; i++) {
        digitalWrite(dirPin, HIGH);
    }
    Serial.print("Stepper Motor Closed.\n");
}