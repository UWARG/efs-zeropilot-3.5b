#pragma once

#include "rc_defines.hpp"
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

        RCControl getRCData() override;
        /**
         * @brief starts DMA receive
         */
        void init();

        /**
         * @brief restarts DMA
         */
        void startDMA();
        /**
         * @brief Updates RCControl values
         */
        void parse();
       
    private:
        UART_HandleTypeDef *uart_;
        RCControl rcData_;
        uint8_t rawSbus_[SBUS_BYTE_COUNT];

        float sbusToRCControl(uint8_t *buf, int channelMappingIdx);
};
