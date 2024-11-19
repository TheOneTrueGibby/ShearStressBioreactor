#include <YAAJ_ModbusMaster.h>

//YAAJ_ModbusMaster controller;

int pumpState = 0;

const int STEP_0 = 8;
const int STEP_1 = 16;
const int STEP_2 = 32;
const int STEP_3 = 64;
const int STEP_4 = 128;
const int STEP_5 = 256;

const uint16_t STEP_0_CMD = 0x4100;
const uint16_t STEP_1_CMD = 0x4180;
const uint16_t STEP_2_CMD = 0x4200;
const uint16_t STEP_3_CMD = 0x4280;
const uint16_t STEP_4_CMD = 0x4300;
const uint16_t STEP_5_CMD = 0x4380;
const uint16_t STEP_6_CMD = 0x43C8;

const int RATE_0 = 16;
const int RATE_1 = 8;
const int RATE_2 = 4;
const int RATE_3 = 2;       
const int RATE_4 = 1;       
const double RATE_5 = 0.5;

//checks if pump is on or off
void checkPump(YAAJ_ModbusMaster controller) {
    if (controller.F1_ReadCoils(0x1001, 1) == 0) {
        uint16_t state = controller.getRxBuf(0);
        Serial.printf("Pump status: %d\n", state);
        pumpState = state; // 0 = off, 1 = on
    }
    else {
        Serial.println("Error: Unable to read pump state!\n");
    }
}

//switches current state of pump, on to off or off to on
void togglePump(YAAJ_ModbusMaster controller) {
    if(pumpState == 0) {
        uint16_t result = controller.F5_WriteSingleCoil(0x1001, 0xFF);
        if (result != 0) {
            Serial.printf("Unable to switch pump state! Error code: %d\n", result);
        } else {
            Serial.printf("Pump has been turned on\n");
        }
    } else {
        uint16_t result = controller.F5_WriteSingleCoil(0x1001, 0x00);
        if (result != 0) {
            Serial.printf("Unable to switch pump state! Error code: %d\n", result);
        } else {
            Serial.printf("Pump has been truned off\n");
        }
    }
}

void setPumpSpeed(int flowSpeed, YAAJ_ModbusMaster controller) {
    //If pump is on we need to turn it off
    checkPump(controller);
    if(pumpState == 1) {
        togglePump(controller);
    }

    //If pump is off set the speed otherwise we were unable to turn it off
    if(pumpState == 0) {
        if (flowSpeed < 8) {
            flowSpeed = 8;
        }
        else if (flowSpeed > 400) {
            flowSpeed = 400;
        }

        uint16_t low = 0;
        uint16_t high = 0;

        // TODO: currently only integer flowSpeed rates are possible, but the high bytes can be used to 
        // achieve decimal values, following the calculations done for flowSpeed rates over 256 ml/min

        // Start with the known register value at the edge of the precision level, then add the needed steps
        if (flowSpeed <= STEP_1) {
            low = STEP_0_CMD + ((flowSpeed - STEP_0) * RATE_0);
        }
        else if (flowSpeed > STEP_1 && flowSpeed <= STEP_2) {
            low = STEP_1_CMD + ((flowSpeed - STEP_1) * RATE_1);
        }
        else if (flowSpeed > STEP_2 && flowSpeed <= STEP_3) {
            low = STEP_2_CMD + ((flowSpeed - STEP_2) * RATE_2);
        }
        else if (flowSpeed > STEP_3 && flowSpeed <= STEP_4) {
            low = STEP_3_CMD + ((flowSpeed - STEP_3) * RATE_3);
        }
        else if (flowSpeed > STEP_4 && flowSpeed <= STEP_5) {
            low = STEP_4_CMD + ((flowSpeed - STEP_4) * RATE_4);
        }
        else if (flowSpeed > STEP_5) {
            low = STEP_5_CMD + (int) ((flowSpeed - STEP_5) * RATE_5);
            high = (flowSpeed % 2) * (0x8000); // add half of a step to achieve odd numbers
        }

        controller.setTxBuf(0, low);
        controller.setTxBuf(1, high);
        int result = controller.F16_WriteMultipleHoldingRegisters(0x3001, 0x02);
        if (result != 0) {
            Serial.printf("Error (%d) setting flow rate!\n", result);
        }

        togglePump(controller);
        if (pumpState == 1) {
            Serial.print("Pump turned back on.\n");
        } else {
            Serial.print("Pump was not turned back on.\n");
        }
    } else {
        Serial.print("Unable to turn off Pump to set speed.\n");
    }

    
}