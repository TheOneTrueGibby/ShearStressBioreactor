#include <YAAJ_ModbusMaster.h>

//YAAJ_ModbusMaster controller;

int pumpState = 0;

void checkPump(YAAJ_ModbusMaster controller) {
    if (controller.F1_ReadCoils(0x1001, 1) == 0) {
        uint16_t state = controller.getRxBuf(0);
        Serial.printf("Pump status: %d\n", state);
        pumpState = state; // 0 = off, 1 = on
    }
    else {
        Serial.println("Error: Unable to read pump state!");
    }
}

void togglePump(YAAJ_ModbusMaster controller) {
    if(pumpState == 0) {
        uint16_t result = controller.F5_WriteSingleCoil(0x1001, 0xFF);
        if (result != 0) {
            Serial.printf("Unable to switch pump state! Error code: %d\n", result);
        } else {
            Serial.printf("Pump has been turned on");
        }
    } else {
        uint16_t result = controller.F5_WriteSingleCoil(0x1001, 0x00);
        if (result != 0) {
            Serial.printf("Unable to switch pump state! Error code: %d\n", result);
        } else {
            Serial.printf("Pump has been truned off");
        }
    }
}

void setPumpSpeed() {
    
}