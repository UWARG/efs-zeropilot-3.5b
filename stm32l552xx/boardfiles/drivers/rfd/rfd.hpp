#pragma once

#include <stm32l5xx_hal_uart.h>
#include "rfd_iface.hpp"
#include "rfd_defines.hpp"

#ifdef __cplusplus
#include <cstdint>
#endif


class RFD : public IRFD {

public:

    static RFD* instance; // assumes only one instance defined at a time

    RFD(UART_HandleTypeDef* huart);
    ~RFD();

    void startReceive(uint8_t* buffer, uint16_t bufferSize) override;
    void transmit(const uint8_t* data, uint16_t size) override;
    uint16_t receive(uint8_t* buffer, uint16_t bufferSize) override;

    // Getters
    UART_HandleTypeDef* getHuart() const;
    bool getErrorFlag() override;

    // DMA callback
    void receiveCallback(uint16_t size);

private:
    UART_HandleTypeDef* huart;
    uint8_t rxBuffer[BUFFER_SIZE];
    uint16_t writeIndex;
    uint16_t readIndex;

    /**
     * Flag to keep track state in the circular buffer and detect error conditions
     * 
     * `true` when writeIndex < readIndex, `false` when writeIndex > readIndex
     */
    bool overlapped;

    /**
     * Flag to record that condition when writeIndex wraps around buffer &
     * overwrites data that is yet to be read
     */
    bool errorFlag;
};
