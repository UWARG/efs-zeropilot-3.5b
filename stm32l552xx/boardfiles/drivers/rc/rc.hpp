#pragma once

#include "rc_defines.hpp"
#include "rc_iface.hpp"
#include "stm32l5xx_hal.h"
#include "error.h"

typedef struct {
    int dataOffset;
    uint8_t mask;
    int bitshift;
} DataChunk_t;

class RCReceiver : public IRCReceiver {
    public:
        RCReceiver(UART_HandleTypeDef *uart);

        ZP_ERROR_e getRCData(RCControl *data) override;

        ZP_ERROR_e init();
        ZP_ERROR_e startDMA();

        ZP_ERROR_e parse();

    private:
        UART_HandleTypeDef *uart_;
        RCControl rcData_;
        uint8_t rawSbus_[SBUS_BYTE_COUNT];

        ZP_ERROR_e sbusToRCControl(float *value, uint8_t *buf, int channelMappingIdx);
};
