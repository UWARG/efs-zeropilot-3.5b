#pragma once

#include "stm32l5xx_hal_uart.h"
#include "rc_iface.hpp"
#include "rc_datatypes.hpp" 

class RCReceiver : public IRCReceiver {
    public:
        RCReceiver(UART_HandleTypeDef *uart);

        RCControl getRCData() override;

        void parse(bool isBufferStart);
       
    private:
        UART_HandleTypeDef *uart_;
        RCControl rcData_;
        uint8_t rawSbus_[50];

        float sbusToRCControl(uint16_t channelValue);
};
