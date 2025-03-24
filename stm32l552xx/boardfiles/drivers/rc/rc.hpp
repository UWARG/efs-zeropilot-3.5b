#pragma once

#include "rc_iface.hpp"
#include "stm32l5xx_hal.h"


typedef struct {
    int dataOffset;
    uint8_t mask;
    int bitshift;
} DataChunk_t;

class RCReceiver : public IRCReceiver {
    public:
        RCReceiver(UART_HandleTypeDef *uart);

        void startDMA();

        RCControl getRCData() override;

        void parse(bool isBufferStart);
       
    private:
        UART_HandleTypeDef *uart_;
        RCControl rcData_;
        uint8_t rawSbus_[50];

        float sbusToRCControl(int channelNum, int length);
};
