#include <cmath>
#include <cstring>
#include "rc_defines.hpp"
#include "rc.hpp"

uint8_t *bufferOffset;

DataChunk_t channelList[16][3] = {
    { //channel 1
        {1, 0xFF, 0}, {2, 0x07, 8}, {0, 0, 0}
    },
    { //channel 2
        {2, 0xF8, -3}, {3, 0x3F, 5}, {0, 0, 0}
    },
    { //channel 3
        {3, 0xC0, -6}, {4, 0xFF, 2}, {5, 0x01, 10}
    },
    { //channel 4
        {5, 0xFE, -1}, {6, 0x0F, 7}, {0, 0 , 0}
    },
    { //channel 5
        {6, 0xF0, -4}, {7, 0x7F, 4}, {0, 0, 0}
    },
    { //channel 6
        {7, 0x80, -7}, {8, 0xFF, 1}, {9, 0x03, 9}
    },
    { //channel 7
        {9, 0xFC, -2}, {10, 0x1F, 6}, {0, 0, 0}
    },
    { //channel 8
        {10, 0xe0, -5}, {11, 0xFF, 3}, {0, 0, 0}
    },
    { //channel 9
        {12, 0xFF, 0}, {13, 0x07, 8}, {0, 0, 0}
    },
    { //channel 10
        {13, 0xF8, -3}, {14, 0x3F, 5}, {0, 0, 0}
    },
    { //channel 11
        {14, 0xC0, -6}, {15, 0xFF, 2}, {16, 0x01, 10}
    },
    { //channel 12
        {16, 0xFE, -1}, {17, 0x0F, 7}, {0, 0, 0}
    },
    { //channel 13
        {17, 0xF0, -4}, {18, 0x7F, 4}, {0, 0, 0}
    },
    { //channel 14
        {18, 0x80, -7}, {19, 0xFF, 1}, {20, 0x03, 9}
    },
    { //channel 15
        {20, 0xFC, -2}, {21, 0x1F, 6}, {0, 0, 0}
    },
    { //channel 16
        {21, 0xe0, -5}, {22, 0xFF, 3}, {0, 0, 0}
    }
};

RCReceiver::RCReceiver(UART_HandleTypeDef* uart) : uart_(uart) {
    memset(rawSbus_, 0, 50);
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

float RCReceiver::sbusToRCControl(int channelNum, int length) {
    uint16_t res = 0;

    for (int i = 0; i < length; i++) {
        DataChunk_t dC = channelList[channelNum-1][i];
        uint16_t tmp = dC.bitshift >= 0 ?
            (bufferOffset[dC.dataOffset] & dC.mask) << dC.bitshift :
            (bufferOffset[dC.dataOffset] & dC.mask) >> abs(dC.bitshift);
        res |= tmp;
    }

    res = (res < SBUS_RANGE_MIN) ? SBUS_RANGE_MIN : (res > SBUS_RANGE_MAX) ? SBUS_RANGE_MAX : res;
    return static_cast<float>((res - SBUS_RANGE_MIN) * (100.0f / SBUS_RANGE_RANGE));
}

void RCReceiver::parse(bool isBufferStart) {

    bufferOffset = isBufferStart ? rawSbus_ : rawSbus_ + 25;

    if ((bufferOffset[0] == HEADER_) && (bufferOffset[24] == FOOTER_)) {

        for (int i = 0; i < 16; i++) {
          rcData_.ControlSignals[i] = sbusToRCControl(i+1, 3);
        }

        rcData_.isDataNew = true;
    }
}
