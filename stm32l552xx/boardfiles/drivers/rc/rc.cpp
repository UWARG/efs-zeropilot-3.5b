#include <cmath>
#include <cstring>
#include "rc_defines.hpp"
#include "rc.hpp"

uint8_t *bufferOffset;

DataChunk_t channel1[2] = {
    {1, 0xFF, 0},
    {2, 0x07, 8}
};

DataChunk_t channel2[2] = {
    {2, 0xF8, -3},
    {3, 0x3F, 5}
};

DataChunk_t channel3[3] = {
    {3, 0xC0, -6},
    {4, 0xFF, 2},
    {5, 0x01, 10}
};

DataChunk_t channel4[2] = {
    {5, 0xFE, -1},
    {6, 0x0F, 7}
};

DataChunk_t channel5[2] = {
    {6, 0xF0, -4},
    {7, 0x7F, 4}
};

DataChunk_t channel6[3] = {
    {7, 0x80, -7},
    {8, 0xFF, 1},
    {9, 0x03, 9}
};

DataChunk_t channel7[2] = {
    {9, 0xFC, -2},
    {10, 0x1F, 6}
};

DataChunk_t channel8[2] = {
    {10, 0xe0, -5},
    {11, 0xFF, 3}
};

DataChunk_t channel9[2] = {
    {12, 0xFF, 0},
    {13, 0x07, 8}
};

DataChunk_t channel10[2] = {
    {13, 0xF8, -3},
    {14, 0x3F, 5}
};

DataChunk_t channel11[3] = {
    {14, 0xC0, -6},
    {15, 0xFF, 2},
    {16, 0x01, 10}
};

DataChunk_t channel12[2] = {
    {16, 0xFE, -1},
    {17, 0x0F, 7}
};

DataChunk_t channel13[2] = {
    {17, 0xF0, -4},
    {18, 0x7F, 4}
};

DataChunk_t channel14[3] = {
    {18, 0x80, -7},
    {19, 0xFF, 1},
    {20, 0x03, 9}
};

DataChunk_t channel15[2] = {
    {20, 0xFC, -2},
    {21, 0x1F, 6}
};

DataChunk_t channel16[2] = {
    {21, 0xe0, -5},
    {22, 0xFF, 3}
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

float RCReceiver::sbusToRCControl(DataChunk_t *chunks, int length) {
    uint16_t res = 0;

    for (int i = 0; i < length; i++) {
        DataChunk_t dC = *(chunks + i);
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

        rcData_.roll = sbusToRCControl(channel1, sizeof(channel1)/sizeof( DataChunk_t));
        rcData_.pitch = sbusToRCControl(channel2, sizeof(channel2)/sizeof( DataChunk_t));
        rcData_.throttle = sbusToRCControl(channel3, sizeof(channel3)/sizeof( DataChunk_t));
        rcData_.yaw = sbusToRCControl(channel4, sizeof(channel4)/sizeof( DataChunk_t));
        rcData_.arm = sbusToRCControl(channel5, sizeof(channel5)/sizeof( DataChunk_t));
        rcData_.aux1 = sbusToRCControl(channel6, sizeof(channel6)/sizeof( DataChunk_t));
        rcData_.aux2 = sbusToRCControl(channel7, sizeof(channel7)/sizeof( DataChunk_t));
        rcData_.aux3 = sbusToRCControl(channel8, sizeof(channel8)/sizeof( DataChunk_t));
        rcData_.aux4 = sbusToRCControl(channel9, sizeof(channel9)/sizeof( DataChunk_t));
        rcData_.aux5 = sbusToRCControl(channel10, sizeof(channel10)/sizeof( DataChunk_t));
        rcData_.aux6 = sbusToRCControl(channel11, sizeof(channel11)/sizeof( DataChunk_t));
        rcData_.aux7 = sbusToRCControl(channel12, sizeof(channel12)/sizeof( DataChunk_t));
        rcData_.aux8 = sbusToRCControl(channel13, sizeof(channel13)/sizeof( DataChunk_t));
        rcData_.aux9 = sbusToRCControl(channel14, sizeof(channel14)/sizeof( DataChunk_t));
        rcData_.aux10 = sbusToRCControl(channel15, sizeof(channel15)/sizeof( DataChunk_t));
        rcData_.aux11 = sbusToRCControl(channel16, sizeof(channel16)/sizeof( DataChunk_t));

        rcData_.isDataNew = true;
    }
}
