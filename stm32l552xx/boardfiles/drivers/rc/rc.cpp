#pragma once

#include "rc.hpp"
#include "rc_defines.hpp"
#include "rc_datatypes.hpp"
#include "stm32l5xx_hal_uart.h"


uint8_t *bufferOffset;

DataChunk_t channel1[2] {
    {bufferOffset[1], 0xFF, 0},
    {bufferOffset[2], 0x07, 8}
};

DataChunk_t channel2[2] {
    {bufferOffset[2], 0xF8, -3},
    {bufferOffset[3], 0x3F, 5}
};

DataChunk_t channel3[3] {
    {bufferOffset[3], 0xC0, -6},
    {bufferOffset[4], 0xFF, 2},
    {bufferOffset[5], 0x01, 10}
};

DataChunk_t channel4[2] {
    {bufferOffset[5], 0xFE, -1},
    {bufferOffset[6], 0x0F, 7}
};

DataChunk_t channel5[2] {
    {bufferOffset[6], 0xF0, -4},
    {bufferOffset[7], 0x7F, 4}
};

DataChunk_t channel6[3] {
    {bufferOffset[7], 0x80, -7},
    {bufferOffset[8], 0xFF, 1},
    {bufferOffset[9], 0x03, 9}
};

DataChunk_t channel7[2] {
    {bufferOffset[9], 0xFC, -2},
    {bufferOffset[10], 0x1F, 6}
};

DataChunk_t channel8[2] {
    {bufferOffset[10], 0xe0, -5},
    {bufferOffset[11], 0xFF, 3}
};

DataChunk_t channel9[2] {
    {bufferOffset[12], 0xFF, 0},
    {bufferOffset[13], 0x07, 8}
};

DataChunk_t channel10[2] {
    {bufferOffset[13], 0xF8, -3},
    {bufferOffset[14], 0x3F, 5}
};

DataChunk_t channel11[3] {
    {bufferOffset[14], 0xC0, -6},
    {bufferOffset[15], 0xFF, 2},
    {bufferOffset[16], 0x01, 10}
};

DataChunk_t channel12[2] {
    {bufferOffset[16], 0xFE, -1},
    {bufferOffset[17], 0x0F, 7}
};

DataChunk_t channel13[2] {
    {bufferOffset[17], 0xF0, -4},
    {bufferOffset[18], 0x7F, 4}
};

DataChunk_t channel14[3] {
    {bufferOffset[18], 0x80, -7},
    {bufferOffset[19], 0xFF, 1},
    {bufferOffset[20], 0x03, 9}
};

DataChunk_t channel15[2] {
    {bufferOffset[20], 0xFC, -2},
    {bufferOffset[21], 0x1F, 6}
};

DataChunk_t channel16[2] {
    {bufferOffset[21], 0xe0, -5},
    {bufferOffset[22], 0xFF, 3}
};

RCReceiver::RCReceiver(UART_HandleTypeDef* uart) : uart_(uart) {
    // empty
}

void RCReceiver::startDMA() {
    // start circular DMA 
    rcData_.isDataNew = false;
    HAL_UART_Receive_DMA(uart_, rawSbus_, 50);
}

RCControl RCReceiver::getRCData() {
    RCControl tmp = rcData_;
    rcData_.isDataNew = false;
    return tmp;
}

float RCReceiver::sbusToRCControl(DataChunk_t *chunks, int length) {
    uint16_t res = 0;

    for (int i = 0; i < length; i++) {
        DataChunk_t dC = *(chunks + i);
        uint16_t tmp = dC.bitshift >= 0 ?
            dC.rawDataByte & dC.mask << dC.bitshift :
            dC.rawDataByte & dC.mask >> dC.bitshift;
        res |= tmp;
    }
    
    res = (res < SBUS_RANGE_MIN) ? SBUS_RANGE_MIN : (res > SBUS_RANGE_MAX) ? SBUS_RANGE_MAX : res;
    return static_cast<float>((res - SBUS_RANGE_MIN) * (100.0f / SBUS_RANGE_RANGE));
}

void RCReceiver::parse(bool isBufferStart) {

    bufferOffset = isBufferStart ? rawSbus_ : rawSbus_ + 25;

    if ((bufferOffset[0] == HEADER_) && (bufferOffset[24] == FOOTER_)) {
        uint16_t tmp;

        sbusToRCControl(channel1, sizeof(channel1)/sizeof( DataChunk_t));
        sbusToRCControl(channel2, sizeof(channel2)/sizeof( DataChunk_t));
        sbusToRCControl(channel3, sizeof(channel3)/sizeof( DataChunk_t));
        sbusToRCControl(channel4, sizeof(channel4)/sizeof( DataChunk_t));
        sbusToRCControl(channel5, sizeof(channel5)/sizeof( DataChunk_t));
        sbusToRCControl(channel6, sizeof(channel6)/sizeof( DataChunk_t));
        sbusToRCControl(channel7, sizeof(channel7)/sizeof( DataChunk_t));
        sbusToRCControl(channel8, sizeof(channel8)/sizeof( DataChunk_t));
        sbusToRCControl(channel9, sizeof(channel9)/sizeof( DataChunk_t));
        sbusToRCControl(channel10, sizeof(channel10)/sizeof( DataChunk_t));
        sbusToRCControl(channel11, sizeof(channel11)/sizeof( DataChunk_t));
        sbusToRCControl(channel12, sizeof(channel12)/sizeof( DataChunk_t));
        sbusToRCControl(channel13, sizeof(channel13)/sizeof( DataChunk_t));
        sbusToRCControl(channel14, sizeof(channel14)/sizeof( DataChunk_t));
        sbusToRCControl(channel15, sizeof(channel15)/sizeof( DataChunk_t));
        sbusToRCControl(channel16, sizeof(channel16)/sizeof( DataChunk_t));

        rcData_.isDataNew = true;
    }
}
