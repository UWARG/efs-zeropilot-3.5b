#pragma once
#include <cstdint>

class IMotorControl {
protected: 
    MotorControl() = default;
public:
    virtual ~IMotorControl() = default;

    // Set the motor speed
    virtual void set(uint8_t percent) = 0;

};

#endif // MOTOR_CONTROL_HPP
