#pragma once

#include "stm32l5xx_hal_uart.h"
#include "rc_iface.hpp"
#include "rc_datatypes.hpp" 

class RCReceiver : public IRCReceiver{
    public:

        RCReceiver(UART_HandleTypeDef* uart);

        RCControl getRCData() override;
       
    private:

        SBus_t received_sbus_;
        RCControl rc_data;
        uint8_t raw_sbus_[50];
        uint8_t *full_buffer_;

        void parse();
        float sbus_to_rccontrol(uint16_t channel_value);

};
