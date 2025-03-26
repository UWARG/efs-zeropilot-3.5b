#pragma once

#include "rc_iface.hpp"
#include "stm32l5xx_hal.h"

typedef struct {
    int dataOffset;
    uint8_t mask;
    int bitshift;
} DataChunk_t;

typedef enum {
    BEGINNING = 0,
    MIDDLE
} ParseStartLocation_e;

class RCReceiver : public IRCReceiver {
    public:
        RCReceiver(UART_HandleTypeDef *uart);

        RCControl getRCData() override;

        void init();

        void parse(ParseStartLocation_e start);
       
    private:
        UART_HandleTypeDef *uart_;
        RCControl rcData_;
        uint8_t rawSbus_[2 * SBUS_BYTE_COUNT];

        float sbusToRCControl(uint8_t *buf, int channelMappingIdx);
};
