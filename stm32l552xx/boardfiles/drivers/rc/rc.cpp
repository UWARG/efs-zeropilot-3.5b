#pragma omce

#include "rc.hpp"
#include "rc_defines.hpp"

RCReceiver::IRCReceiver(UART_HandleTypeDef* uart) : uart_(uart){
    //TO DO
}

RCControl RCReceiver::getRCData(){
    //TO DO 
}

SBus_t RCReceiver::getSBusData() {
    //TO DO 
}

void RCReceiver::parse(){
    if ((raw_sbus_[0] == HEADER_) && (raw_sbus_[24] == FOOTER_)){
        received_sbus_.ch[0] = static_cast<int16_t>((raw_sbus_[1] | (raw_sbus_[2] << 8) ) & 0x07FF );
        received_sbus_.ch[1] = static_cast<int16_t>(((raw_sbus_[2] >> 3) | (raw_sbus_[3] << 5)) & 0x07FF );
        received_sbus_.ch[2] = static_cast<int16_t>(((raw_sbus_[3] >> 6) | (raw_sbus_[4] << 2)) | (raw_sbus_[5] << 10) & 0x07FF );
        received_sbus_.ch[3] = static_cast<int16_t>(((raw_sbus_[5] >> 1) | (raw_sbus_[6] << 7)) & 0x07FF );
        received_sbus_.ch[4] = static_cast<int16_t>(((raw_sbus_[6] >> 4) | (raw_sbus_[7] << 4)) & 0x07FF );
        received_sbus_.ch[5] = static_cast<int16_t>(((raw_sbus_[7] >> 7) | (raw_sbus_[8] << 1)) | (raw_sbus_[9] << 9) & 0x07FF );
        received_sbus_.ch[6] = static_cast<int16_t>(((raw_sbus_[9] >> 2) | (raw_sbus_[10] << 2) ) & 0x07FF );
        received_sbus_.ch[7] = static_cast<int16_t>(((raw_sbus_[10] >> 5) | (raw_sbus_[11] << 3) ) & 0x07FF );
        received_sbus_.ch[8] = static_cast<int16_t>((raw_sbus_[12] | (raw_sbus_[13] << 8) ) & 0x07FF );
        received_sbus_.ch[9] = static_cast<int16_t>(((raw_sbus_[13] >> 3) | (raw_sbus_[14] << 5) ) & 0x07FF );
        received_sbus_.ch[10] = static_cast<int16_t>(((raw_sbus_[14] >> 6) | (raw_sbus_[15] << 2) | raw_sbus_[16] << 10) & 0x07FF );
        received_sbus_.ch[11] = static_cast<int16_t>(((raw_sbus_[16] >> 1) | (raw_sbus_[17] << 7) ) & 0x07FF );
        received_sbus_.ch[12] = static_cast<int16_t>(((raw_sbus_[17] >> 4) | (raw_sbus_[18] << 4) ) & 0x07FF );
        received_sbus_.ch[13] = static_cast<int16_t>(((raw_sbus_[18] >> 1) | (raw_sbus_[19] << 1) ) | (raw_sbus_[20] << 9) & 0x07FF );
        received_sbus_.ch[14] = static_cast<int16_t>(((raw_sbus_[20] >> 2) | (raw_sbus_[21] << 6) ) & 0x07FF );
        received_sbus_.ch[15] = static_cast<int16_t>(((raw_sbus_[21] >> 5) | (raw_sbus_[22] << 3) ) & 0x07FF );

        received_sbus_.ch17 = raw_sbus[23] & CH17_MASK_;
        received_sbus_.ch18 = raw_sbus_[23] & CH17_MAKSK_;
        received_sbus_.lost_frame = raw_sbus_[23] & LOST_FRAME_MASK_;
        received_sbus_.failsafe = raw_sbus_[23] & FAILSAFE_MASK_:
   
        received_sbus_.isDataNew = true;
    }else{

        received_sbus_.isDataNew = false;
    }
}

float RCReceiver::sbus_to_rccontrol(uint16_t channel_value){
    if(channel_value < SBUS_RANGE_MIN)
        channel_value = SBUS_RANGE_MIN
    else if(channel_value > SBUS_RANGE_MAX)
        channel_value = SBUS_RANGE_MAX;
    
    return static__cast<float>((channnel_value - SBUS_RANGE_MIN) * (100.0f / SBUS_RANGE_RANGE))
}