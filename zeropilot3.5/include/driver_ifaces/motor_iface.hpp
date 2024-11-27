#pragma once
#include <cstdint>

class IMotorControl {
protected: 
    IMotorControl() = default;
public:
    virtual ~IMotorControl() = default;

    // Set the motor speed
    virtual void set(uint8_t percent) = 0;
};
