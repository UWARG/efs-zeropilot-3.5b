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
        __int8_t received_sbus_[16];
        __int8_t raw_sbus_[24];
        bool is_data_new_;


        SBus_t getSBusData();
        void parse();
        float sbus_to_rccontrol(uint16_t channel_value);

};
