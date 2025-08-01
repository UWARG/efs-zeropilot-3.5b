#pragma once

#include <cstdint>

class IRFD {
protected:
	IRFD() = default;

public:
    virtual ~IRFD() = default;
    virtual void startReceive() = 0;
    virtual void transmit(const uint8_t* data, uint16_t size) = 0;
    virtual uint16_t receive(uint8_t* buffer, uint16_t bufferSize) = 0;
    virtual bool getErrorFlag() const = 0;
    virtual void resetErrorFlag() = 0;
};
