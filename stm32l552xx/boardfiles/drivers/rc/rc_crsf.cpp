#include <cmath>
#include <cstring>
#include <cstdio>
#include "rc_defines.hpp"
#include "rc_crsf.hpp"

CRSFReceiver::CRSFReceiver(UART_HandleTypeDef* uart) : uart_(uart) {
    memset(rawCrsf_, 0, CRSF_BYTE_COUNT);
}

RCControl CRSFReceiver::getRCData() {
    RCControl tmp = rcData_;
    rcData_.isDataNew = false;
    return tmp;
}

void CRSFReceiver::init() {
    // start circular DMA
    rcData_.isDataNew = false;
    HAL_UARTEx_ReceiveToIdle_DMA(uart_, rawCrsf_, CRSF_BYTE_COUNT);
}

void CRSFReceiver::startDMA() {
    // start circular DMA
    HAL_UARTEx_ReceiveToIdle_DMA(uart_, rawCrsf_, CRSF_BYTE_COUNT);
}

void CRSFReceiver::parse() {

    uint8_t *buf = rawCrsf_;

    // Validate sync byte and frame type
   if (buf[0] != CRSF_SYNC_BYTE || buf[2] != CRSF_FRAMETYPE_RC_CHANNELS_PACKED) {
       return;
   }

    // Extract payload
    const uint8_t *payload = &buf[3];

    // Decode channels
    uint16_t channels[16];
    uint32_t bitBuffer = 0;
    uint8_t bitsInBuffer = 0;
    uint8_t payloadIndex = 0;

    for (int i = 0; i < 16; ++i) {
        while (bitsInBuffer < 11) {
            bitBuffer |= ((uint32_t)payload[payloadIndex++]) << bitsInBuffer;
            bitsInBuffer += 8;
        }
        channels[i] = bitBuffer & 0x7FF; // Extract 11 bits
        bitBuffer >>= 11;
        bitsInBuffer -= 11;

        // Map to desired range
        rcData_.ControlSignals[i] = static_cast<float>((channels[i] - CRSF_RANGE_MIN) * (100.0f / CRSF_RANGE_RANGE));
    }

    rcData_.isDataNew = true;
}
