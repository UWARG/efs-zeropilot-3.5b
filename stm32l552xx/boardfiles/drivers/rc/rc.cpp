#pragma once

#include "rc.hpp"
#include "rc_defines.hpp"
#include "rc_datatypes.hpp"
#include "stm32l5xx_hal_uart.h"

RCReceiver::RCReceiver(UART_HandleTypeDef* uart) : uart_(uart) {
    // empty
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
        // no need for received_sbus_
        // tmp = static_cast(. . .)
        // rcData_.(. . .) = sbusToRCControl(tmp)

        received_sbus_.ch[0]  = static_cast<int16_t>((buffer_offset_ [1] | (buffer_offset_[2] << 8) ) & 0x07FF );
        received_sbus_.ch[1]  = static_cast<int16_t>(((buffer_offset_[2] >> 3) | (buffer_offset_[3] << 5)) & 0x07FF );
        received_sbus_.ch[2]  = static_cast<int16_t>(((buffer_offset_[3] >> 6) | (buffer_offset_[4] << 2)) | (buffer_offset_[5] << 10) & 0x07FF );
        received_sbus_.ch[3]  = static_cast<int16_t>(((buffer_offset_[5] >> 1) | (buffer_offset_[6] << 7)) & 0x07FF );
        received_sbus_.ch[4]  = static_cast<int16_t>(((buffer_offset_[6] >> 4) | (buffer_offset_[7] << 4)) & 0x07FF );
        received_sbus_.ch[5]  = static_cast<int16_t>(((buffer_offset_[7] >> 7) | (buffer_offset_[8] << 1)) | (buffer_offset_[9] << 9) & 0x07FF );
        received_sbus_.ch[6]  = static_cast<int16_t>(((buffer_offset_[9] >> 2) | (buffer_offset_[10] << 2) ) & 0x07FF );
        received_sbus_.ch[7]  = static_cast<int16_t>(((buffer_offset_[10] >> 5)| (buffer_offset_[11] << 3) ) & 0x07FF );
        received_sbus_.ch[8]  = static_cast<int16_t>((buffer_offset_ [12] | (buffer_offset_[13] << 8) ) & 0x07FF );
        received_sbus_.ch[9]  = static_cast<int16_t>(((buffer_offset_[13] >> 3) | (buffer_offset_[14] << 5) ) & 0x07FF );
        received_sbus_.ch[10] = static_cast<int16_t>(((buffer_offset_[14] >> 6) | (buffer_offset_[15] << 2) | buffer_offset_[16] << 10) & 0x07FF );
        received_sbus_.ch[11] = static_cast<int16_t>(((buffer_offset_[16] >> 1) | (buffer_offset_[17] << 7) ) & 0x07FF );
        received_sbus_.ch[12] = static_cast<int16_t>(((buffer_offset_[17] >> 4) | (buffer_offset_[18] << 4) ) & 0x07FF );
        received_sbus_.ch[13] = static_cast<int16_t>(((buffer_offset_[18] >> 1) | (buffer_offset_[19] << 1) ) | (buffer_offset_[20] << 9) & 0x07FF );
        received_sbus_.ch[14] = static_cast<int16_t>(((buffer_offset_[20] >> 2) | (buffer_offset_[21] << 6) ) & 0x07FF );
        received_sbus_.ch[15] = static_cast<int16_t>(((buffer_offset_[21] >> 5) | (buffer_offset_[22] << 3) ) & 0x07FF );

        rcData_.isDataNew = true;
    }
}


float RCReceiver::sbusToRCControl(uint16_t channelValue) {
    // use std::clamp

    return static_cast<float>((channelValue - SBUS_RANGE_MIN) * (100.0f / SBUS_RANGE_RANGE));
}
