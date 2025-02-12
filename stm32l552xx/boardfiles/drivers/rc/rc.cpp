#pragma once

#include "rc.hpp"
#include "rc_defines.hpp"
#include "rc_datatypes.hpp"
#include "stm32l5xx_hal_uart.h"

#include <algorithm>

RCReceiver::RCReceiver(UART_HandleTypeDef* uart) : uart_(uart) {
    // empty
}

void RCReceiver::startDMA() {
    // start circular DMA 
    rcData_.isDataNew = false;
    HAL_UART_Receive_DMA(uart_, rawSbus_, 25);
}

RCControl RCReceiver::getRCData() {
    RCControl tmp = rcData_;
    rcData_.isDataNew = false;
    return tmp;
}

void RCReceiver::parse(bool isBufferStart) {
	uint8_t *bufferOffset;

	if(isBufferStart) {
		bufferOffset = rawSbus_;
	}
    else {
		bufferOffset = rawSbus_ + 25;
	}

    if ((bufferOffset[0] == HEADER_) && (bufferOffset[24] == FOOTER_)) {

        uint16_t tmp;

        tmp = static_cast<int16_t>((bufferOffset[1] | (bufferOffset[2] << 8) ) & 0x07FF );
        rcData_.roll = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[2] >> 3) | (bufferOffset[3] << 5)) & 0x07FF );
        rcData_.pitch = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[3] >> 6) | (bufferOffset[4] << 2)) | (bufferOffset[5] << 10) & 0x07FF );
        rcData_.throttle = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[5] >> 1) | (bufferOffset[6] << 7)) & 0x07FF );
        rcData_.yaw      = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[6] >> 4) | (bufferOffset[7] << 4)) & 0x07FF );
        rcData_.arm      = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[7] >> 7) | (bufferOffset[8] << 1)) | (bufferOffset[9] << 9) & 0x07FF );
        rcData_.aux1     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[9] >> 2) | (bufferOffset[10] << 2) ) & 0x07FF );
        rcData_.aux2     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[10] >> 5)| (bufferOffset[11] << 3) ) & 0x07FF );
        rcData_.aux3     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>((bufferOffset[12] | (bufferOffset[13] << 8) ) & 0x07FF );
        rcData_.aux4     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[13] >> 3) | (bufferOffset[14] << 5) ) & 0x07FF );
        rcData_.aux5     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[14] >> 6) | (bufferOffset[15] << 2) | bufferOffset[16] << 10) & 0x07FF );
        rcData_.aux6     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[16] >> 1) | (bufferOffset[17] << 7) ) & 0x07FF );
        rcData_.aux7     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[17] >> 4) | (bufferOffset[18] << 4) ) & 0x07FF );
        rcData_.aux8     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[18] >> 1) | (bufferOffset[19] << 1) ) | (bufferOffset[20] << 9) & 0x07FF );
        rcData_.aux9     = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[20] >> 2) | (bufferOffset[21] << 6) ) & 0x07FF );
        rcData_.aux10    = sbusToRCControl(tmp);

        tmp = static_cast<int16_t>(((bufferOffset[21] >> 5) | (bufferOffset[22] << 3) ) & 0x07FF );
        rcData_.aux11    = sbusToRCControl(tmp);
        
        rcData_.isDataNew = true;
    }
}


float RCReceiver::sbusToRCControl(uint16_t channelValue) {
    // clamp channel value and scale it to a percentage
    channelValue = std::clamp(channelValue, SBUS_RANGE_MIN, SBUS_RANGE_MAX);
    return static_cast<float>((channelValue - SBUS_RANGE_MIN) * (100.0f / SBUS_RANGE_RANGE));
}
