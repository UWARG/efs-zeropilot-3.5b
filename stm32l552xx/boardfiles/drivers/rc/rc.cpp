#pragma once

#include "rc.hpp"
#include "rc_defines.hpp"
#include "rc_datatypes.hpp"
#include "stm32l5xx_hal_uart.h"

RCReceiver::RCReceiver(UART_HandleTypeDef* uart){

	rc_data.isDataNew = false;
	full_buffer_ = raw_sbus_ + 25;

	HAL_UART_Receive_DMA(uart, raw_sbus_, 25);
}

RCControl RCReceiver::getRCData(){

	for(uint8_t i = 0; i < SBUS_INPUT_CHANNELS; i++){
		rc_data.ControlSignals[0] = sbus_to_rccontrol(received_sbus_.ch[i]);
	}


	rc_data.isDataNew = false;

	return rc_data;

}


void RCReceiver::parse(bool buffer_start){

	uint8_t *buffer_offset_;

	if(buffer_start == 0){
		buffer_offset_ = raw_sbus_;
	}else{
		buffer_offset_ = full_buffer_;
	}

    if ((buffer_offset_[0] == HEADER_) && (buffer_offset_[24] == FOOTER_)){
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

        received_sbus_.ch17 = buffer_offset_[23] & CH17_MASK_;
        received_sbus_.ch18 = buffer_offset_[23] & CH17_MASK_;
        received_sbus_.lost_frame = buffer_offset_[23] & LOST_FRAME_MASK_;
        received_sbus_.failsafe = buffer_offset_[23] & FAILSAFE_MASK_;
   
        rc_data.isDataNew = true;
    }else{

        rc_data.isDataNew = false;
    }
}


float RCReceiver::sbus_to_rccontrol(uint16_t channel_value){
    if(channel_value < SBUS_RANGE_MIN){
        channel_value = SBUS_RANGE_MIN;
    }else if(channel_value > SBUS_RANGE_MAX){
        channel_value = SBUS_RANGE_MAX;
    }
    
    return static_cast<float>((channel_value - SBUS_RANGE_MIN) * (100.0f / SBUS_RANGE_RANGE));
}

