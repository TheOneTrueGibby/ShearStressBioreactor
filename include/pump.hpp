#pragma once

#include <YAAJ_ModbusMaster.h>

class Pump {
    public:
        Pump(YAAJ_ModbusMaster controller);

        bool setPump(bool option);
        bool togglePump();
        bool setSpeed(uint16_t high, uint16_t low);
        bool setSpeed(int flow);
        bool isPumpOn();
        int32_t getSpeed(bool print);
    private:
        bool pumpOn; // state of the pump

        YAAJ_ModbusMaster controller;

        // Pump speeds in ml/min above which the precision of the pump decreases by a factor of 2
        const int STEP_1 = 63.2;
        const int STEP_2 = 127.5;
        const int STEP_3 = 258;

        // Register values corresponding to the above speeds
        const int STEP_0_CMD = 0x4180; // TODO: confirm this equates to 0 ml/min
        const int STEP_1_CMD = 0x4287;
        const int STEP_2_CMD = 0x4309;
        const int STEP_3_CMD = 0x438B;
        const int STEP_4_CMD = 0x43C7; // TODO: currently 398 ml/min instead of 400, not necessarily important

        // Change in pump speed in ml/min for increase of 1 in register value
        const int RATE_0 = 1 / 0.25;
        const int RATE_1 = 1 / 0.5;
        const int RATE_2 = 1 / 1;
        const int RATE_3 = 1 / 2;
};