#include "rfd.hpp"
#include "stm32l5xx_hal_uart.h"

RFD* RFD::instance = nullptr; // global instance

RFD::RFD(UART_HandleTypeDef* huart) : huart(huart), readIndex(0), writeIndex(0), overlapped(false), errorFlag(true){
    instance = this;
}

RFD::~RFD() {
    instance = nullptr;
}

void RFD::transmit(const uint8_t* data, uint16_t size) {
    if (huart) {
        HAL_UART_Transmit_DMA(huart, data, size);
    }
}

void RFD::startReceive(uint8_t* buffer, uint16_t bufferSize) {
    if (huart) {
        HAL_UARTEx_ReceiveToIdle_DMA(huart, rxBuffer, BUFFER_SIZE);
    }
}

uint16_t RFD::receive(uint8_t* buffer, uint16_t bufferSize) {
    for (uint16_t i = 0; i < bufferSize; i++) {
        if (!overlapped) {
            // Stop reading if readIndex catches up to writeIndex.
            if (readIndex == writeIndex) {
                return i;
            }
            buffer[i] = rxBuffer[readIndex];
            readIndex++;
        } else {
            // Read until the end of the buffer, then wrap around.
            buffer[i] = rxBuffer[readIndex];
            readIndex++;
            if (readIndex >= BUFFER_SIZE) {
                readIndex = 0;
                overlapped = false;
            }
        }
    }
    return bufferSize;
}

void RFD::receiveCallback(uint16_t size){
    
    uint16_t prevWriteIndex = writeIndex;
    writeIndex = size % BUFFER_SIZE; // don't need % BUFFER_SIZE here

    // assumes that size < BUFFER_LENGTH always
    if (writeIndex < prevWriteIndex) overlapped = true;

    // Overflow: writeIndex wrapped around and surpassed readIndex. (reads not fast enough)
    if (overlapped && (writeIndex > readIndex)) {
        errorFlag = true;
        overlapped = false;
    }

    HAL_UARTEx_ReceiveToIdle_DMA(huart, rxBuffer, BUFFER_SIZE);
}

UART_HandleTypeDef* RFD::getHuart() const {
    return huart;
}

bool RFD::getErrorFlag() {
    return errorFlag;
}
