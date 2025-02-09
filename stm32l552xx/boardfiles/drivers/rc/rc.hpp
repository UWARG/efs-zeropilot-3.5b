#pragma once

#include "stm32l5xx_hal_uart.h"
#include "rc_iface.hpp"
#include "rc_datatypes.hpp" 

class RCReceiver : public IRCReceiver {
    public:
        RCReceiver(UART_HandleTypeDef *uart);

        RCControl getRCData() override;
       
    private:
        UART_HandleTypeDef *uart_;
        RCControl rcData_;
        uint8_t rawSbus_[50];

        void parse(bool isBufferStart);

        float sbusToRCControl(uint16_t channelValue);
};
